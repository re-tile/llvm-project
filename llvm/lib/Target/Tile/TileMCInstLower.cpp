//===-- TileMCInstLower.cpp - Convert Tile MachineInstr to MCInst ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower Tile MachineInstrs to their corresponding
// MCInst records.
//
// First-light subset: the packetizer is disabled, so every instruction is solo
// and maps directly to its base opcode. Relocation specifiers (hw0/hw1/plt/tls)
// are staged in with the integrated assembler in a later phase.
//
//===----------------------------------------------------------------------===//

#include "Tile.h"
#include "MCTargetDesc/TileBaseInfo.h"
#include "MCTargetDesc/TileOpcodeTable.h"
#include "TileMachineFunction.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"

using namespace llvm;

static MCOperand lowerSymbolOperand(const MachineOperand &MO,
                                    const MCSymbol *Symbol, AsmPrinter &AP) {
  MCContext &Ctx = AP.OutContext;
  const MCExpr *Expr = MCSymbolRefExpr::create(Symbol, Ctx);
  if (!MO.isJTI() && !MO.isMBB() && MO.getOffset())
    Expr = MCBinaryExpr::createAdd(
        Expr, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);

  // The hw0/hw1/hw2_last (and GOT/TLS) halfword-extraction relocation specifier
  // rides on the machine-operand target flag. The TileII::TOF value is used
  // directly as the MCSpecifierExpr code and decoded by the code emitter and
  // the asm printer.
  unsigned Flag = MO.getTargetFlags();
  if (Flag == TileII::MO_HW1_LAST_PIC || Flag == TileII::MO_HW0_PIC) {
    MCSymbol *PicLabel =
        AP.MF->getInfo<TileFunctionInfo>()->getPicLabel(Ctx);
    Expr = MCBinaryExpr::createSub(Expr, MCSymbolRefExpr::create(PicLabel, Ctx),
                                   Ctx);
  }
  if (Flag != TileII::MO_NO_FLAG)
    Expr = MCSpecifierExpr::create(Expr, Flag, Ctx);

  return MCOperand::createExpr(Expr);
}

// A one-slot assembler bundle of the tile-as mnemonic, in the first of Y2, X1
// and X0 it issues in, as tile-as would place it.
void llvm::lowerTileTableBundle(StringRef Mnemonic, ArrayRef<int64_t> Values,
                                MCInst &OutMI, MCContext &Ctx) {
  using namespace TileISA;
  unsigned Index = *findOpcode(Mnemonic);
  const Opcode &Op = opcode(Index);
  Pipe P = Op.hasPipe(PipeY2) ? PipeY2 : Op.hasPipe(PipeX1) ? PipeX1 : PipeX0;

  MCInst *Sub = Ctx.createMCInst();
  Sub->setOpcode(Tile::TABLE_INST);
  Sub->addOperand(MCOperand::createImm(Index));
  Sub->addOperand(MCOperand::createImm(P));
  for (int64_t Value : Values)
    Sub->addOperand(MCOperand::createImm(Value));
  OutMI.setOpcode(Tile::BUNDLE);
  OutMI.addOperand(MCOperand::createInst(Sub));
}

static void lowerTableOp(const MachineInstr *MI, StringRef Mnemonic,
                         MCInst &OutMI, AsmPrinter &AP) {
  const TargetRegisterInfo *TRI = AP.MF->getSubtarget().getRegisterInfo();
  SmallVector<int64_t, 4> Values;
  for (const MachineOperand &MO : MI->operands())
    if (MO.isReg() && !(MO.isUse() && MO.isTied()))
      Values.push_back(TRI->getEncodingValue(MO.getReg()));
    else if (MO.isImm())
      Values.push_back(MO.getImm());
  lowerTileTableBundle(Mnemonic, Values, OutMI, AP.OutContext);
}

void llvm::LowerTileMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                         AsmPrinter &AP) {
  StringRef Name =
      AP.MF->getSubtarget().getInstrInfo()->getName(MI->getOpcode());
  if (Name.consume_front("TBL_"))
    return lowerTableOp(MI, Name.lower(), OutMI, AP);

  OutMI.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    switch (MO.getType()) {
    default:
      llvm_unreachable("unknown operand type");
    case MachineOperand::MO_Register:
      assert(!MO.getSubReg() && "Subregs should be eliminated!");
      MCOp = MCOperand::createReg(MO.getReg());
      break;
    case MachineOperand::MO_Immediate:
      MCOp = MCOperand::createImm(MO.getImm());
      break;
    case MachineOperand::MO_MachineBasicBlock:
      MCOp = MCOperand::createExpr(
          MCSymbolRefExpr::create(MO.getMBB()->getSymbol(), AP.OutContext));
      break;
    case MachineOperand::MO_GlobalAddress:
      MCOp = lowerSymbolOperand(MO, AP.getSymbol(MO.getGlobal()), AP);
      break;
    case MachineOperand::MO_ExternalSymbol:
      MCOp = lowerSymbolOperand(
          MO, AP.GetExternalSymbolSymbol(MO.getSymbolName()), AP);
      break;
    case MachineOperand::MO_JumpTableIndex:
      MCOp = lowerSymbolOperand(MO, AP.GetJTISymbol(MO.getIndex()), AP);
      break;
    case MachineOperand::MO_ConstantPoolIndex:
      MCOp = lowerSymbolOperand(MO, AP.GetCPISymbol(MO.getIndex()), AP);
      break;
    case MachineOperand::MO_BlockAddress:
      MCOp = lowerSymbolOperand(
          MO, AP.GetBlockAddressSymbol(MO.getBlockAddress()), AP);
      break;
    case MachineOperand::MO_RegisterMask:
      continue;
    }

    OutMI.addOperand(MCOp);
  }
}
