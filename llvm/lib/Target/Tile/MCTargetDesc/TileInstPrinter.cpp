//===-- TileInstPrinter.cpp - Convert Tile MCInst to assembly syntax ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints a Tile MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "TileInstPrinter.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#include "TileGenAsmWriter.inc"

void TileInstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) {
  OS << StringRef(getRegisterName(Reg)).lower();
}

void TileInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                StringRef Annot, const MCSubtargetInfo &STI,
                                raw_ostream &O) {
  // Packetizer is disabled for first light: one instruction per bundle, so we
  // print each instruction standalone without VLIW packet braces.
  printInstruction(MI, Address, O);
  printAnnotation(O, Annot);
}

void TileInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  MAI.printExpr(O, *Op.getExpr());
}

void TileInstPrinter::printOperand(const MCInst *MI, uint64_t Address,
                                   unsigned OpNo, raw_ostream &O) {
  printOperand(MI, OpNo, O);
}

void TileInstPrinter::printUnsignedImm(const MCInst *MI, int opNum,
                                       raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(opNum);
  if (MO.isImm())
    O << (unsigned short int)MO.getImm();
  else
    printOperand(MI, opNum, O);
}

bool TileInstPrinter::printPICLNKReg(const MCInst *MI, unsigned opNum,
                                     raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(opNum);
  const MCOperand &FName = MI->getOperand(opNum + 1);

  assert(Op.isReg());

  MAI.printExpr(O, *FName.getExpr());
  O << " = . + 8" << '\n';
  O << "\tlnk " << getRegisterName(Op.getReg());

  return true;
}

void TileInstPrinter::printS16ImmOperand(const MCInst *MI, unsigned OpNo,
                                         raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm())
    O << (short)Op.getImm();
  else if (Op.isExpr())
    MAI.printExpr(O, *Op.getExpr());
  else
    llvm_unreachable("only Imm && Expr should enter here!\n");
}
