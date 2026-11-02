//===-- TileELFObjectWriter.cpp - Tile ELF Writer ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TileFixupKinds.h"
#include "MCTargetDesc/TileMCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstdint>

using namespace llvm;

namespace {
class TileELFObjectWriter : public MCELFObjectTargetWriter {
public:
  TileELFObjectWriter(uint8_t OSABI);
  ~TileELFObjectWriter() override = default;

  bool needsRelocateWithSymbol(const MCValue &, unsigned Type) const override;

protected:
  unsigned getRelocType(const MCFixup &Fixup, const MCValue &Target,
                        bool IsPCRel) const override;
};
} // namespace

// TILE-Gx is 64-bit and uses RELA relocations.
TileELFObjectWriter::TileELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit=*/true, OSABI, ELF::EM_TILEGX,
                              /*HasRelocationAddend=*/true) {}

static bool isTlsReloc(unsigned Type) {
  using namespace ELF;
  switch (Type) {
  case R_TILEGX_IMM16_X0_HW0_TLS_GD:
  case R_TILEGX_IMM16_X1_HW0_TLS_GD:
  case R_TILEGX_IMM16_X0_HW0_TLS_LE:
  case R_TILEGX_IMM16_X1_HW0_TLS_LE:
  case R_TILEGX_IMM16_X0_HW0_LAST_TLS_LE:
  case R_TILEGX_IMM16_X1_HW0_LAST_TLS_LE:
  case R_TILEGX_IMM16_X0_HW1_LAST_TLS_LE:
  case R_TILEGX_IMM16_X1_HW1_LAST_TLS_LE:
  case R_TILEGX_IMM16_X0_HW0_LAST_TLS_GD:
  case R_TILEGX_IMM16_X1_HW0_LAST_TLS_GD:
  case R_TILEGX_IMM16_X0_HW1_LAST_TLS_GD:
  case R_TILEGX_IMM16_X1_HW1_LAST_TLS_GD:
  case R_TILEGX_IMM16_X0_HW0_TLS_IE:
  case R_TILEGX_IMM16_X1_HW0_TLS_IE:
  case R_TILEGX_IMM16_X0_HW0_LAST_TLS_IE:
  case R_TILEGX_IMM16_X1_HW0_LAST_TLS_IE:
  case R_TILEGX_IMM16_X0_HW1_LAST_TLS_IE:
  case R_TILEGX_IMM16_X1_HW1_LAST_TLS_IE:
    return true;
  default:
    return Type >= R_TILEGX_TLS_GD_CALL && Type <= R_TILEGX_IMM8_Y1_TLS_ADD;
  }
}

// A plain halfword relocation over sym - label, with the label in the fixup's
// section, reaches here PC-relative and takes the matching _PCREL type, which
// is how tile-as spells the GOT-base and _start address computations. The
// linker only accepts a TLS relocation against an STT_TLS symbol, and an
// undefined TLS variable gets that type only from the relocations against it.
unsigned TileELFObjectWriter::getRelocType(const MCFixup &Fixup,
                                           const MCValue &Target,
                                           bool IsPCRel) const {
  using namespace ELF;
  switch (Fixup.getKind()) {
  case FK_Data_1:
    return IsPCRel ? R_TILEGX_8_PCREL : R_TILEGX_8;
  case FK_Data_2:
    return IsPCRel ? R_TILEGX_16_PCREL : R_TILEGX_16;
  case FK_Data_4:
    return IsPCRel ? R_TILEGX_32_PCREL : R_TILEGX_32;
  case FK_Data_8:
    return IsPCRel ? R_TILEGX_64_PCREL : R_TILEGX_64;
  default:
    break;
  }
  assert(Fixup.getKind() >= FirstTargetFixupKind && "invalid fixup kind");

  unsigned Type = Tile::getFixupReloc(Fixup.getKind());
  if (IsPCRel && !Fixup.isPCRel()) {
    if (Type >= R_TILEGX_IMM16_X0_HW0 && Type <= R_TILEGX_IMM16_X1_HW2_LAST)
      Type += R_TILEGX_IMM16_X0_HW0_PCREL - R_TILEGX_IMM16_X0_HW0;
    else if (!(Type >= R_TILEGX_IMM16_X0_HW0_PCREL &&
               Type <= R_TILEGX_IMM16_X1_HW2_LAST_PCREL) &&
             !(Type >= R_TILEGX_IMM16_X0_HW0_PLT_PCREL &&
               Type <= R_TILEGX_IMM16_X1_HW2_PLT_PCREL) &&
             !(Type >= R_TILEGX_IMM16_X0_HW0_LAST_PLT_PCREL &&
               Type <= R_TILEGX_IMM16_X1_HW2_LAST_PLT_PCREL) &&
             Type != R_TILEGX_IMM16_X0_HW3_PLT_PCREL &&
             Type != R_TILEGX_IMM16_X1_HW3_PLT_PCREL)
      getContext().reportError(Fixup.getLoc(),
                               "relocation has no PC-relative form");
  }

  if (isTlsReloc(Type))
    if (auto *Sym = const_cast<MCSymbol *>(Target.getAddSym()))
      static_cast<MCSymbolELF *>(Sym)->setType(STT_TLS);
  return Type;
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createTileELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<TileELFObjectWriter>(OSABI);
}

// A GOT relocation selects one slot per symbol and the linker adds the addend
// to the slot offset, so a local label must not decay to section + offset.
bool TileELFObjectWriter::needsRelocateWithSymbol(const MCValue &,
                                                  unsigned Type) const {
  switch (Type) {
  case ELF::R_TILEGX_IMM16_X0_HW0_GOT:
  case ELF::R_TILEGX_IMM16_X1_HW0_GOT:
  case ELF::R_TILEGX_IMM16_X0_HW0_LAST_GOT:
  case ELF::R_TILEGX_IMM16_X1_HW0_LAST_GOT:
  case ELF::R_TILEGX_IMM16_X0_HW1_LAST_GOT:
  case ELF::R_TILEGX_IMM16_X1_HW1_LAST_GOT:
    return true;
  default:
    return false;
  }
}
