//===-- TileMCCodeEmitter.cpp - Convert Tile Code to Machine Code ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the TileMCCodeEmitter class.
//
// First-light scope: the packetizer is disabled, so each instruction is its own
// 64-bit VLIW bundle. The TileGen format classes already fill the unused issue
// slot with FNOP (see TileInstrFormats.td), so a single getBinaryCodeForInstr
// call yields the complete bundle. Symbol-relative operands (hw0/hw1/got/tls)
// are rejected here and wired up with the full relocation set in a later phase.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TileBaseInfo.h"
#include "MCTargetDesc/TileFixupKinds.h"
#include "MCTargetDesc/TileMCTargetDesc.h"
#include "MCTargetDesc/TileOpcodeTable.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

namespace {
// A tilegx X-format bundle is two issue slots packed into one 64-bit word:
// the X1 slot occupies bits 63-31, the X0 slot bits 30-0. Every single
// instruction is encoded with its real bits in its own slot and FNOP in the
// other (see TileInstrFormats.td), so a packed bundle is just the X1-slot
// half of the X1-encoded instruction merged with the X0-slot half of the
// X0-encoded instruction.
static constexpr uint64_t X1SlotMask = 0xFFFFFFFF80000000ULL; // bits 63-31
static constexpr uint64_t X0SlotMask = 0x000000007FFFFFFFULL; // bits 30-0

class TileMCCodeEmitter : public MCCodeEmitter {
  MCContext &Ctx;
  const MCInstrInfo &MCII;

public:
  TileMCCodeEmitter(const MCInstrInfo &mcii, MCContext &ctx)
      : Ctx(ctx), MCII(mcii) {}
  TileMCCodeEmitter(const TileMCCodeEmitter &) = delete;
  void operator=(const TileMCCodeEmitter &) = delete;
  ~TileMCCodeEmitter() override = default;

  // TableGen'erated function for getting the binary encoding of an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  // Return the binary encoding of an operand. Symbol references record a fixup
  // and contribute zero bits.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  unsigned getJumpTargetOpValue(const MCInst &MI, unsigned OpNo,
                                SmallVectorImpl<MCFixup> &Fixups,
                                const MCSubtargetInfo &STI) const;

  unsigned getBranchTargetOpValue(const MCInst &MI, unsigned OpNo,
                                  SmallVectorImpl<MCFixup> &Fixups,
                                  const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  uint64_t encodeTableBundle(const MCInst &MI,
                             SmallVectorImpl<MCFixup> &Fixups) const;
};
} // namespace

MCCodeEmitter *llvm::createTileMCCodeEmitter(const MCInstrInfo &MCII,
                                             MCContext &Ctx) {
  return new TileMCCodeEmitter(MCII, Ctx);
}

void TileMCCodeEmitter::encodeInstruction(const MCInst &MI,
                                          SmallVectorImpl<char> &CB,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  // A packed VLIW bundle carries its issue-slot instructions as sub-MCInsts.
  // Encode each in isolation (each already fills the complementary slot with
  // FNOP) and overwrite the slot it really occupies in the accumulated word.
  // Relocations come only from X1-pipe operands and ride at bundle offset 0, so
  // the recorded fixups stay correct for the merged word.
  if (MI.getOpcode() == Tile::BUNDLE && MI.getNumOperands() &&
      MI.getOperand(0).getInst()->getOpcode() == Tile::TABLE_INST) {
    support::endian::write<uint64_t>(CB, encodeTableBundle(MI, Fixups),
                                     llvm::endianness::little);
    return;
  }

  if (MI.getOpcode() == Tile::BUNDLE) {
    uint64_t Bundle = 0;
    bool First = true;
    for (const MCOperand &Op : MI) {
      assert(Op.isInst() && "bundle operand is not a sub-instruction");
      const MCInst &Sub = *Op.getInst();
      uint64_t E = getBinaryCodeForInstr(Sub, Fixups, STI);
      uint64_t TSFlags = MCII.get(Sub.getOpcode()).TSFlags;
      bool SubIsX0 = (TSFlags >> TileII::EncPipePos) & TileII::EncPipeMask;
      // The merge assumes the sub fills its complementary slot with FNOP. A
      // non-standard (Y2 load/store) encoding would corrupt the other slot, so
      // refuse rather than emit a wrong bundle. The packetizer and parser keep
      // such instructions out of bundles; this guards the encoder itself.
      uint64_t Comp = SubIsX0 ? (E & X1SlotMask) : (E & X0SlotMask);
      uint64_t ExpectFnop = SubIsX0 ? TileII::FnopX1 : TileII::FnopX0;
      if (!TileII::isBundleMergeable(TSFlags) || Comp != ExpectFnop)
        report_fatal_error("Tile: instruction is not bundle-mergeable");
      if (First) {
        Bundle = E;
        First = false;
      } else if (SubIsX0) {
        Bundle = (Bundle & X1SlotMask) | (E & X0SlotMask);
      } else {
        Bundle = (Bundle & X0SlotMask) | (E & X1SlotMask);
      }
    }
    support::endian::write<uint64_t>(CB, Bundle, llvm::endianness::little);
    return;
  }

  uint64_t Bundle = getBinaryCodeForInstr(MI, Fixups, STI);
  support::endian::write<uint64_t>(CB, Bundle, llvm::endianness::little);
}

// Bundles from the asm parser. The parser already chose a pipe per slot and
// checked operand kinds and ranges against the table.
uint64_t
TileMCCodeEmitter::encodeTableBundle(const MCInst &MI,
                                     SmallVectorImpl<MCFixup> &Fixups) const {
  using namespace TileISA;
  uint64_t Bundle = 0;
  unsigned Used = 0;
  for (const MCOperand &SubOp : MI) {
    const MCInst &Sub = *SubOp.getInst();
    const Opcode &Op = opcode(Sub.getOperand(0).getImm());
    auto P = Pipe(Sub.getOperand(1).getImm());
    Used |= 1u << P;
    Bundle |= Op.FixedBits[P];
    for (unsigned I = 0; I != Op.NumOperands; ++I) {
      const Operand &Info = operand(Op, P, I);
      const MCOperand &MO = Sub.getOperand(I + 2);
      if (MO.isImm()) {
        Bundle |= insertField(field(Info.FieldId), MO.getImm());
        continue;
      }

      const MCExpr *Expr = MO.getExpr();
      std::optional<unsigned> Reloc = Info.DefaultReloc;
      if (const auto *SE = dyn_cast<MCSpecifierExpr>(Expr)) {
        Reloc = applyOperator(SE->getSpecifier(), Info.DefaultReloc);
        Expr = SE->getSubExpr();
        if (operatorNeedsSymbol(SE->getSpecifier()) &&
            !isa<MCSymbolRefExpr>(Expr))
          Ctx.reportError(Sub.getLoc(),
                          "operator may only be applied to a symbol");
      }
      if (!Reloc || *Reloc == ELF::R_TILEGX_NONE) {
        Ctx.reportError(Sub.getLoc(), "invalid operator for operand");
        continue;
      }
      Fixups.push_back(MCFixup::create(0, Expr, Tile::getFixupKind(*Reloc),
                                       Info.PCRel));
    }
  }

  // tile-as fills beside an instruction that may not bundle with a nop, which
  // binary rewriters must keep, and elsewhere with an fnop.
  bool Lone = MI.getNumOperands() == 1 &&
              !opcode(MI.getOperand(0).getInst()->getOperand(0).getImm())
                   .CanBundle;
  bool YMode = Used & ~((1u << PipeX0) | (1u << PipeX1));
  for (Pipe P : YMode ? std::initializer_list<Pipe>{PipeY0, PipeY1, PipeY2}
                      : std::initializer_list<Pipe>{PipeX0, PipeX1}) {
    if (Used & (1u << P))
      continue;
    if (std::optional<uint64_t> Fill = fillerBits(P, Lone))
      Bundle |= *Fill;
    else
      Ctx.reportError(MI.getLoc(), "a Y-mode bundle needs a load or store");
  }
  return Bundle;
}

unsigned
TileMCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  assert(MO.isExpr() && "unexpected operand kind");
  const MCExpr *Expr = MO.getExpr();

  // A halfword-extraction relocation specifier (hw0/hw1/hw2_last) drives the
  // moveli/shl16insli address sequence. moveli and shl16insli are X1-pipe in
  // the unpacketized bundle, so every such operand records an X1 fixup; the
  // value bits stay zero and the linker patches them through the recorded
  // relocation.
  if (const auto *SE = dyn_cast<MCSpecifierExpr>(Expr)) {
    Tile::Fixups Kind;
    switch (SE->getSpecifier()) {
    default:
      report_fatal_error("Tile: unsupported relocation specifier");
    case TileII::MO_HW0:
      Kind = Tile::fixup_Tile_X1_HW0;
      break;
    case TileII::MO_HW1:
      Kind = Tile::fixup_Tile_X1_HW1;
      break;
    case TileII::MO_HW1_LAST:
      Kind = Tile::fixup_Tile_X1_HW1_LAST;
      break;
    case TileII::MO_HW2_LAST:
      Kind = Tile::fixup_Tile_X1_HW2_LAST;
      break;
    // GOT-relative halfwords for PIC global access.
    case TileII::MO_HW0_GOT:
      Kind = Tile::fixup_Tile_X1_HW0_GOT;
      break;
    case TileII::MO_HW1_LAST_GOT:
      Kind = Tile::fixup_Tile_X1_HW1_LAST_GOT;
      break;
    // PC-relative halfwords for the GOT base self-computation. The textual
    // operator is the plain hw0/hw1_last on a sym - label difference; the
    // PCREL relocation is selected here and matches GNU as. The fixup itself is
    // not PC-relative, since ELFObjectWriter turns the same-section difference
    // into a PC-relative relocation and asserts the fixup was not already one.
    case TileII::MO_HW0_PIC:
      Kind = Tile::fixup_Tile_X1_HW0_PCREL;
      break;
    case TileII::MO_HW1_LAST_PIC:
      Kind = Tile::fixup_Tile_X1_HW1_LAST_PCREL;
      break;
    // Thread-local storage halfwords and relaxation hints.
    case TileII::MO_HW0_TLS_GD:
      Kind = Tile::fixup_Tile_X1_HW0_TLS_GD;
      break;
    case TileII::MO_HW1_LAST_TLS_GD:
      Kind = Tile::fixup_Tile_X1_HW1_LAST_TLS_GD;
      break;
    case TileII::MO_HW0_TLS_IE:
      Kind = Tile::fixup_Tile_X1_HW0_TLS_IE;
      break;
    case TileII::MO_HW1_LAST_TLS_IE:
      Kind = Tile::fixup_Tile_X1_HW1_LAST_TLS_IE;
      break;
    case TileII::MO_HW0_TLS_LE:
      Kind = Tile::fixup_Tile_X1_HW0_TLS_LE;
      break;
    case TileII::MO_HW1_LAST_TLS_LE:
      Kind = Tile::fixup_Tile_X1_HW1_LAST_TLS_LE;
      break;
    case TileII::MO_TLS_ADD:
      Kind = Tile::fixup_Tile_X1_TLS_ADD;
      break;
    case TileII::MO_TLS_GD_ADD:
      Kind = Tile::fixup_Tile_X1_TLS_GD_ADD;
      break;
    case TileII::MO_TLS_IE_LOAD:
      Kind = Tile::fixup_Tile_X1_TLS_IE_LOAD;
      break;
    }
    Fixups.push_back(
        MCFixup::create(0, Expr, MCFixupKind(Kind), /*PCRel=*/false));
    return 0;
  }

  report_fatal_error("Tile: unsupported symbol-relative operand");
}

unsigned
TileMCCodeEmitter::getJumpTargetOpValue(const MCInst &MI, unsigned OpNo,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  assert(MO.isExpr() && "getJumpTargetOpValue expects an expression");
  const MCExpr *Expr = MO.getExpr();

  // A PIC call goes through the PLT and a general-dynamic TLS access calls
  // __tls_get_addr; both ride on the call operand's relocation specifier.
  Tile::Fixups Kind = Tile::fixup_Tile_X1_JUMPOFF;
  bool PCRel = true;
  if (const auto *SE = dyn_cast<MCSpecifierExpr>(Expr)) {
    switch (SE->getSpecifier()) {
    default:
      report_fatal_error("Tile: unsupported call relocation specifier");
    case TileII::MO_PLT_CALL:
      Kind = Tile::fixup_Tile_X1_JUMPOFF_PLT;
      break;
    case TileII::MO_TLS_GD_CALL:
      Kind = Tile::fixup_Tile_X1_TLS_GD_CALL;
      PCRel = false;
      break;
    }
  }
  Fixups.push_back(MCFixup::create(0, Expr, MCFixupKind(Kind), PCRel));
  return 0;
}

unsigned
TileMCCodeEmitter::getBranchTargetOpValue(const MCInst &MI, unsigned OpNo,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  assert(MO.isExpr() && "getBranchTargetOpValue expects an expression");
  Fixups.push_back(MCFixup::create(0, MO.getExpr(),
                                   MCFixupKind(Tile::fixup_Tile_X1_BROFF),
                                   /*PCRel=*/true));
  return 0;
}

#include "TileGenMCCodeEmitter.inc"
