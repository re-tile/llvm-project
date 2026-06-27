//===-- TileISelLowering.cpp - Tile DAG Lowering Implementation -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Tile uses to lower LLVM code into a
// selection DAG.
//
// First-light subset: enough to lower integer formal arguments and returns for
// simple functions. Calls, byval, varargs, address/TLS and float lowering are
// staged in over later phases.
//
//===----------------------------------------------------------------------===//

#include "TileISelLowering.h"
#include "MCTargetDesc/TileBaseInfo.h"
#include "TileCallingConv.h"
#include "TileMachineFunction.h"
#include "TileSubtarget.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "tile-lower"

static const MCPhysReg TileIntRegs[TILEGX_AREG_NUM] = {
    Tile::R0, Tile::R1, Tile::R2, Tile::R3, Tile::R4,
    Tile::R5, Tile::R6, Tile::R7, Tile::R8, Tile::R9};

static bool CC_TileByval(unsigned ValNo, MVT ValVT, MVT LocVT,
                         CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                         CCState &State) {
  Align Alignment = std::max(ArgFlags.getNonZeroByValAlign(), Align(8));
  unsigned Size = alignTo(ArgFlags.getByValSize(), Alignment);
  unsigned FirstIdx = State.getFirstUnallocated(TileIntRegs);

  assert(Alignment <= Align(16) && "Cannot handle alignments larger than 16.");

  // If byval is 16-byte aligned, the first arg register must be even.
  if (Alignment == Align(16) && (FirstIdx % 2)) {
    State.AllocateReg(TileIntRegs[FirstIdx]);
    ++FirstIdx;
  }

  for (unsigned I = FirstIdx; Size && (I < TILEGX_AREG_NUM); Size -= 8, ++I)
    State.AllocateReg(TileIntRegs[I]);

  unsigned Offset = State.AllocateStack(Size, Alignment);

  if (FirstIdx < TILEGX_AREG_NUM)
    State.addLoc(CCValAssign::getReg(ValNo, ValVT, TileIntRegs[FirstIdx], LocVT,
                                     LocInfo));
  else
    State.addLoc(CCValAssign::getMem(ValNo, ValVT, Offset, LocVT, LocInfo));

  return true;
}

#include "TileGenCallingConv.inc"

TileTargetLowering::TileTargetLowering(const TargetMachine &TM,
                                       const TileSubtarget &STI)
    : TargetLowering(TM, STI), Subtarget(STI) {
  setBooleanContents(ZeroOrOneBooleanContent);
  setBooleanVectorContents(ZeroOrOneBooleanContent);

  addRegisterClass(MVT::i64, &Tile::CPURegsRegClass);
  addRegisterClass(MVT::i32, &Tile::CPU32RegsRegClass);

  setStackPointerRegisterToSaveRestore(Tile::SP);

  // i1 loads always widen to a full register.
  setLoadExtAction(ISD::EXTLOAD, MVT::i64, MVT::i1, Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i64, MVT::i1, Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i64, MVT::i1, Promote);

  // Population count on 32-bit values runs through the 64-bit pcnt.
  setOperationAction(ISD::CTPOP, MVT::i32, Promote);

  // Comparisons produce a 64-bit boolean; the 32-bit form is promoted so the
  // high bits are well defined when a setcc feeds a 64-bit consumer.
  setOperationAction(ISD::SETCC, MVT::i32, Promote);

  // The condition of a branch and the result of a select stay as Tile nodes
  // and are matched by the cmoveqz/cmovnez and beqz/bnez patterns; expanding
  // BR_CC and SELECT_CC reduces them onto setcc + these primitives.
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  setOperationAction(ISD::BR_CC, MVT::i64, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i64, Expand);
  setOperationAction(ISD::SELECT, MVT::i64, Custom);
  setOperationAction(ISD::BRCOND, MVT::Other, Custom);

  // TILE-Gx has no single 64-bit multiply; it is expanded to a multiply-add
  // sequence during selection, so keep the node off the legalizer's libcall
  // path by marking it Custom.
  setOperationAction(ISD::MUL, MVT::i64, Custom);
  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::SREM, MVT::i32, Expand);
  setOperationAction(ISD::SDIV, MVT::i64, Expand);
  setOperationAction(ISD::SREM, MVT::i64, Expand);
  setOperationAction(ISD::UDIV, MVT::i32, Expand);
  setOperationAction(ISD::UREM, MVT::i32, Expand);
  setOperationAction(ISD::UDIV, MVT::i64, Expand);
  setOperationAction(ISD::UREM, MVT::i64, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i64, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i64, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i64, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i64, Expand);

  // Only left rotate is provided; right rotate becomes a negated left rotate.
  setOperationAction(ISD::ROTR, MVT::i32, Expand);
  setOperationAction(ISD::ROTR, MVT::i64, Expand);
  setOperationAction(ISD::ROTL, MVT::i32, Expand);

  // Sub-word sign extension is realized with a shift pair.
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i32, Expand);

  // Addresses are materialized with the moveli/shl16insli halfword sequence.
  setOperationAction(ISD::GlobalAddress, MVT::i64, Custom);

  setTargetDAGCombine(ISD::SELECT);
  setTargetDAGCombine(ISD::ZERO_EXTEND);

  setMinFunctionAlignment(Align(8));

  computeRegisterProperties(Subtarget.getRegisterInfo());
}

const char *TileTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case TileISD::TLSGD:
    return "TileISD::TLSGD";
  case TileISD::LDTLS:
    return "TileISD::LDTLS";
  case TileISD::TLSRELAXADD0:
    return "TileISD::TLSRELAXADD0";
  case TileISD::TLSRELAXADD1:
    return "TileISD::TLSRELAXADD1";
  case TileISD::MOVE:
    return "TileISD::MOVE";
  case TileISD::SAR16:
    return "TileISD::SAR16";
  case TileISD::JmpLink:
    return "TileISD::JmpLink";
  case TileISD::Ret:
    return "TileISD::Ret";
  case TileISD::MF:
    return "TileISD::MF";
  case TileISD::BFINS:
    return "TileISD::BFINS";
  case TileISD::BFEXTU:
    return "TileISD::BFEXTU";
  case TileISD::BRINDJT:
    return "TileISD::BRINDJT";
  case TileISD::VAARG_SP:
    return "TileISD::VAARG_SP";
  case TileISD::ALLOCA_SP:
    return "TileISD::ALLOCA_SP";
  case TileISD::ALLOCA_ADDR:
    return "TileISD::ALLOCA_ADDR";
  default:
    return nullptr;
  }
}

EVT TileTargetLowering::getSetCCResultType(const DataLayout &, LLVMContext &,
                                           EVT VT) const {
  if (!VT.isVector())
    return MVT::i32;
  return VT.changeVectorElementTypeToInteger();
}

unsigned TileTargetLowering::getJumpTableEncoding() const {
  return MachineJumpTableInfo::EK_Inline;
}

bool TileTargetLowering::isOffsetFoldingLegal(
    const GlobalAddressSDNode *GA) const {
  return false;
}

bool TileTargetLowering::isShuffleMaskLegal(ArrayRef<int> M, EVT VT) const {
  return false;
}

SDValue TileTargetLowering::lowerGlobalAddress(SDValue Op,
                                               SelectionDAG &DAG) const {
  const GlobalAddressSDNode *GA = cast<GlobalAddressSDNode>(Op);
  SDLoc DL(Op);
  const GlobalValue *GV = GA->getGlobal();
  int64_t Offset = GA->getOffset();

  if (getTargetMachine().getRelocationModel() == Reloc::PIC_)
    report_fatal_error("Tile: PIC global address lowering not yet supported");

  // Absolute 64-bit address: moveli hw2_last, shl16insli hw1, shl16insli hw0.
  SDValue High =
      DAG.getTargetGlobalAddress(GV, DL, MVT::i64, Offset, TileII::MO_HW2_LAST);
  SDValue Middle =
      DAG.getTargetGlobalAddress(GV, DL, MVT::i64, Offset, TileII::MO_HW1);
  SDValue Low =
      DAG.getTargetGlobalAddress(GV, DL, MVT::i64, Offset, TileII::MO_HW0);

  SDValue Addr = DAG.getNode(TileISD::MOVE, DL, MVT::i64, High);
  Addr = DAG.getNode(TileISD::SAR16, DL, MVT::i64, Addr, Middle);
  return DAG.getNode(TileISD::SAR16, DL, MVT::i64, Addr, Low);
}

SDValue TileTargetLowering::LowerOperation(SDValue Op,
                                           SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::BRCOND:
  case ISD::SELECT:
    // Kept as-is and matched by the beqz/bnez and cmoveqz/cmovnez patterns.
    return Op;
  case ISD::MUL:
    // Expanded into a multiply-add sequence during DAG-to-DAG selection.
    return Op;
  case ISD::GlobalAddress:
    return lowerGlobalAddress(Op, DAG);
  }
  return SDValue();
}

static SDValue performSELECTCombine(SDNode *N, SelectionDAG &DAG,
                                    TargetLowering::DAGCombinerInfo &DCI) {
  if (DCI.isBeforeLegalizeOps())
    return SDValue();

  SDValue SetCC = N->getOperand(0);
  if (SetCC.getOpcode() != ISD::SETCC ||
      !SetCC.getOperand(0).getValueType().isInteger())
    return SDValue();

  SDValue False = N->getOperand(2);
  EVT FalseTy = False.getValueType();
  if (!FalseTy.isInteger())
    return SDValue();

  // Fold "select (setcc x), t, 0" into "select (setcc.inverse x), 0, t" so the
  // zero arm lands on the cmovnez false operand a cmoveqz expects.
  ConstantSDNode *CN = dyn_cast<ConstantSDNode>(False);
  if (!CN || CN->getZExtValue())
    return SDValue();

  const SDLoc DL(N);
  ISD::CondCode CC = cast<CondCodeSDNode>(SetCC.getOperand(2))->get();
  SDValue True = N->getOperand(1);

  SetCC = DAG.getSetCC(DL, SetCC.getValueType(), SetCC.getOperand(0),
                       SetCC.getOperand(1), ISD::getSetCCInverse(CC, MVT::i64));

  return DAG.getNode(ISD::SELECT, DL, FalseTy, SetCC, False, True);
}

static SDValue performZEXTCombine(SDNode *N, SelectionDAG &DAG) {
  // A setcc already writes a zero-extended 64-bit value, so a zext of it is
  // redundant: rebuild the setcc directly at i64.
  SDValue SetCC = N->getOperand(0);
  if (SetCC.getOpcode() != ISD::SETCC || N->getValueType(0) != MVT::i64)
    return SDValue();

  return DAG.getNode(ISD::SETCC, SDLoc(SetCC), MVT::i64, SetCC.getOperand(0),
                     SetCC.getOperand(1), SetCC.getOperand(2));
}

SDValue TileTargetLowering::PerformDAGCombine(SDNode *N,
                                              DAGCombinerInfo &DCI) const {
  switch (N->getOpcode()) {
  default:
    break;
  case ISD::SELECT:
    return performSELECTCombine(N, DCI.DAG, DCI);
  case ISD::ZERO_EXTEND:
    return performZEXTCombine(N, DCI.DAG);
  }
  return SDValue();
}

MachineBasicBlock *
TileTargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                MachineBasicBlock *BB) const {
  llvm_unreachable("Unexpected instr type to insert");
}

// Add the specified physical register as a live-in value and create a
// corresponding virtual register for it.
static unsigned AddLiveIn(MachineFunction &MF, unsigned PReg,
                          const TargetRegisterClass *RC) {
  assert(RC->contains(PReg) && "Not the correct regclass!");
  unsigned VReg = MF.getRegInfo().createVirtualRegister(RC);
  MF.getRegInfo().addLiveIn(PReg, VReg);
  return VReg;
}

SDValue TileTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  if (IsVarArg)
    report_fatal_error("Tile: varargs not yet supported");

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Tile);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    EVT ValVT = VA.getValVT();
    ISD::ArgFlagsTy Flags = Ins[i].Flags;

    if (Flags.isByVal())
      report_fatal_error("Tile: byval arguments not yet supported");

    if (VA.isRegLoc()) {
      EVT RegVT = VA.getLocVT();
      unsigned ArgReg = VA.getLocReg();
      const TargetRegisterClass *RC;

      if (RegVT == MVT::i32 || RegVT == MVT::f32)
        RC = &Tile::CPU32RegsRegClass;
      else
        RC = &Tile::CPURegsRegClass;

      unsigned Reg = AddLiveIn(MF, ArgReg, RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, Reg, RegVT);

      switch (VA.getLocInfo()) {
      default:
        llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full:
        break;
      case CCValAssign::BCvt:
        ArgValue = DAG.getNode(ISD::BITCAST, dl, ValVT, ArgValue);
        break;
      case CCValAssign::SExt:
        ArgValue = DAG.getNode(ISD::AssertSext, dl, RegVT, ArgValue,
                               DAG.getValueType(ValVT));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, ValVT, ArgValue);
        break;
      case CCValAssign::ZExt:
        ArgValue = DAG.getNode(ISD::AssertZext, dl, RegVT, ArgValue,
                               DAG.getValueType(ValVT));
        ArgValue = DAG.getNode(ISD::TRUNCATE, dl, ValVT, ArgValue);
        break;
      }

      InVals.push_back(ArgValue);
    } else {
      assert(VA.isMemLoc());
      int FI = MFI.CreateFixedObject(ValVT.getSizeInBits() / 8,
                                     VA.getLocMemOffset(), true);
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
      InVals.push_back(DAG.getLoad(ValVT, dl, Chain, FIN,
                                   MachinePointerInfo::getFixedStack(MF, FI)));
    }
  }

  return Chain;
}

SDValue TileTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                      SmallVectorImpl<SDValue> &InVals) const {
  report_fatal_error("Tile: calls not yet supported");
}

SDValue TileTargetLowering::LowerCallResult(
    SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &dl,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  report_fatal_error("Tile: call results not yet supported");
}

SDValue
TileTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                const SDLoc &dl, SelectionDAG &DAG) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_Tile);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);
  RetOps.push_back(SDValue());

  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");
    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(), OutVals[i], Flag);
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  // Return on Tile is always a "jr $lr".
  RetOps[0] = Chain;
  RetOps[1] = DAG.getRegister(Tile::LR, MVT::i64);

  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(TileISD::Ret, dl, MVT::Other, RetOps);
}

//===----------------------------------------------------------------------===//
//                           Tile Inline Assembly Support
//===----------------------------------------------------------------------===//

TileTargetLowering::ConstraintType
TileTargetLowering::getConstraintType(StringRef Constraint) const {
  // 'd' : An address register. 'y' : Equivalent to r (compatibility).
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    default:
      break;
    case 'd':
    case 'y':
      return C_RegisterClass;
    }
  }
  return TargetLowering::getConstraintType(Constraint);
}

TargetLowering::ConstraintWeight
TileTargetLowering::getSingleConstraintMatchWeight(
    AsmOperandInfo &info, const char *constraint) const {
  ConstraintWeight Weight = CW_Invalid;
  Value *CallOperandVal = info.CallOperandVal;
  if (!CallOperandVal)
    return CW_Default;
  Type *type = CallOperandVal->getType();
  switch (*constraint) {
  default:
    Weight = TargetLowering::getSingleConstraintMatchWeight(info, constraint);
    break;
  case 'd':
  case 'y':
    if (type->isIntegerTy())
      Weight = CW_Register;
    break;
  }
  return Weight;
}

std::pair<unsigned, const TargetRegisterClass *>
TileTargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                 StringRef Constraint,
                                                 MVT VT) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 'd':
    case 'y':
    case 'r':
      return std::make_pair(0U, &Tile::CPURegsRegClass);
    }
  }
  return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}
