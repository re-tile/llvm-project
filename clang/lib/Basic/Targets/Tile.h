//===--- Tile.h - Declare TILE-Gx target feature support --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the TILE-Gx TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_TILE_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_TILE_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY TileGXTargetInfo : public TargetInfo {
  static const char *const GCCRegNames[];
  static const TargetInfo::GCCRegAlias GCCRegAliases[];

public:
  TileGXTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    // TILE-Gx is a little-endian LP64 architecture.
    BigEndian = false;
    LongWidth = LongAlign = 64;
    PointerWidth = PointerAlign = 64;
    IntMaxType = SignedLong;
    Int64Type = SignedLong;
    IntPtrType = SignedLong;
    PtrDiffType = SignedLong;
    SizeType = UnsignedLong;
    WCharType = SignedInt;
    WIntType = UnsignedInt;
    LongDoubleWidth = LongDoubleAlign = 64;
    LongDoubleFormat = &llvm::APFloat::IEEEdouble();
    // 128-bit stack alignment and a 64-bit lock-free atomic word.
    SuitableAlign = 128;
    MaxAtomicPromoteWidth = MaxAtomicInlineWidth = 64;
    resetDataLayout("e-m:e-p:64:64-i8:8:32-i16:16:32-i64:64-n32:64-S128");
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  bool hasFeature(StringRef Feature) const override {
    return Feature == "tilegx";
  }

  llvm::SmallVector<Builtin::InfosShard> getTargetBuiltins() const override;

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  ArrayRef<const char *> getGCCRegNames() const override;
  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override;

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    switch (*Name) {
    default:
      return false;
    case 'I': // Signed 8-bit constant.
    case 'J': // Signed 16-bit constant.
    case 'K': // Unsigned 16-bit constant.
    case 'L': // Constant that fits in a signed byte when incremented.
    case 'M': // Bit mask for bfins.
    case 'N': // Byte replicated eight times.
    case 'O': // The constant zero.
    case 'P': // Sign-extended byte replicated as four shorts.
    case 'Q': // Same as 'L' but not -1.
    case 'S': // Run of consecutive low one-bits.
      return true;
    case 'U': // Non-auto-incrementing memory.
      Info.setAllowsMemory();
      return true;
    }
  }

  std::string_view getClobbers() const override { return ""; }

  bool hasBitIntType() const override { return true; }
};

} // namespace targets
} // namespace clang

#endif // LLVM_CLANG_LIB_BASIC_TARGETS_TILE_H
