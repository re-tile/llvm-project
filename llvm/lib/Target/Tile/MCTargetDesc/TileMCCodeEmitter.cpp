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

  assert(MO.isExpr() && "unexpected operand kind");
  const MCExpr *Expr = MO.getExpr();

  // A halfword-extraction relocation specifier (hw0/hw1/hw2_last) drives the
  // moveli/shl16insli address sequence. moveli and shl16insli are X1-pipe in
  // the unpacketized bundle, so every such operand records an X1 fixup; the
  // value bits stay zero and the linker patches them through the recorded
  // relocation.
  if (const auto *SE = dyn_cast<MCSpecifierExpr>(Expr)) {
    Tile::Fixups Kind;
    // The GOT base self-computation feeds a sym - pclabel difference through the
    // PCREL halfword fixups; mark them PC-relative so MC folds the local label
    // into the addend instead of rejecting the subtraction.
    bool PCRel = false;
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
    // PCREL relocation is selected here and matches GNU as.
    case TileII::MO_HW0_PIC:
      Kind = Tile::fixup_Tile_X1_HW0_PCREL;
      PCRel = true;
      break;
    case TileII::MO_HW1_LAST_PIC:
      Kind = Tile::fixup_Tile_X1_HW1_LAST_PCREL;
      PCRel = true;
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
    Fixups.push_back(MCFixup::create(0, Expr, MCFixupKind(Kind), PCRel));
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
