//===-- TileOpcodeTable.cpp - TILE-Gx ISA table ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TileOpcodeTable.h"
#include "TileBaseInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/BinaryFormat/ELF.h"

using namespace llvm;
using namespace llvm::TileISA;

static const Field Fields[] = {
#define TILE_FIELDS
#include "TileOpcodeTable.inc"
};

static const Operand Operands[] = {
#define TILE_OPERANDS
#include "TileOpcodeTable.inc"
};

static const Opcode Opcodes[] = {
#define TILE_OPCODES
#include "TileOpcodeTable.inc"
};

static const struct {
  unsigned Number;
  const char *Name;
} Sprs[] = {
#define TILE_SPRS
#include "TileOpcodeTable.inc"
};

const Opcode &TileISA::opcode(unsigned Index) { return Opcodes[Index]; }

const Operand &TileISA::operand(const Opcode &Op, Pipe P, unsigned I) {
  return Operands[Op.Operands[P][I]];
}

const Field &TileISA::field(unsigned Index) { return Fields[Index]; }

std::optional<unsigned> TileISA::findOpcode(StringRef Name) {
  static const StringMap<unsigned> Index = [] {
    StringMap<unsigned> Map;
    for (auto [I, Op] : enumerate(Opcodes))
      Map[Op.Name] = I;
    return Map;
  }();
  auto It = Index.find(Name);
  if (It == Index.end())
    return std::nullopt;
  return It->second;
}

std::optional<unsigned> TileISA::findSpr(StringRef Name) {
  static const StringMap<unsigned> Index = [] {
    StringMap<unsigned> Map;
    for (const auto &Spr : Sprs)
      Map[Spr.Name] = Spr.Number;
    return Map;
  }();
  auto It = Index.find(Name);
  if (It == Index.end())
    return std::nullopt;
  return It->second;
}

uint64_t TileISA::insertField(const Field &F, uint64_t Value) {
  uint64_t Bits = 0;
  for (unsigned I = 0; I != F.NumChunks; ++I)
    Bits |= (Value & F.Chunks[I].Mask) << F.Chunks[I].Shift;
  return Bits;
}

std::optional<uint64_t> TileISA::fillerBits(Pipe P, bool Nop) {
  static const unsigned Fnop = *findOpcode("fnop");
  static const unsigned RealNop = *findOpcode("nop");
  const Opcode &Op = Opcodes[Nop ? RealNop : Fnop];
  if (!Op.hasPipe(P))
    return std::nullopt;
  return Op.FixedBits[P];
}

static unsigned fieldId(StringRef Name) {
  for (auto [I, F] : enumerate(Fields))
    if (Name == F.Name)
      return I;
  llvm_unreachable("unknown TILE-Gx field");
}

std::optional<RelocField> TileISA::relocField(unsigned Type) {
  using namespace ELF;
  static const unsigned Imm16[] = {fieldId("Imm16_X0"), fieldId("Imm16_X1")};
  static const unsigned Imm8[] = {fieldId("Imm8_X0"), fieldId("Imm8_X1"),
                                  fieldId("Imm8_Y0"), fieldId("Imm8_Y1")};
  static const unsigned BrOff = fieldId("BrOff_X1");
  static const unsigned JumpOff = fieldId("JumpOff_X1");

  // R_TILEGX_IMM16_* run X0/X1 pairs, the X0 member even. The first 28 are
  // HW0 HW1 HW2 HW3 HW0_LAST HW1_LAST HW2_LAST, plain then PCREL.
  if (Type >= R_TILEGX_IMM16_X0_HW0 && Type <= R_TILEGX_IMM16_X1_HW1_LAST_TLS_IE) {
    static const uint8_t Plain[] = {0, 16, 32, 48, 0, 16, 32};
    unsigned Pair = (Type - R_TILEGX_IMM16_X0_HW0) / 2;
    unsigned Shift;
    if (Pair < 14) {
      Shift = Plain[Pair % 7];
    } else {
      switch (Type & ~1u) {
      case R_TILEGX_IMM16_X0_HW1_PLT_PCREL:
      case R_TILEGX_IMM16_X0_HW1_LAST_GOT:
      case R_TILEGX_IMM16_X0_HW1_LAST_TLS_LE:
      case R_TILEGX_IMM16_X0_HW1_LAST_TLS_GD:
      case R_TILEGX_IMM16_X0_HW1_LAST_PLT_PCREL:
      case R_TILEGX_IMM16_X0_HW1_LAST_TLS_IE:
        Shift = 16;
        break;
      case R_TILEGX_IMM16_X0_HW2_PLT_PCREL:
      case R_TILEGX_IMM16_X0_HW2_LAST_PLT_PCREL:
        Shift = 32;
        break;
      case R_TILEGX_IMM16_X0_HW3_PLT_PCREL:
        Shift = 48;
        break;
      default:
        Shift = 0;
        break;
      }
    }
    return RelocField{Imm16[Type & 1], Shift};
  }

  switch (Type) {
  case R_TILEGX_BROFF_X1:
    return RelocField{BrOff, 3};
  case R_TILEGX_JUMPOFF_X1:
  case R_TILEGX_JUMPOFF_X1_PLT:
  case R_TILEGX_TLS_GD_CALL:
    return RelocField{JumpOff, 3};
  case R_TILEGX_IMM8_X0:
  case R_TILEGX_IMM8_X0_TLS_GD_ADD:
  case R_TILEGX_IMM8_X0_TLS_ADD:
    return RelocField{Imm8[0], 0};
  case R_TILEGX_IMM8_X1:
  case R_TILEGX_IMM8_X1_TLS_GD_ADD:
  case R_TILEGX_IMM8_X1_TLS_ADD:
  case R_TILEGX_TLS_IE_LOAD:
    return RelocField{Imm8[1], 0};
  case R_TILEGX_IMM8_Y0:
  case R_TILEGX_IMM8_Y0_TLS_GD_ADD:
  case R_TILEGX_IMM8_Y0_TLS_ADD:
    return RelocField{Imm8[2], 0};
  case R_TILEGX_IMM8_Y1:
  case R_TILEGX_IMM8_Y1_TLS_GD_ADD:
  case R_TILEGX_IMM8_Y1_TLS_ADD:
    return RelocField{Imm8[3], 0};
  case R_TILEGX_DEST_IMM8_X1:
    return RelocField{fieldId("Dest_Imm8_X1"), 0};
  case R_TILEGX_MT_IMM14_X1:
    return RelocField{fieldId("MT_Imm14_X1"), 0};
  case R_TILEGX_MF_IMM14_X1:
    return RelocField{fieldId("MF_Imm14_X1"), 0};
  case R_TILEGX_MMSTART_X0:
    return RelocField{fieldId("BFStart_X0"), 0};
  case R_TILEGX_MMEND_X0:
    return RelocField{fieldId("BFEnd_X0"), 0};
  case R_TILEGX_SHAMT_X0:
    return RelocField{fieldId("ShAmt_X0"), 0};
  case R_TILEGX_SHAMT_X1:
    return RelocField{fieldId("ShAmt_X1"), 0};
  case R_TILEGX_SHAMT_Y0:
    return RelocField{fieldId("ShAmt_Y0"), 0};
  case R_TILEGX_SHAMT_Y1:
    return RelocField{fieldId("ShAmt_Y1"), 0};
  default:
    return std::nullopt;
  }
}

std::optional<unsigned> TileISA::applyOperator(uint16_t Specifier,
                                               unsigned DefaultReloc) {
  using namespace ELF;
  auto imm16 = [&](unsigned X0Reloc) -> std::optional<unsigned> {
    if (DefaultReloc == R_TILEGX_IMM16_X0_HW0_LAST)
      return X0Reloc;
    if (DefaultReloc == R_TILEGX_IMM16_X1_HW0_LAST)
      return X0Reloc + 1;
    return std::nullopt;
  };
  auto imm8 = [&](unsigned X0, unsigned X1, unsigned Y0,
                  unsigned Y1) -> std::optional<unsigned> {
    switch (DefaultReloc) {
    case R_TILEGX_IMM8_X0:
      return X0;
    case R_TILEGX_IMM8_X1:
      return X1;
    case R_TILEGX_IMM8_Y0:
      return Y0;
    case R_TILEGX_IMM8_Y1:
      return Y1;
    default:
      return std::nullopt;
    }
  };

  switch (Specifier) {
  case TileII::MO_HW0:
    return imm16(R_TILEGX_IMM16_X0_HW0);
  case TileII::MO_HW1:
    return imm16(R_TILEGX_IMM16_X0_HW1);
  case TileII::MO_HW2:
    return imm16(R_TILEGX_IMM16_X0_HW2);
  case TileII::MO_HW3:
    return imm16(R_TILEGX_IMM16_X0_HW3);
  case TileII::MO_HW0_LAST:
    return imm16(R_TILEGX_IMM16_X0_HW0_LAST);
  case TileII::MO_HW1_LAST:
    return imm16(R_TILEGX_IMM16_X0_HW1_LAST);
  case TileII::MO_HW2_LAST:
    return imm16(R_TILEGX_IMM16_X0_HW2_LAST);
  case TileII::MO_HW0_PIC:
    return imm16(R_TILEGX_IMM16_X0_HW0_PCREL);
  case TileII::MO_HW1_LAST_PIC:
    return imm16(R_TILEGX_IMM16_X0_HW1_LAST_PCREL);
  case TileII::MO_HW0_GOT:
    return imm16(R_TILEGX_IMM16_X0_HW0_GOT);
  case TileII::MO_HW0_LAST_GOT:
    return imm16(R_TILEGX_IMM16_X0_HW0_LAST_GOT);
  case TileII::MO_HW1_LAST_GOT:
    return imm16(R_TILEGX_IMM16_X0_HW1_LAST_GOT);
  case TileII::MO_HW0_TLS_GD:
    return imm16(R_TILEGX_IMM16_X0_HW0_TLS_GD);
  case TileII::MO_HW0_LAST_TLS_GD:
    return imm16(R_TILEGX_IMM16_X0_HW0_LAST_TLS_GD);
  case TileII::MO_HW1_LAST_TLS_GD:
    return imm16(R_TILEGX_IMM16_X0_HW1_LAST_TLS_GD);
  case TileII::MO_HW0_TLS_IE:
    return imm16(R_TILEGX_IMM16_X0_HW0_TLS_IE);
  case TileII::MO_HW0_LAST_TLS_IE:
    return imm16(R_TILEGX_IMM16_X0_HW0_LAST_TLS_IE);
  case TileII::MO_HW1_LAST_TLS_IE:
    return imm16(R_TILEGX_IMM16_X0_HW1_LAST_TLS_IE);
  case TileII::MO_HW0_TLS_LE:
    return imm16(R_TILEGX_IMM16_X0_HW0_TLS_LE);
  case TileII::MO_HW0_LAST_TLS_LE:
    return imm16(R_TILEGX_IMM16_X0_HW0_LAST_TLS_LE);
  case TileII::MO_HW1_LAST_TLS_LE:
    return imm16(R_TILEGX_IMM16_X0_HW1_LAST_TLS_LE);
  case TileII::MO_HW0_PLT:
    return imm16(R_TILEGX_IMM16_X0_HW0_PLT_PCREL);
  case TileII::MO_HW1_PLT:
    return imm16(R_TILEGX_IMM16_X0_HW1_PLT_PCREL);
  case TileII::MO_HW1_LAST_PLT:
    return imm16(R_TILEGX_IMM16_X0_HW1_LAST_PLT_PCREL);
  case TileII::MO_HW2_LAST_PLT:
    return imm16(R_TILEGX_IMM16_X0_HW2_LAST_PLT_PCREL);
  case TileII::MO_PLT_CALL:
    if (DefaultReloc == R_TILEGX_JUMPOFF_X1)
      return R_TILEGX_JUMPOFF_X1_PLT;
    return std::nullopt;
  case TileII::MO_TLS_GD_CALL:
    if (DefaultReloc == R_TILEGX_JUMPOFF_X1)
      return R_TILEGX_TLS_GD_CALL;
    return std::nullopt;
  case TileII::MO_TLS_IE_LOAD:
    if (DefaultReloc == R_TILEGX_IMM8_X1)
      return R_TILEGX_TLS_IE_LOAD;
    return std::nullopt;
  case TileII::MO_TLS_GD_ADD:
    return imm8(R_TILEGX_IMM8_X0_TLS_GD_ADD, R_TILEGX_IMM8_X1_TLS_GD_ADD,
                R_TILEGX_IMM8_Y0_TLS_GD_ADD, R_TILEGX_IMM8_Y1_TLS_GD_ADD);
  case TileII::MO_TLS_ADD:
    return imm8(R_TILEGX_IMM8_X0_TLS_ADD, R_TILEGX_IMM8_X1_TLS_ADD,
                R_TILEGX_IMM8_Y0_TLS_ADD, R_TILEGX_IMM8_Y1_TLS_ADD);
  default:
    return std::nullopt;
  }
}

bool TileISA::operatorNeedsSymbol(uint16_t Specifier) {
  switch (Specifier) {
  case TileII::MO_HW0_GOT:
  case TileII::MO_HW0_LAST_GOT:
  case TileII::MO_HW1_LAST_GOT:
  case TileII::MO_HW0_TLS_GD:
  case TileII::MO_HW0_LAST_TLS_GD:
  case TileII::MO_HW1_LAST_TLS_GD:
  case TileII::MO_HW0_TLS_IE:
  case TileII::MO_HW0_LAST_TLS_IE:
  case TileII::MO_HW1_LAST_TLS_IE:
  case TileII::MO_HW0_TLS_LE:
  case TileII::MO_HW0_LAST_TLS_LE:
  case TileII::MO_HW1_LAST_TLS_LE:
  case TileII::MO_PLT_CALL:
  case TileII::MO_TLS_GD_CALL:
  case TileII::MO_TLS_GD_ADD:
  case TileII::MO_TLS_IE_LOAD:
  case TileII::MO_TLS_ADD:
    return true;
  default:
    return false;
  }
}
