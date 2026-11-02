//===-- TileFixupKinds.h - Tile Specific Fixup Entries ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TILE_TILEFIXUPKINDS_H
#define LLVM_TILE_TILEFIXUPKINDS_H

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Tile {
// A target fixup kind is FirstTargetFixupKind plus the R_TILEGX_* relocation
// it becomes, so the asm parser can create any relocation the ISA has.
inline MCFixupKind getFixupKind(unsigned Reloc) {
  return MCFixupKind(FirstTargetFixupKind + Reloc);
}

inline unsigned getFixupReloc(MCFixupKind Kind) {
  return Kind - FirstTargetFixupKind;
}

enum Fixups {
  fixup_Tile_X0_HW0 = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW0,
  fixup_Tile_X1_HW0 = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW0,
  fixup_Tile_X0_HW0_PCREL = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW0_PCREL,
  fixup_Tile_X1_HW0_PCREL = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW0_PCREL,
  fixup_Tile_X0_HW0_GOT = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW0_GOT,
  fixup_Tile_X1_HW0_GOT = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW0_GOT,
  fixup_Tile_X0_HW1 = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW1,
  fixup_Tile_X1_HW1 = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW1,
  fixup_Tile_X0_HW1_PCREL = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW1_PCREL,
  fixup_Tile_X1_HW1_PCREL = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW1_PCREL,
  fixup_Tile_X0_HW1_LAST = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW1_LAST,
  fixup_Tile_X1_HW1_LAST = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW1_LAST,
  fixup_Tile_X0_HW1_LAST_PCREL = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW1_LAST_PCREL,
  fixup_Tile_X1_HW1_LAST_PCREL = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW1_LAST_PCREL,
  fixup_Tile_X0_HW1_LAST_GOT = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW1_LAST_GOT,
  fixup_Tile_X1_HW1_LAST_GOT = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW1_LAST_GOT,
  fixup_Tile_X0_HW2_LAST = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X0_HW2_LAST,
  fixup_Tile_X1_HW2_LAST = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW2_LAST,
  fixup_Tile_X1_JUMPOFF = FirstTargetFixupKind + ELF::R_TILEGX_JUMPOFF_X1,
  fixup_Tile_X1_JUMPOFF_PLT = FirstTargetFixupKind + ELF::R_TILEGX_JUMPOFF_X1_PLT,
  fixup_Tile_X1_BROFF = FirstTargetFixupKind + ELF::R_TILEGX_BROFF_X1,
  // Thread-local storage halfword and relaxation-hint relocations.
  fixup_Tile_X1_HW0_TLS_GD = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW0_TLS_GD,
  fixup_Tile_X1_HW1_LAST_TLS_GD = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW1_LAST_TLS_GD,
  fixup_Tile_X1_HW0_TLS_IE = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW0_TLS_IE,
  fixup_Tile_X1_HW1_LAST_TLS_IE = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW1_LAST_TLS_IE,
  fixup_Tile_X1_HW0_TLS_LE = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW0_TLS_LE,
  fixup_Tile_X1_HW1_LAST_TLS_LE = FirstTargetFixupKind + ELF::R_TILEGX_IMM16_X1_HW1_LAST_TLS_LE,
  fixup_Tile_X1_TLS_GD_CALL = FirstTargetFixupKind + ELF::R_TILEGX_TLS_GD_CALL,
  fixup_Tile_X1_TLS_GD_ADD = FirstTargetFixupKind + ELF::R_TILEGX_IMM8_X1_TLS_GD_ADD,
  fixup_Tile_X1_TLS_ADD = FirstTargetFixupKind + ELF::R_TILEGX_IMM8_X1_TLS_ADD,
  fixup_Tile_X1_TLS_IE_LOAD = FirstTargetFixupKind + ELF::R_TILEGX_TLS_IE_LOAD,
};
} // namespace Tile
} // namespace llvm

#endif // LLVM_TILE_TILEFIXUPKINDS_H
