//===- Tile.cpp -----------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ABIInfoImpl.h"
#include "TargetInfo.h"

using namespace clang;
using namespace clang::CodeGen;

//===----------------------------------------------------------------------===//
// TILE-Gx ABI Implementation.
//
// Based on the TILE-Gx ABI (UG513) and matched against GCC config/tilegx. Up to
// ten 8-byte argument doublewords are passed in r0-r9; the same registers carry
// return values. A scalar smaller than a register is promoted to a register;
// an aggregate is passed by value spread over consecutive argument words. An
// aggregate or return value too large for the ten registers is passed through
// an indirect pointer. Floating point shares the integer registers, so there is
// no separate FP-register coercion to perform.
//===----------------------------------------------------------------------===//

namespace {
class TileABIInfo : public ABIInfo {
  static const unsigned NumArgGPRs = 10;
  static const unsigned RegSizeBits = 64;

public:
  TileABIInfo(CodeGenTypes &CGT) : ABIInfo(CGT) {}

private:
  ABIArgInfo classifyType(QualType Ty, unsigned SizeLimitBits) const;
  void computeInfo(CGFunctionInfo &FI) const override;
  RValue EmitVAArg(CodeGenFunction &CGF, Address VAListAddr, QualType Ty,
                   AggValueSlot Slot) const override;
};
} // end anonymous namespace

ABIArgInfo TileABIInfo::classifyType(QualType Ty, unsigned SizeLimitBits) const {
  if (Ty->isVoidType())
    return ABIArgInfo::getIgnore();

  uint64_t Size = getContext().getTypeSize(Ty);

  if (!isAggregateTypeForABI(Ty)) {
    // Treat an enum type as its underlying type.
    if (const auto *ED = Ty->getAsEnumDecl())
      Ty = ED->getIntegerType();

    if (Size < RegSizeBits && Ty->isIntegralOrEnumerationType())
      return ABIArgInfo::getExtend(Ty);

    if (const auto *EIT = Ty->getAs<BitIntType>())
      if (EIT->getNumBits() < RegSizeBits)
        return ABIArgInfo::getExtend(Ty);

    return ABIArgInfo::getDirect();
  }

  // A C++ record with a non-trivial copy constructor or destructor is passed
  // through an indirect pointer.
  if (CGCXXABI::RecordArgABI RAA = getRecordArgABI(Ty, getCXXABI()))
    return getNaturalAlignIndirect(Ty, getDataLayout().getAllocaAddrSpace(),
                                   RAA == CGCXXABI::RAA_DirectInMemory);

  if (Size == 0)
    return ABIArgInfo::getIgnore();

  // Aggregates that do not fit in the argument registers are passed through an
  // indirect pointer.
  if (Size > SizeLimitBits)
    return getNaturalAlignIndirect(Ty, getDataLayout().getAllocaAddrSpace(),
                                   /*ByVal=*/false);

  // A small aggregate rides in registers as a sequence of doublewords.
  uint64_t Words = llvm::alignTo(Size, RegSizeBits) / RegSizeBits;
  llvm::Type *I64 = llvm::Type::getInt64Ty(getVMContext());
  llvm::Type *CoerceTy =
      Words == 1 ? I64 : llvm::ArrayType::get(I64, Words);
  return ABIArgInfo::getDirect(CoerceTy);
}

void TileABIInfo::computeInfo(CGFunctionInfo &FI) const {
  FI.getReturnInfo() =
      classifyType(FI.getReturnType(), NumArgGPRs * RegSizeBits);
  for (auto &Arg : FI.arguments())
    Arg.info = classifyType(Arg.type, NumArgGPRs * RegSizeBits);
}

// GCC's tilegx_gimplify_va_arg_expr. An argument that would straddle or start
// at __skip, the entry sp, is fetched from past the caller's 16-byte reserve.
// A type aligned beyond 8 bytes rounds __args up by adding its bit 3, as GCC
// does.
RValue TileABIInfo::EmitVAArg(CodeGenFunction &CGF, Address VAListAddr,
                              QualType Ty, AggValueSlot Slot) const {
  CGBuilderTy &Builder = CGF.Builder;
  bool IsIndirect =
      isAggregateTypeForABI(Ty) &&
      getContext().getTypeSize(Ty) > NumArgGPRs * RegSizeBits;
  auto TInfo = getContext().getTypeInfoInChars(Ty);
  CharUnits Size = IsIndirect ? CharUnits::fromQuantity(8) : TInfo.Width;
  CharUnits RSize = Size.alignTo(CharUnits::fromQuantity(8));
  if (RSize.isZero())
    RSize = CharUnits::fromQuantity(8);

  Address ArgsAddr = Builder.CreateStructGEP(VAListAddr, 0, "args_p");
  Address SkipAddr = Builder.CreateStructGEP(VAListAddr, 1, "skip_p");
  llvm::Value *Args = Builder.CreateLoad(ArgsAddr, "args");
  llvm::Value *Skip = Builder.CreateLoad(SkipAddr, "skip");

  if (!IsIndirect && TInfo.Align > CharUnits::fromQuantity(8)) {
    llvm::Value *Bit = Builder.CreateAnd(
        Builder.CreatePtrToInt(Args, CGF.IntPtrTy), 8, "align_bit");
    Args = Builder.CreateGEP(CGF.Int8Ty, Args, Bit, "args_aligned");
  }

  llvm::Value *End =
      Builder.CreateGEP(CGF.Int8Ty, Args, CGF.Builder.getInt64(RSize.getQuantity()));
  llvm::Value *Cross = Builder.CreateAnd(Builder.CreateICmpUGT(End, Skip),
                                         Builder.CreateICmpULE(Args, Skip));
  llvm::Value *PastReserve =
      Builder.CreateGEP(CGF.Int8Ty, Skip, CGF.Builder.getInt64(16));
  llvm::Value *ArgPtr = Builder.CreateSelect(Cross, PastReserve, Args, "argp");
  Builder.CreateStore(Builder.CreateGEP(CGF.Int8Ty, ArgPtr,
                                        CGF.Builder.getInt64(RSize.getQuantity())),
                      ArgsAddr);

  CharUnits SlotAlign = CharUnits::fromQuantity(8);
  Address Addr = IsIndirect
                     ? Address(Builder.CreateLoad(Address(ArgPtr, CGF.DefaultPtrTy,
                                                          SlotAlign)),
                               CGF.ConvertTypeForMem(Ty), TInfo.Align)
                     : Address(ArgPtr, CGF.ConvertTypeForMem(Ty),
                               std::min(TInfo.Align, SlotAlign));
  return CGF.EmitLoadOfAnyValue(CGF.MakeAddrLValue(Addr, Ty), Slot);
}

namespace {
class TileTargetCodeGenInfo : public TargetCodeGenInfo {
public:
  TileTargetCodeGenInfo(CodeGenTypes &CGT)
      : TargetCodeGenInfo(std::make_unique<TileABIInfo>(CGT)) {}

  int getDwarfEHStackPointer(CodeGen::CodeGenModule &M) const override {
    return 54;
  }
};
} // end anonymous namespace

std::unique_ptr<TargetCodeGenInfo>
CodeGen::createTileTargetCodeGenInfo(CodeGenModule &CGM) {
  return std::make_unique<TileTargetCodeGenInfo>(CGM.getTypes());
}
