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

#include "MCTargetDesc/TileBaseInfo.h"
#include "MCTargetDesc/TileInstPrinter.h"
#include "MCTargetDesc/TileOpcodeTable.h"
#include "Tile.h"
#include "TileMachineFunction.h"
#include "TargetInfo/TileTargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
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
  bool emitSequence(const MachineInstr *MI);
  void emitGotSetup();

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

// When a function references the GOT (any PIC global, jump table, constant
// pool or general-dynamic/initial-exec TLS access), the GOT base must be loaded
// into r51 before first use. There is no fixed GOT-base register in the ABI, so
// it is recomputed PC-relatively: lnk loads the address of the next bundle, then
// the difference _GLOBAL_OFFSET_TABLE_ - .Llabel is added back. The plain
// hw1_last/hw0 operators over a sym - label difference assemble to the PCREL
// relocations, exactly as GNU as emits them.
void TileAsmPrinter::emitGotSetup() {
  const auto *FI = MF->getInfo<TileFunctionInfo>();
  MCContext &Ctx = OutContext;
  Register GotReg = const_cast<TileFunctionInfo *>(FI)->getGlobalBaseReg();
  Register LnkReg = const_cast<TileFunctionInfo *>(FI)->getLinkReg();

  // lnk r50 ; the label sits at the next bundle, which is what lnk loads.
  EmitToStreamer(*OutStreamer,
                 MCInstBuilder(Tile::LNK).addReg(LnkReg).addImm(0));
  MCSymbol *PcLabel = const_cast<TileFunctionInfo *>(FI)->getPicLabel(Ctx);
  OutStreamer->emitLabel(PcLabel);

  MCSymbol *GotSym = Ctx.getOrCreateSymbol("_GLOBAL_OFFSET_TABLE_");
  const MCExpr *Diff = MCBinaryExpr::createSub(
      MCSymbolRefExpr::create(GotSym, Ctx),
      MCSymbolRefExpr::create(PcLabel, Ctx), Ctx);
  const MCExpr *Hi = MCSpecifierExpr::create(Diff, TileII::MO_HW1_LAST_PIC, Ctx);
  const MCExpr *Lo = MCSpecifierExpr::create(Diff, TileII::MO_HW0_PIC, Ctx);

  EmitToStreamer(*OutStreamer,
                 MCInstBuilder(Tile::MOVELI).addReg(GotReg).addExpr(Hi));
  EmitToStreamer(*OutStreamer, MCInstBuilder(Tile::SHL16INSLI)
                                   .addReg(GotReg)
                                   .addReg(GotReg)
                                   .addExpr(Lo));
  EmitToStreamer(*OutStreamer, MCInstBuilder(Tile::ADD)
                                   .addReg(GotReg)
                                   .addReg(GotReg)
                                   .addReg(LnkReg));
}

// Codegen-only instructions that stand for two bundles. Their TableGen
// records carry only the first instruction's encoding.
bool TileAsmPrinter::emitSequence(const MachineInstr *MI) {
  auto Reg = [&](unsigned I) -> int64_t {
    return MF->getSubtarget().getRegisterInfo()->getEncodingValue(
        MI->getOperand(I).getReg());
  };
  auto Emit = [&](StringRef Mnemonic, ArrayRef<int64_t> Values) {
    MCInst Bundle;
    lowerTileTableBundle(Mnemonic, Values, Bundle, OutContext);
    EmitToStreamer(*OutStreamer, Bundle);
  };

  switch (MI->getOpcode()) {
  case Tile::XDN_SEND:
    Emit("move", {MI->getOperand(1).getImm(), Reg(0)});
    return true;
  case Tile::XDN_RECV:
    Emit("move", {Reg(0), MI->getOperand(1).getImm()});
    return true;
  case Tile::NET_BARRIER:
    return true;
  case Tile::SHUFFLEBYTES1:
    Emit("shufflebytes", {Reg(0), Reg(1), Reg(2)});
    return true;
  case Tile::CLZ32:
    Emit("shli", {Reg(0), Reg(1), 32});
    Emit("clz", {Reg(0), Reg(0)});
    return true;
  case Tile::REVBYTES32:
    Emit("revbytes", {Reg(0), Reg(1)});
    Emit("shrui", {Reg(0), Reg(0), 32});
    return true;
  case Tile::ATOMIC_CMP_SWAP:
  case Tile::ATOMIC_CMP_SWAP32: {
    int64_t CmpExchValue = *TileISA::findSpr("CMPEXCH_VALUE");
    Emit("mtspr", {CmpExchValue, Reg(2)});
    Emit(MI->getOpcode() == Tile::ATOMIC_CMP_SWAP ? "cmpexch" : "cmpexch4",
         {Reg(0), Reg(1), Reg(3)});
    return true;
  }
  default:
    return false;
  }
}

void TileAsmPrinter::emitInstruction(const MachineInstr *MI) {
  if (MI->isDebugInstr())
    return;

  if (MI->getOpcode() == Tile::GOT_SETUP) {
    emitGotSetup();
    return;
  }

  if (emitSequence(MI))
    return;

  // A packetized bundle is emitted as a single BUNDLE MCInst whose operands are
  // the issue-slot sub-instructions. The streamer hands it to the code emitter,
  // which merges the slots into one 64-bit word, and to the instruction printer,
  // which renders the { a ; b } syntax.
  if (MI->isBundle()) {
    SmallVector<MCInst *, 2> Subs;
    const MachineBasicBlock *MBB = MI->getParent();
    MachineBasicBlock::const_instr_iterator I = MI->getIterator();
    for (++I; I != MBB->instr_end() && I->isInsideBundle(); ++I) {
      // Meta instructions (KILL, IMPLICIT_DEF, CFI, debug values, ...) carry no
      // encoding. The generic AsmPrinter filters them for unbundled code, but
      // this hand-rolled bundle walk has to do it itself or the code emitter
      // trips over an unencodable sub-instruction.
      if (I->isMetaInstruction())
        continue;
      MCInst *Sub = OutContext.createMCInst();
      LowerTileMachineInstrToMCInst(&*I, *Sub, *this);
      Subs.push_back(Sub);
    }
    // The packetizer leaves both ALU ops in their native X1 encoding. Choose the
    // X0/X1 slots and emission order canonically here so the asm and object paths
    // (whose machine-level member order is not stable) agree, and so tile-as
    // reproduces the bytes from the printed text.
    if (Subs.size() == 2)
      if (const char *Err = TileII::orderBundleForEmission(
              Subs[0], Subs[1], *TM.getMCInstrInfo()))
        report_fatal_error(Twine("Tile: ") + Err);
    MCInst Bundle;
    Bundle.setOpcode(Tile::BUNDLE);
    for (MCInst *Sub : Subs)
      Bundle.addOperand(MCOperand::createInst(Sub));
    EmitToStreamer(*OutStreamer, Bundle);
    return;
  }

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
