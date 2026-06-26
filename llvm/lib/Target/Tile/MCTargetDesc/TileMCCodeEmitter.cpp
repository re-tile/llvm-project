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

#include "MCTargetDesc/TileFixupKinds.h"
#include "MCTargetDesc/TileMCTargetDesc.h"
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
class TileMCCodeEmitter : public MCCodeEmitter {
  MCContext &Ctx;

public:
  TileMCCodeEmitter(const MCInstrInfo &, MCContext &ctx) : Ctx(ctx) {}
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
  uint64_t Bundle = getBinaryCodeForInstr(MI, Fixups, STI);
  support::endian::write<uint64_t>(CB, Bundle, llvm::endianness::little);
}

unsigned
TileMCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  // Symbol-relative immediates require the hw0/hw1/got/tls relocation
  // specifiers, which are deferred to the relocation phase.
  report_fatal_error("Tile: symbol-relative operand encoding not yet "
                     "implemented");
}

unsigned
TileMCCodeEmitter::getJumpTargetOpValue(const MCInst &MI, unsigned OpNo,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  assert(MO.isExpr() && "getJumpTargetOpValue expects an expression");
  Fixups.push_back(MCFixup::create(0, MO.getExpr(),
                                   MCFixupKind(Tile::fixup_Tile_X1_JUMPOFF),
                                   /*PCRel=*/true));
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
