//===-- TileAsmBackend.cpp - Tile Assembler Backend ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the TileAsmBackend class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TileFixupKinds.h"
#include "MCTargetDesc/TileMCTargetDesc.h"
#include "MCTargetDesc/TileOpcodeTable.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include <cstdint>

using namespace llvm;

// GOT, PLT and TLS relocations name a linker-built slot or sequence, so they
// stay relocations even when the symbol is local and defined here.
static bool forcesRelocation(unsigned Type) {
  using namespace ELF;
  switch (Type) {
  case R_TILEGX_JUMPOFF_X1_PLT:
  case R_TILEGX_IMM16_X0_HW0_GOT:
  case R_TILEGX_IMM16_X1_HW0_GOT:
  case R_TILEGX_IMM16_X0_HW0_LAST_GOT:
  case R_TILEGX_IMM16_X1_HW0_LAST_GOT:
  case R_TILEGX_IMM16_X0_HW1_LAST_GOT:
  case R_TILEGX_IMM16_X1_HW1_LAST_GOT:
    return true;
  default:
    break;
  }
  if (Type >= R_TILEGX_IMM16_X0_HW0_PLT_PCREL &&
      Type <= R_TILEGX_IMM16_X1_HW2_PLT_PCREL)
    return true;
  if (Type >= R_TILEGX_IMM16_X0_HW3_PLT_PCREL &&
      Type <= R_TILEGX_IMM16_X1_HW1_LAST_TLS_IE)
    return true;
  return Type >= R_TILEGX_TLS_DTPMOD64 && Type <= R_TILEGX_IMM8_Y1_TLS_ADD;
}

namespace {
class TileAsmBackend : public MCAsmBackend {
  uint8_t OSABI;

public:
  TileAsmBackend(uint8_t OSABI)
      : MCAsmBackend(llvm::endianness::little), OSABI(OSABI) {}
  ~TileAsmBackend() override = default;

  void applyFixup(const MCFragment &F, const MCFixup &Fixup,
                  const MCValue &Target, uint8_t *Data, uint64_t Value,
                  bool IsResolved) override {
    MCFixupKind Kind = Fixup.getKind();
    bool IsTarget = Kind >= FirstTargetFixupKind;
    if (IsTarget && forcesRelocation(Tile::getFixupReloc(Kind)))
      IsResolved = false;
    maybeAddReloc(F, Fixup, Target, Value, IsResolved);
    if (!IsResolved)
      return;

    if (!IsTarget) {
      unsigned NumBytes = getFixupKindInfo(Kind).TargetSize / 8;
      for (unsigned I = 0; I != NumBytes; ++I)
        Data[I] |= uint8_t(Value >> (I * 8));
      return;
    }

    std::optional<TileISA::RelocField> Where =
        TileISA::relocField(Tile::getFixupReloc(Kind));
    if (!Where)
      return;
    const TileISA::Field &Field = TileISA::field(Where->FieldId);
    if (Where->ValueShift == 3 &&
        !isIntN(Field.NumChunks == 1 ? 27 : 17, int64_t(Value) >> 3)) {
      getContext().reportError(Fixup.getLoc(), "branch target out of range");
      return;
    }
    uint64_t Bits = TileISA::insertField(Field, Value >> Where->ValueShift);
    for (unsigned I = 0; I != 8; ++I)
      Data[I] |= uint8_t(Bits >> (I * 8));
  }

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    return createTileELFObjectWriter(OSABI);
  }

  // Fold an unsigned LEB128 whose value is a label difference spanning
  // fragments (as the __gcc_except_table call-site table emits) into an
  // absolute constant. TILE-Gx has no LEB128 linker relocation, so an
  // unresolved difference has nowhere to go and must be evaluated here.
  std::pair<bool, bool> relaxLEB128(MCFragment &F,
                                    int64_t &Value) const override {
    if (F.isLEBSigned())
      return std::make_pair(false, false);
    return std::make_pair(F.getLEBValue().evaluateKnownAbsolute(Value, *Asm),
                          false);
  }

  MCFixupKindInfo getFixupKindInfo(MCFixupKind Kind) const override {
    if (Kind < FirstTargetFixupKind)
      return MCAsmBackend::getFixupKindInfo(Kind);
    return {"fixup_tile", 0, 64, 0};
  }

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override {
    if ((Count % 8) != 0)
      return false;
    // A bundle of two FNOPs: FNOP in X0 (bits 30-0) and X1 (bits 61-31).
    const uint64_t Nop = 0x51483000ULL | (0x50d46000ULL << 31);
    for (uint64_t I = 0; I < Count; I += 8)
      support::endian::write<uint64_t>(OS, Nop, llvm::endianness::little);
    return true;
  }
};
} // namespace

MCAsmBackend *llvm::createTileAsmBackend(const Target &T,
                                         const MCSubtargetInfo &STI,
                                         const MCRegisterInfo &MRI,
                                         const MCTargetOptions &Options) {
  uint8_t OSABI = MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  return new TileAsmBackend(OSABI);
}
