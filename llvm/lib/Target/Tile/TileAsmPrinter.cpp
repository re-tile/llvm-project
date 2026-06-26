//===-- TileAsmPrinter.cpp - Tile LLVM Assembly Printer -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format Tile assembly language.
//
// First-light subset: the packetizer is disabled, so each instruction is
// emitted solo (no VLIW packet braces). Relocation specifiers on operands are
// staged in with the integrated assembler in a later phase.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TileInstPrinter.h"
#include "Tile.h"
#include "TargetInfo/TileTargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "tile-asm-printer"

namespace {

class TileAsmPrinter : public AsmPrinter {
public:
  explicit TileAsmPrinter(TargetMachine &TM,
                          std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer), ID) {}

  StringRef getPassName() const override { return "Tile Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       const char *ExtraCode, raw_ostream &O) override;
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum,
                             const char *ExtraCode, raw_ostream &O) override;

  static char ID;

private:
  void printOperand(const MachineInstr *MI, int opNum, raw_ostream &O);
};

} // end anonymous namespace

char TileAsmPrinter::ID = 0;

void TileAsmPrinter::emitInstruction(const MachineInstr *MI) {
  if (MI->isDebugInstr())
    return;

  MCInst TmpInst;
  LowerTileMachineInstrToMCInst(MI, TmpInst, *this);
  EmitToStreamer(*OutStreamer, TmpInst);
}

void TileAsmPrinter::printOperand(const MachineInstr *MI, int opNum,
                                  raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(opNum);
  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    O << StringRef(TileInstPrinter::getRegisterName(MO.getReg())).lower();
    break;
  case MachineOperand::MO_Immediate:
    O << MO.getImm();
    break;
  case MachineOperand::MO_MachineBasicBlock:
    MO.getMBB()->getSymbol()->print(O, MAI);
    break;
  case MachineOperand::MO_GlobalAddress:
    getSymbol(MO.getGlobal())->print(O, MAI);
    break;
  default:
    llvm_unreachable("<unknown operand type>");
  }
}

bool TileAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                     const char *ExtraCode, raw_ostream &O) {
  if (ExtraCode && ExtraCode[0]) {
    if (ExtraCode[1] != 0)
      return true; // Unknown modifier.
    return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, O);
  }

  printOperand(MI, OpNo, O);
  return false;
}

bool TileAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                           unsigned OpNum, const char *ExtraCode,
                                           raw_ostream &O) {
  if (ExtraCode && ExtraCode[0])
    return true; // Unknown modifier.

  const MachineOperand &MO = MI->getOperand(OpNum);
  assert(MO.isReg() && "unexpected inline asm memory operand");
  O << StringRef(TileInstPrinter::getRegisterName(MO.getReg())).lower();
  return false;
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeTileAsmPrinter() {
  RegisterAsmPrinter<TileAsmPrinter> X(getTheTileTarget());
}
