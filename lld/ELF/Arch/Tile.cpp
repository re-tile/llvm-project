//===- Tile.cpp -----------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// TILE-Gx. Instructions are 64-bit bundles of two (X mode) or three (Y mode)
// slot instructions, and relocations patch an operand field of one slot. The
// PLT, GOT layout and TLS sequence rewrites follow binutils' elfxx-tilegx.c,
// which glibc's tile dynamic linker was written against.
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "RelocScan.h"
#include "Symbols.h"
#include "SymbolTable.h"
#include "SyntheticSections.h"
#include "Target.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {
class Tile final : public TargetInfo {
public:
  Tile(Ctx &);
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  RelType getDynRel(RelType type) const override;
  void writeGotHeader(uint8_t *buf) const override;
  void writeGotPltHeader(uint8_t *buf) const override;
  void writeGotPlt(uint8_t *buf, const Symbol &s) const override;
  void writePltHeader(uint8_t *buf) const override;
  void writePlt(uint8_t *buf, const Symbol &sym,
                uint64_t pltEntryAddr) const override;
  template <class ELFT, class RelTy>
  void scanSectionImpl(InputSectionBase &sec, Relocs<RelTy> rels,
                       unsigned shard);
  void scanSection(InputSectionBase &sec, unsigned shard) override {
    elf::scanSection1<Tile, ELF64LE>(*this, sec, shard);
  }
  void finalizeRelocScan() override;
  void relocate(uint8_t *loc, const Relocation &rel,
                uint64_t val) const override;
};
} // namespace

// Operand field placement within a bundle, from binutils' opcode/tilegx.h.
static uint64_t imm16X0(uint64_t v) { return (v & 0xffff) << 12; }
static uint64_t imm16X1(uint64_t v) { return (v & 0xffff) << 43; }
static uint64_t imm8X0(uint64_t v) { return (v & 0xff) << 12; }
static uint64_t imm8X1(uint64_t v) { return (v & 0xff) << 43; }
static uint64_t shamtX0(uint64_t v) { return (v & 0x3f) << 12; }
static uint64_t shamtX1(uint64_t v) { return (v & 0x3f) << 43; }
static uint64_t brOffX1(uint64_t v) {
  return ((v & 0x3f) << 31) | ((v & 0x1ffc0) << 37);
}
static uint64_t jumpOffX1(uint64_t v) { return (v & 0x7ffffff) << 31; }
static uint64_t destImm8X1(uint64_t v) {
  return ((v & 0x3f) << 31) | ((v & 0xc0) << 43);
}
static uint64_t mtImm14X1(uint64_t v) {
  return ((v & 0x3f) << 31) | ((v & 0x3fc0) << 37);
}
static uint64_t mfImm14X1(uint64_t v) { return (v & 0x3fff) << 37; }
static uint64_t bfStartX0(uint64_t v) { return (v & 0x3f) << 18; }
static uint64_t bfEndX0(uint64_t v) { return (v & 0x3f) << 12; }

// Merge `field(v)` into the bundle at loc, touching only the bytes the field
// covers. A field that ends in the low word may sit in a 32-bit datum.
static void insert(uint8_t *loc, uint64_t (*field)(uint64_t), uint64_t v) {
  uint64_t mask = field(~uint64_t(0));
  uint64_t bits = field(v);
  for (; mask != 0; ++loc, mask >>= 8, bits >>= 8)
    *loc = (*loc & ~uint8_t(mask)) | (uint8_t(bits) & uint8_t(mask));
}

static void replaceInsn(uint8_t *loc, uint64_t mask, uint64_t insn) {
  write64le(loc, (read64le(loc) & ~mask) | (insn & mask));
}

static void copyBits(uint8_t *loc, unsigned from, unsigned to, unsigned size) {
  uint64_t fieldMask = (uint64_t(1) << size) - 1;
  uint64_t v = read64le(loc);
  uint64_t f = (v >> from) & fieldMask;
  write64le(loc, (v & ~(fieldMask << to)) | (f << to));
}

// Slot masks and synthesized instructions for the TLS sequence rewrites. The
// values are elfxx-tilegx.c's byte arrays read as little-endian words.
static constexpr uint64_t maskX1 = 0x3fffffff80000000;
static constexpr uint64_t maskX0NoDestNoSrcA = 0x000000007ffff000;
static constexpr uint64_t maskX1NoDestNoSrcA = 0x3ffff80000000000;
static constexpr uint64_t maskY0NoDestNoSrcA = 0x00000000780ff000;
static constexpr uint64_t maskY1NoDestNoSrcA = 0x3c07f80000000000;
static constexpr uint64_t maskX0NoOperand = 0x000000007ffc0000;
static constexpr uint64_t maskX1NoOperand = 0x3ffe000000000000;
static constexpr uint64_t maskY0NoOperand = 0x00000000780c0000;
static constexpr uint64_t maskY1NoOperand = 0x3c06000000000000;
static constexpr uint64_t ldR0R0X1 = 0x286ae80000000000;
static constexpr uint64_t addR0R0TpX0X1 = 0x2807a800500f5000;
static constexpr uint64_t addR0R0TpY0Y1 = 0x9a13a8002c275000;
static constexpr uint64_t moveR0R0X0X1 = 0x283bf8005107f000;
static constexpr uint64_t moveR0R0Y0Y1 = 0xae05f800540bf000;

Tile::Tile(Ctx &ctx) : TargetInfo(ctx) {
  copyRel = R_TILEGX_COPY;
  gotRel = R_TILEGX_GLOB_DAT;
  pltRel = R_TILEGX_JMP_SLOT;
  relativeRel = R_TILEGX_RELATIVE;
  symbolicRel = R_TILEGX_64;
  tlsModuleIndexRel = R_TILEGX_TLS_DTPMOD64;
  tlsOffsetRel = R_TILEGX_TLS_DTPOFF64;
  tlsGotRel = R_TILEGX_TLS_TPOFF64;

  // .got[0] = _DYNAMIC. .got.plt[0] and [1] are the link map and the lazy
  // resolver, both stored by ld.so.
  gotHeaderEntriesNum = 1;
  gotPltHeaderEntriesNum = 2;

  pltHeaderSize = 24;
  pltEntrySize = 40;
  ipltEntrySize = 40;
  defaultMaxPageSize = 65536;
}

RelExpr Tile::getRelExpr(RelType type, const Symbol &s,
                         const uint8_t *loc) const {
  switch (type) {
  case R_TILEGX_NONE:
    return R_NONE;
  case R_TILEGX_64:
  case R_TILEGX_32:
  case R_TILEGX_16:
  case R_TILEGX_8:
    return R_ABS;
  case R_TILEGX_64_PCREL:
  case R_TILEGX_32_PCREL:
    return R_PC;
  case R_TILEGX_TLS_DTPOFF64:
  case R_TILEGX_TLS_DTPOFF32:
    return R_DTPREL;
  default:
    Err(ctx) << getErrorLoc(ctx, loc) << "unknown relocation (" << type.v
             << ") against symbol " << &s;
    return R_NONE;
  }
}

RelType Tile::getDynRel(RelType type) const {
  return type == R_TILEGX_64 ? type : RelType(R_TILEGX_NONE);
}

void Tile::writeGotHeader(uint8_t *buf) const {
  write64le(buf, ctx.in.dynamic->getVA());
}

void Tile::writeGotPltHeader(uint8_t *buf) const {
  write64le(buf, -1);
  write64le(buf + 8, 0);
}

void Tile::writeGotPlt(uint8_t *buf, const Symbol &) const {
  write64le(buf, ctx.in.plt->getVA());
}

void Tile::writePltHeader(uint8_t *buf) const {
  // An entry jumps here with &.got.plt[0] in r27 and its index in r29.
  //   { ld_add r28, r27, 8 }   r28 = .got.plt[0], the link map
  //   { ld r27, r27 }          r27 = .got.plt[1], the resolver
  //   { info 10 ; jr r27 }
  const uint8_t header[] = {
      0x00, 0x30, 0x48, 0x51, 0x6e, 0x43, 0xa0, 0x18,
      0x00, 0x30, 0xbc, 0x35, 0x00, 0x40, 0xde, 0x9e,
      0xff, 0xaf, 0x30, 0x40, 0x60, 0x73, 0x6a, 0x28,
  };
  memcpy(buf, header, sizeof(header));
}

void Tile::writePlt(uint8_t *buf, const Symbol &sym,
                    uint64_t pltEntryAddr) const {
  // Both forms load the .got.plt slot relative to the lnk in their first
  // bundle and jump through it with r29 = the PLT index and r27 = &.got.plt[0].
  // The short form needs both offsets to fit in 16 bits.
  //   { lnk r26 }
  //   { addli r28, r26, gotEntry ; moveli r27, got0 }
  //   { shl16insli r29, zero, index ; ld r28, r28 }
  //   { add r27, r26, r27 ; jr r28 }
  //   { info 10 }
  // The long form builds each offset from two halfwords.
  //   { moveli r28, gotEntry >> 16 ; lnk r26 }
  //   { moveli r27, got0 >> 16 ; shl16insli r28, r28, gotEntry }
  //   { add r28, r26, r28 ; shl16insli r27, r27, got0 }
  //   { add r27, r26, r27 ; info 10 ; ld r28, r28 }
  //   { shl16insli r29, zero, index ; jr r28 }
  static const uint8_t shortEntry[] = {
      0x00, 0x30, 0x48, 0x51, 0x0d, 0xf0, 0x6a, 0x28,
      0x9c, 0x06, 0x00, 0x90, 0xed, 0x07, 0x00, 0x00,
      0xdd, 0x0f, 0x00, 0x70, 0x8e, 0xeb, 0x6a, 0x28,
      0x9b, 0xb6, 0x0d, 0x50, 0x80, 0x73, 0x6a, 0x28,
      0x00, 0x30, 0x48, 0xd1, 0xff, 0x57, 0x18, 0x18,
  };
  static const uint8_t longEntry[] = {
      0xdc, 0x0f, 0x00, 0x10, 0x0d, 0xf0, 0x6a, 0x28,
      0xdb, 0x0f, 0x00, 0x10, 0x8e, 0x03, 0x00, 0x38,
      0x9c, 0xc6, 0x0d, 0xd0, 0x6d, 0x03, 0x00, 0x38,
      0x9b, 0xb6, 0xc5, 0xad, 0xff, 0x57, 0xe0, 0x8e,
      0xdd, 0x0f, 0x00, 0x70, 0x80, 0x73, 0x6a, 0x28,
  };
  uint64_t lnk = pltEntryAddr + 8;
  int64_t gotEntry = sym.getGotPltVA(ctx) - lnk;
  int64_t got0 = ctx.in.gotPlt->getVA() - lnk;
  uint64_t index = sym.getPltIdx(ctx);

  if (isInt<16>(gotEntry) && isInt<16>(got0)) {
    memcpy(buf, shortEntry, sizeof(shortEntry));
    insert(buf + 8, imm16X0, gotEntry);
    insert(buf + 8, imm16X1, got0);
    insert(buf + 16, imm16X0, index);
    return;
  }

  memcpy(buf, longEntry, sizeof(longEntry));
  insert(buf, imm16X0, gotEntry >> 16);
  insert(buf + 8, imm16X0, got0 >> 16);
  insert(buf + 8, imm16X1, gotEntry);
  insert(buf + 16, imm16X1, got0);
  insert(buf + 32, imm16X0, index);
}

template <class ELFT, class RelTy>
void Tile::scanSectionImpl(InputSectionBase &sec, Relocs<RelTy> rels,
                           unsigned shard) {
  RelocScan rs(ctx, &sec, shard);
  sec.relocations.reserve(rels.size());
  for (auto it = rels.begin(); it != rels.end(); ++it) {
    const RelTy &rel = *it;
    uint32_t symIdx = rel.getSymbol(false);
    Symbol &sym = sec.getFile<ELFT>()->getSymbol(symIdx);
    uint64_t offset = rel.r_offset;
    RelType type = rel.getType(false);
    if (sym.isUndefined() && symIdx != 0 &&
        rs.maybeReportUndefined(cast<Undefined>(sym), offset))
      continue;
    int64_t addend = rs.getAddend<ELFT>(rel, type);
    bool toLe = !ctx.arg.shared && !sym.isPreemptible;
    RelExpr expr;
    switch (type) {
    case R_TILEGX_NONE:
    case R_TILEGX_GNU_VTINHERIT:
    case R_TILEGX_GNU_VTENTRY:
      continue;

    case R_TILEGX_64:
    case R_TILEGX_32:
    case R_TILEGX_16:
    case R_TILEGX_8:
    case R_TILEGX_HW0:
    case R_TILEGX_HW1:
    case R_TILEGX_HW2:
    case R_TILEGX_HW3:
    case R_TILEGX_HW0_LAST:
    case R_TILEGX_HW1_LAST:
    case R_TILEGX_HW2_LAST:
    case R_TILEGX_IMM8_X0:
    case R_TILEGX_IMM8_Y0:
    case R_TILEGX_IMM8_X1:
    case R_TILEGX_IMM8_Y1:
    case R_TILEGX_DEST_IMM8_X1:
    case R_TILEGX_MT_IMM14_X1:
    case R_TILEGX_MF_IMM14_X1:
    case R_TILEGX_MMSTART_X0:
    case R_TILEGX_MMEND_X0:
    case R_TILEGX_SHAMT_X0:
    case R_TILEGX_SHAMT_X1:
    case R_TILEGX_SHAMT_Y0:
    case R_TILEGX_SHAMT_Y1:
    case R_TILEGX_IMM16_X0_HW0:
    case R_TILEGX_IMM16_X1_HW0:
    case R_TILEGX_IMM16_X0_HW1:
    case R_TILEGX_IMM16_X1_HW1:
    case R_TILEGX_IMM16_X0_HW2:
    case R_TILEGX_IMM16_X1_HW2:
    case R_TILEGX_IMM16_X0_HW3:
    case R_TILEGX_IMM16_X1_HW3:
    case R_TILEGX_IMM16_X0_HW0_LAST:
    case R_TILEGX_IMM16_X1_HW0_LAST:
    case R_TILEGX_IMM16_X0_HW1_LAST:
    case R_TILEGX_IMM16_X1_HW1_LAST:
    case R_TILEGX_IMM16_X0_HW2_LAST:
    case R_TILEGX_IMM16_X1_HW2_LAST:
      expr = R_ABS;
      break;

    case R_TILEGX_64_PCREL:
    case R_TILEGX_32_PCREL:
    case R_TILEGX_16_PCREL:
    case R_TILEGX_8_PCREL:
    case R_TILEGX_BROFF_X1:
    case R_TILEGX_IMM16_X0_HW0_PCREL:
    case R_TILEGX_IMM16_X1_HW0_PCREL:
    case R_TILEGX_IMM16_X0_HW1_PCREL:
    case R_TILEGX_IMM16_X1_HW1_PCREL:
    case R_TILEGX_IMM16_X0_HW2_PCREL:
    case R_TILEGX_IMM16_X1_HW2_PCREL:
    case R_TILEGX_IMM16_X0_HW3_PCREL:
    case R_TILEGX_IMM16_X1_HW3_PCREL:
    case R_TILEGX_IMM16_X0_HW0_LAST_PCREL:
    case R_TILEGX_IMM16_X1_HW0_LAST_PCREL:
    case R_TILEGX_IMM16_X0_HW1_LAST_PCREL:
    case R_TILEGX_IMM16_X1_HW1_LAST_PCREL:
    case R_TILEGX_IMM16_X0_HW2_LAST_PCREL:
    case R_TILEGX_IMM16_X1_HW2_LAST_PCREL:
      rs.processR_PC(type, offset, addend, sym);
      continue;

    case R_TILEGX_JUMPOFF_X1:
    case R_TILEGX_JUMPOFF_X1_PLT:
    case R_TILEGX_IMM16_X0_HW0_PLT_PCREL:
    case R_TILEGX_IMM16_X1_HW0_PLT_PCREL:
    case R_TILEGX_IMM16_X0_HW1_PLT_PCREL:
    case R_TILEGX_IMM16_X1_HW1_PLT_PCREL:
    case R_TILEGX_IMM16_X0_HW2_PLT_PCREL:
    case R_TILEGX_IMM16_X1_HW2_PLT_PCREL:
    case R_TILEGX_IMM16_X0_HW3_PLT_PCREL:
    case R_TILEGX_IMM16_X1_HW3_PLT_PCREL:
    case R_TILEGX_IMM16_X0_HW0_LAST_PLT_PCREL:
    case R_TILEGX_IMM16_X1_HW0_LAST_PLT_PCREL:
    case R_TILEGX_IMM16_X0_HW1_LAST_PLT_PCREL:
    case R_TILEGX_IMM16_X1_HW1_LAST_PLT_PCREL:
    case R_TILEGX_IMM16_X0_HW2_LAST_PLT_PCREL:
    case R_TILEGX_IMM16_X1_HW2_LAST_PLT_PCREL:
      rs.processR_PLT_PC(type, offset, addend, sym);
      continue;

    // GOT entries are addressed from _GLOBAL_OFFSET_TABLE_, the start of .got.
    case R_TILEGX_IMM16_X0_HW0_GOT:
    case R_TILEGX_IMM16_X1_HW0_GOT:
    case R_TILEGX_IMM16_X0_HW0_LAST_GOT:
    case R_TILEGX_IMM16_X1_HW0_LAST_GOT:
    case R_TILEGX_IMM16_X0_HW1_LAST_GOT:
    case R_TILEGX_IMM16_X1_HW1_LAST_GOT:
      expr = R_GOT_OFF;
      break;

    case R_TILEGX_IMM16_X0_HW0_TLS_LE:
    case R_TILEGX_IMM16_X1_HW0_TLS_LE:
    case R_TILEGX_IMM16_X0_HW0_LAST_TLS_LE:
    case R_TILEGX_IMM16_X1_HW0_LAST_TLS_LE:
    case R_TILEGX_IMM16_X0_HW1_LAST_TLS_LE:
    case R_TILEGX_IMM16_X1_HW1_LAST_TLS_LE:
      if (rs.checkTlsLe(offset, sym, type))
        continue;
      expr = R_TPREL;
      break;
    case R_TILEGX_IMM16_X0_HW0_TLS_IE:
    case R_TILEGX_IMM16_X1_HW0_TLS_IE:
    case R_TILEGX_IMM16_X0_HW0_LAST_TLS_IE:
    case R_TILEGX_IMM16_X1_HW0_LAST_TLS_IE:
    case R_TILEGX_IMM16_X0_HW1_LAST_TLS_IE:
    case R_TILEGX_IMM16_X1_HW1_LAST_TLS_IE:
      rs.handleTlsIe(R_GOT_OFF, type, offset, addend, sym);
      continue;
    case R_TILEGX_IMM16_X0_HW0_TLS_GD:
    case R_TILEGX_IMM16_X1_HW0_TLS_GD:
    case R_TILEGX_IMM16_X0_HW0_LAST_TLS_GD:
    case R_TILEGX_IMM16_X1_HW0_LAST_TLS_GD:
    case R_TILEGX_IMM16_X0_HW1_LAST_TLS_GD:
    case R_TILEGX_IMM16_X1_HW1_LAST_TLS_GD:
      rs.handleTlsGd(R_TLSGD_GOT, R_RELAX_TLS_GD_TO_IE_GOT_OFF, R_TPREL, type,
                     offset, addend, sym);
      continue;
    case R_TILEGX_TLS_DTPOFF64:
    case R_TILEGX_TLS_DTPOFF32:
      sec.addReloc(
          {ctx.arg.shared ? R_DTPREL : R_TPREL, type, offset, addend, &sym});
      continue;

    // Sequence markers. Their value is unused, and the expression only records
    // the access model the halfword relocations chose, matching handleTlsGd
    // and handleTlsIe: R_TPREL for local exec, R_GOT_OFF for initial exec, and
    // R_PLT_PC for the general dynamic call, which finalizeRelocScan rebinds
    // to __tls_get_addr.
    case R_TILEGX_TLS_GD_CALL:
      if (ctx.arg.shared)
        sec.addReloc({R_PLT_PC, type, offset, addend, &sym});
      else
        sec.addReloc({toLe ? R_TPREL : R_GOT_OFF, type, offset, addend, &sym});
      continue;
    case R_TILEGX_IMM8_X0_TLS_GD_ADD:
    case R_TILEGX_IMM8_X1_TLS_GD_ADD:
    case R_TILEGX_IMM8_Y0_TLS_GD_ADD:
    case R_TILEGX_IMM8_Y1_TLS_GD_ADD:
      sec.addReloc({ctx.arg.shared ? R_TLSGD_GOT : R_TPREL, type, offset,
                    addend, &sym});
      continue;
    case R_TILEGX_TLS_IE_LOAD:
    case R_TILEGX_IMM8_X0_TLS_ADD:
    case R_TILEGX_IMM8_X1_TLS_ADD:
    case R_TILEGX_IMM8_Y0_TLS_ADD:
    case R_TILEGX_IMM8_Y1_TLS_ADD:
      sec.addReloc({toLe ? R_TPREL : R_GOT_OFF, type, offset, addend, &sym});
      continue;

    default:
      Err(ctx) << getErrorLoc(ctx, sec.content().data() + offset)
               << "unknown relocation (" << type.v << ") against symbol "
               << &sym;
      continue;
    }
    rs.process(expr, type, offset, sym, addend);
  }
}

void Tile::finalizeRelocScan() {
  Symbol *tga = nullptr;
  for (ELFFileBase *f : ctx.objectFiles) {
    for (InputSectionBase *s : f->getSections()) {
      auto *isec = dyn_cast_or_null<InputSection>(s);
      if (!isec || !isec->isLive())
        continue;
      for (Relocation &rel : isec->relocs()) {
        if (rel.type != R_TILEGX_TLS_GD_CALL || rel.expr != R_PLT_PC)
          continue;
        if (!tga) {
          tga = ctx.symtab->addSymbol(Undefined{ctx.internalFile,
                                                "__tls_get_addr", STB_GLOBAL,
                                                STV_DEFAULT, STT_FUNC});
          tga->isUsedInRegularObj = true;
          tga->isPreemptible = true;
          tga->setFlags(NEEDS_PLT | USED);
        }
        rel.sym = tga;
      }
    }
  }
}

static void relocateTlsAdd(uint8_t *loc, const Relocation &rel) {
  // `addi dst, got, tls_add(x)` becomes `add dst, got, dst` while the sequence
  // still reads the GOT, and `move dst, dst` once it is local exec.
  bool pipe0 = rel.type == R_TILEGX_IMM8_X0_TLS_ADD ||
               rel.type == R_TILEGX_IMM8_Y0_TLS_ADD;
  bool xMode = rel.type == R_TILEGX_IMM8_X0_TLS_ADD ||
               rel.type == R_TILEGX_IMM8_X1_TLS_ADD;
  unsigned dest = pipe0 ? 0 : 31;
  if (rel.expr == R_TPREL) {
    copyBits(loc, dest, pipe0 ? 6 : 37, 6);
    uint64_t mask = pipe0 ? (xMode ? maskX0NoDestNoSrcA : maskY0NoDestNoSrcA)
                          : (xMode ? maskX1NoDestNoSrcA : maskY1NoDestNoSrcA);
    replaceInsn(loc, mask, xMode ? moveR0R0X0X1 : moveR0R0Y0Y1);
    return;
  }

  copyBits(loc, dest, pipe0 ? 12 : 43, 6);
  uint64_t mask = pipe0 ? (xMode ? maskX0NoOperand : maskY0NoOperand)
                        : (xMode ? maskX1NoOperand : maskY1NoOperand);
  replaceInsn(loc, mask, xMode ? addR0R0TpX0X1 : addR0R0TpY0Y1);
}

void Tile::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {
  switch (rel.type) {
  case R_TILEGX_64:
  case R_TILEGX_64_PCREL:
  case R_TILEGX_TLS_DTPOFF64:
    write64le(loc, val);
    return;
  case R_TILEGX_32:
  case R_TILEGX_TLS_DTPOFF32:
    checkIntUInt(ctx, loc, val, 32, rel);
    write32le(loc, val);
    return;
  case R_TILEGX_32_PCREL:
    checkInt(ctx, loc, val, 32, rel);
    write32le(loc, val);
    return;
  case R_TILEGX_16:
    checkIntUInt(ctx, loc, val, 16, rel);
    write16le(loc, val);
    return;
  case R_TILEGX_16_PCREL:
    checkInt(ctx, loc, val, 16, rel);
    write16le(loc, val);
    return;
  case R_TILEGX_8:
    checkUInt(ctx, loc, val, 8, rel);
    *loc = val;
    return;
  case R_TILEGX_8_PCREL:
    checkInt(ctx, loc, val, 8, rel);
    *loc = val;
    return;

  case R_TILEGX_HW0:
    write16le(loc, val);
    return;
  case R_TILEGX_HW1:
    write16le(loc, val >> 16);
    return;
  case R_TILEGX_HW2:
    write16le(loc, val >> 32);
    return;
  case R_TILEGX_HW3:
    write16le(loc, val >> 48);
    return;
  case R_TILEGX_HW0_LAST:
    checkInt(ctx, loc, val, 16, rel);
    write16le(loc, val);
    return;
  case R_TILEGX_HW1_LAST:
    checkInt(ctx, loc, val, 32, rel);
    write16le(loc, val >> 16);
    return;
  case R_TILEGX_HW2_LAST:
    checkInt(ctx, loc, val, 48, rel);
    write16le(loc, val >> 32);
    return;

  case R_TILEGX_BROFF_X1:
    checkInt(ctx, loc, val, 20, rel);
    checkAlignment(ctx, loc, val, 8, rel);
    insert(loc, brOffX1, val >> 3);
    return;
  case R_TILEGX_JUMPOFF_X1:
  case R_TILEGX_JUMPOFF_X1_PLT:
    checkInt(ctx, loc, val, 30, rel);
    checkAlignment(ctx, loc, val, 8, rel);
    insert(loc, jumpOffX1, val >> 3);
    return;

  case R_TILEGX_IMM8_X0:
  case R_TILEGX_IMM8_Y0:
    checkInt(ctx, loc, val, 8, rel);
    insert(loc, imm8X0, val);
    return;
  case R_TILEGX_IMM8_X1:
  case R_TILEGX_IMM8_Y1:
    checkInt(ctx, loc, val, 8, rel);
    insert(loc, imm8X1, val);
    return;
  case R_TILEGX_DEST_IMM8_X1:
    checkInt(ctx, loc, val, 8, rel);
    insert(loc, destImm8X1, val);
    return;
  case R_TILEGX_MT_IMM14_X1:
    checkUInt(ctx, loc, val, 14, rel);
    insert(loc, mtImm14X1, val);
    return;
  case R_TILEGX_MF_IMM14_X1:
    checkUInt(ctx, loc, val, 14, rel);
    insert(loc, mfImm14X1, val);
    return;
  case R_TILEGX_MMSTART_X0:
    checkUInt(ctx, loc, val, 6, rel);
    insert(loc, bfStartX0, val);
    return;
  case R_TILEGX_MMEND_X0:
    checkUInt(ctx, loc, val, 6, rel);
    insert(loc, bfEndX0, val);
    return;
  case R_TILEGX_SHAMT_X0:
  case R_TILEGX_SHAMT_Y0:
    checkUInt(ctx, loc, val, 6, rel);
    insert(loc, shamtX0, val);
    return;
  case R_TILEGX_SHAMT_X1:
  case R_TILEGX_SHAMT_Y1:
    checkUInt(ctx, loc, val, 6, rel);
    insert(loc, shamtX1, val);
    return;

  case R_TILEGX_TLS_GD_CALL:
    if (rel.expr == R_TPREL) {
      replaceInsn(loc, maskX1, moveR0R0X0X1);
    } else if (rel.expr == R_GOT_OFF) {
      replaceInsn(loc, maskX1, ldR0R0X1);
    } else {
      checkInt(ctx, loc, val, 30, rel);
      insert(loc, jumpOffX1, val >> 3);
    }
    return;
  case R_TILEGX_IMM8_X0_TLS_GD_ADD:
    replaceInsn(loc, maskX0NoDestNoSrcA,
                rel.expr == R_TLSGD_GOT ? moveR0R0X0X1 : addR0R0TpX0X1);
    return;
  case R_TILEGX_IMM8_X1_TLS_GD_ADD:
    replaceInsn(loc, maskX1NoDestNoSrcA,
                rel.expr == R_TLSGD_GOT ? moveR0R0X0X1 : addR0R0TpX0X1);
    return;
  case R_TILEGX_IMM8_Y0_TLS_GD_ADD:
    replaceInsn(loc, maskY0NoDestNoSrcA,
                rel.expr == R_TLSGD_GOT ? moveR0R0Y0Y1 : addR0R0TpY0Y1);
    return;
  case R_TILEGX_IMM8_Y1_TLS_GD_ADD:
    replaceInsn(loc, maskY1NoDestNoSrcA,
                rel.expr == R_TLSGD_GOT ? moveR0R0Y0Y1 : addR0R0TpY0Y1);
    return;
  case R_TILEGX_TLS_IE_LOAD:
    replaceInsn(loc, maskX1NoDestNoSrcA,
                rel.expr == R_TPREL ? moveR0R0X0X1 : ldR0R0X1);
    return;
  case R_TILEGX_IMM8_X0_TLS_ADD:
  case R_TILEGX_IMM8_X1_TLS_ADD:
  case R_TILEGX_IMM8_Y0_TLS_ADD:
  case R_TILEGX_IMM8_Y1_TLS_ADD:
    relocateTlsAdd(loc, rel);
    return;
  default:
    break;
  }

  unsigned shift;
  bool last;
  switch (rel.type) {
  case R_TILEGX_IMM16_X0_HW0:
  case R_TILEGX_IMM16_X1_HW0:
  case R_TILEGX_IMM16_X0_HW0_PCREL:
  case R_TILEGX_IMM16_X1_HW0_PCREL:
  case R_TILEGX_IMM16_X0_HW0_PLT_PCREL:
  case R_TILEGX_IMM16_X1_HW0_PLT_PCREL:
  case R_TILEGX_IMM16_X0_HW0_GOT:
  case R_TILEGX_IMM16_X1_HW0_GOT:
  case R_TILEGX_IMM16_X0_HW0_TLS_GD:
  case R_TILEGX_IMM16_X1_HW0_TLS_GD:
  case R_TILEGX_IMM16_X0_HW0_TLS_IE:
  case R_TILEGX_IMM16_X1_HW0_TLS_IE:
  case R_TILEGX_IMM16_X0_HW0_TLS_LE:
  case R_TILEGX_IMM16_X1_HW0_TLS_LE:
    shift = 0;
    last = false;
    break;
  case R_TILEGX_IMM16_X0_HW1:
  case R_TILEGX_IMM16_X1_HW1:
  case R_TILEGX_IMM16_X0_HW1_PCREL:
  case R_TILEGX_IMM16_X1_HW1_PCREL:
  case R_TILEGX_IMM16_X0_HW1_PLT_PCREL:
  case R_TILEGX_IMM16_X1_HW1_PLT_PCREL:
    shift = 16;
    last = false;
    break;
  case R_TILEGX_IMM16_X0_HW2:
  case R_TILEGX_IMM16_X1_HW2:
  case R_TILEGX_IMM16_X0_HW2_PCREL:
  case R_TILEGX_IMM16_X1_HW2_PCREL:
  case R_TILEGX_IMM16_X0_HW2_PLT_PCREL:
  case R_TILEGX_IMM16_X1_HW2_PLT_PCREL:
    shift = 32;
    last = false;
    break;
  case R_TILEGX_IMM16_X0_HW3:
  case R_TILEGX_IMM16_X1_HW3:
  case R_TILEGX_IMM16_X0_HW3_PCREL:
  case R_TILEGX_IMM16_X1_HW3_PCREL:
  case R_TILEGX_IMM16_X0_HW3_PLT_PCREL:
  case R_TILEGX_IMM16_X1_HW3_PLT_PCREL:
    shift = 48;
    last = false;
    break;
  case R_TILEGX_IMM16_X0_HW0_LAST:
  case R_TILEGX_IMM16_X1_HW0_LAST:
  case R_TILEGX_IMM16_X0_HW0_LAST_PCREL:
  case R_TILEGX_IMM16_X1_HW0_LAST_PCREL:
  case R_TILEGX_IMM16_X0_HW0_LAST_PLT_PCREL:
  case R_TILEGX_IMM16_X1_HW0_LAST_PLT_PCREL:
  case R_TILEGX_IMM16_X0_HW0_LAST_GOT:
  case R_TILEGX_IMM16_X1_HW0_LAST_GOT:
  case R_TILEGX_IMM16_X0_HW0_LAST_TLS_GD:
  case R_TILEGX_IMM16_X1_HW0_LAST_TLS_GD:
  case R_TILEGX_IMM16_X0_HW0_LAST_TLS_IE:
  case R_TILEGX_IMM16_X1_HW0_LAST_TLS_IE:
  case R_TILEGX_IMM16_X0_HW0_LAST_TLS_LE:
  case R_TILEGX_IMM16_X1_HW0_LAST_TLS_LE:
    shift = 0;
    last = true;
    break;
  case R_TILEGX_IMM16_X0_HW1_LAST:
  case R_TILEGX_IMM16_X1_HW1_LAST:
  case R_TILEGX_IMM16_X0_HW1_LAST_PCREL:
  case R_TILEGX_IMM16_X1_HW1_LAST_PCREL:
  case R_TILEGX_IMM16_X0_HW1_LAST_PLT_PCREL:
  case R_TILEGX_IMM16_X1_HW1_LAST_PLT_PCREL:
  case R_TILEGX_IMM16_X0_HW1_LAST_GOT:
  case R_TILEGX_IMM16_X1_HW1_LAST_GOT:
  case R_TILEGX_IMM16_X0_HW1_LAST_TLS_GD:
  case R_TILEGX_IMM16_X1_HW1_LAST_TLS_GD:
  case R_TILEGX_IMM16_X0_HW1_LAST_TLS_IE:
  case R_TILEGX_IMM16_X1_HW1_LAST_TLS_IE:
  case R_TILEGX_IMM16_X0_HW1_LAST_TLS_LE:
  case R_TILEGX_IMM16_X1_HW1_LAST_TLS_LE:
    shift = 16;
    last = true;
    break;
  case R_TILEGX_IMM16_X0_HW2_LAST:
  case R_TILEGX_IMM16_X1_HW2_LAST:
  case R_TILEGX_IMM16_X0_HW2_LAST_PCREL:
  case R_TILEGX_IMM16_X1_HW2_LAST_PCREL:
  case R_TILEGX_IMM16_X0_HW2_LAST_PLT_PCREL:
  case R_TILEGX_IMM16_X1_HW2_LAST_PLT_PCREL:
    shift = 32;
    last = true;
    break;
  default:
    llvm_unreachable("unknown relocation");
  }

  if (last)
    checkInt(ctx, loc, val, shift + 16, rel);
  // Every IMM16 family numbers its X0 form even and its X1 form odd.
  bool x1 = (rel.type - R_TILEGX_IMM16_X0_HW0) % 2 == 1;
  insert(loc, x1 ? imm16X1 : imm16X0, val >> shift);
}

void elf::setTileTargetInfo(Ctx &ctx) { ctx.target.reset(new Tile(ctx)); }
