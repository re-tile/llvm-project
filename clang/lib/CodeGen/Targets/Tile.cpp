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

RValue TileABIInfo::EmitVAArg(CodeGenFunction &CGF, Address VAListAddr,
                              QualType Ty, AggValueSlot Slot) const {
  CharUnits SlotSize = CharUnits::fromQuantity(8);
  auto TInfo = getContext().getTypeInfoInChars(Ty);
  TInfo.Width = std::max(TInfo.Width, CharUnits::fromQuantity(1));

  // Aggregates larger than the ten argument registers are passed indirectly.
  bool IsIndirect =
      isAggregateTypeForABI(Ty) &&
      getContext().getTypeSize(Ty) > NumArgGPRs * RegSizeBits;
  return emitVoidPtrVAArg(CGF, VAListAddr, Ty, IsIndirect, TInfo, SlotSize,
                          /*AllowHigherAlign=*/true, Slot);
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
