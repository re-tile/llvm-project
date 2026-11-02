//===-- TileOpcodeTable.h - TILE-Gx ISA table -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The TILE-Gx instruction set as tile-as describes it, generated into
// TileOpcodeTable.inc. The assembler parses, bundles and encodes through this
// table. Codegen keeps its own TableGen instruction definitions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TILE_MCTARGETDESC_TILEOPCODETABLE_H
#define LLVM_LIB_TARGET_TILE_MCTARGETDESC_TILEOPCODETABLE_H

#include "llvm/ADT/StringRef.h"
#include <cstdint>
#include <optional>

namespace llvm {
namespace TileISA {

enum Pipe : uint8_t { PipeX0, PipeX1, PipeY0, PipeY1, PipeY2, NumPipes };

enum class OperandKind : uint8_t { Register, Immediate, Address, Spr };

struct FieldChunk {
  uint64_t Mask;
  uint8_t Shift;
};

// One instruction field. A value lands in the bundle as the OR of
// (value & Mask) << Shift over the chunks.
struct Field {
  const char *Name;
  uint8_t NumChunks;
  FieldChunk Chunks[3];
};

struct Operand {
  OperandKind Kind;
  uint16_t DefaultReloc;
  uint8_t NumBits;
  bool Signed;
  bool Src;
  bool Dest;
  bool PCRel;
  uint8_t RightShift;
  uint8_t FieldId;
};

struct Opcode {
  const char *Name;
  uint8_t Pipes;
  uint8_t NumOperands;
  uint8_t ImplicitReg;
  bool CanBundle;
  uint8_t Operands[NumPipes][4];
  uint64_t FixedMasks[NumPipes];
  uint64_t FixedBits[NumPipes];

  bool hasPipe(Pipe P) const { return Pipes & (1u << P); }
};

const Opcode &opcode(unsigned Index);
const Operand &operand(const Opcode &Op, Pipe P, unsigned I);
const Field &field(unsigned Index);
std::optional<unsigned> findOpcode(StringRef Name);
std::optional<unsigned> findSpr(StringRef Name);
uint64_t insertField(const Field &F, uint64_t Value);

// Bits of the fnop, or with Nop set the nop, that fills an empty slot, or
// nullopt for Y2.
std::optional<uint64_t> fillerBits(Pipe P, bool Nop);

// Where a relocation of this type writes in the bundle and how far the value
// is shifted right first, or nullopt for a data or marker relocation.
struct RelocField {
  unsigned FieldId;
  unsigned ValueShift;
};
std::optional<RelocField> relocField(unsigned Type);

// The relocation an operator such as hw1_last or tls_add selects for an
// operand whose default relocation is DefaultReloc, or nullopt when tile-as
// rejects the operator on that operand.
std::optional<unsigned> applyOperator(uint16_t Specifier, unsigned DefaultReloc);

// GOT, TLS and plt() operators name one symbol's slot, so tile-as rejects an
// addend or difference under them.
bool operatorNeedsSymbol(uint16_t Specifier);

} // namespace TileISA
} // namespace llvm

#endif
