//===-- Tile.td - Describe the Tile Target Machine ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This is the top level entry point for the Tile target.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces.
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions.
//===----------------------------------------------------------------------===//

include "TileRegisterInfo.td"
include "TileSchedule.td"
include "TileInstrInfo.td"
include "TileCallingConv.td"

def TileInstrInfo : InstrInfo;

// Map the pointer operands of LLVM's standard target-independent pseudos
// (LOAD_STACK_GUARD, PREALLOCATED_*, PATCHABLE_*) onto Tile's GPR class.
defm : RemapAllTargetPseudoPointerOperands<CPURegs>;

//===----------------------------------------------------------------------===//
// Tile processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, TileItineraries, Features>;

def : Proc<"tilegx", []>;

def TileAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  bit isMCAsmWriter = 1;
}

def TileAsmParser : AsmParser {
  let ShouldEmitMatchRegisterName = 0;
}

def Tile : Target {
  let InstructionSet = TileInstrInfo;
  let AssemblyWriters = [TileAsmWriter];
  let AssemblyParsers = [TileAsmParser];
}

