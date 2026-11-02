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
#include "TileMCTargetDesc.h"
#include "TileOpcodeTable.h"
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

static std::string tableRegisterName(int64_t N) {
  static const char *const Named[] = {"tp",   "sp",   "lr",   "sn",
                                      "idn0", "idn1", "udn0", "udn1",
                                      "udn2", "udn3", "zero"};
  if (N >= 53)
    return Named[N - 53];
  return "r" + std::to_string(N);
}

void TileInstPrinter::printTableInst(const MCInst *MI, raw_ostream &O) {
  const TileISA::Opcode &Op = TileISA::opcode(MI->getOperand(0).getImm());
  auto P = TileISA::Pipe(MI->getOperand(1).getImm());
  O << Op.Name;
  for (unsigned I = 0; I != Op.NumOperands; ++I) {
    O << (I ? ", " : "\t");
    const MCOperand &MO = MI->getOperand(I + 2);
    if (TileISA::operand(Op, P, I).Kind == TileISA::OperandKind::Register)
      O << tableRegisterName(MO.getImm());
    else if (MO.isImm())
      O << MO.getImm();
    else
      MAI.printExpr(O, *MO.getExpr());
  }
}

void TileInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                StringRef Annot, const MCSubtargetInfo &STI,
                                raw_ostream &O) {
  // A packed VLIW bundle prints as a brace-delimited, semicolon-separated list
  // of its issue-slot instructions: { a ; b }. A single instruction prints bare
  // (its complementary FNOP slot is implicit), matching GNU tile-as.
  if (MI->getOpcode() == Tile::BUNDLE && MI->getNumOperands() == 1 &&
      MI->getOperand(0).getInst()->getOpcode() == Tile::TABLE_INST) {
    O << '\t';
    printTableInst(MI->getOperand(0).getInst(), O);
    printAnnotation(O, Annot);
    return;
  }

  if (MI->getOpcode() == Tile::BUNDLE) {
    O << "\t{ ";
    bool First = true;
    for (const MCOperand &Op : *MI) {
      const MCInst &Sub = *Op.getInst();
      if (!First)
        O << " ; ";
      First = false;
      std::string Buf;
      raw_string_ostream SubOS(Buf);
      if (Sub.getOpcode() == Tile::TABLE_INST)
        printTableInst(&Sub, SubOS);
      else
        printInstruction(&Sub, Address, SubOS);
      O << StringRef(Buf).trim();
    }
    O << " }";
    printAnnotation(O, Annot);
    return;
  }

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
  assert(Op.isReg());
  // The PC-anchor label that GNU as writes as `L = . + 8` is emitted by the
  // AsmPrinter as a real label after this bundle, so both the textual and the
  // object paths agree; here we only print the lnk itself.
  O << "lnk\t" << StringRef(getRegisterName(Op.getReg())).lower();
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
