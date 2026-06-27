//===--- Tile.cpp - Implement TILE-Gx target feature support --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the TILE-Gx TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "Tile.h"
#include "clang/Basic/MacroBuilder.h"

using namespace clang;
using namespace clang::targets;

const char *const TileGXTargetInfo::GCCRegNames[] = {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "r16",  "r17",  "r18",  "r19",  "r20",  "r21",  "r22",  "r23",
    "r24",  "r25",  "r26",  "r27",  "r28",  "r29",  "r30",  "r31",
    "r32",  "r33",  "r34",  "r35",  "r36",  "r37",  "r38",  "r39",
    "r40",  "r41",  "r42",  "r43",  "r44",  "r45",  "r46",  "r47",
    "r48",  "r49",  "r50",  "r51",  "r52",  "tp",   "sp",   "lr",
    "r56",  "idn0", "idn1", "udn0", "udn1", "udn2", "udn3", "zero",
};

ArrayRef<const char *> TileGXTargetInfo::getGCCRegNames() const {
  return llvm::ArrayRef(GCCRegNames);
}

const TargetInfo::GCCRegAlias TileGXTargetInfo::GCCRegAliases[] = {
    {{"r52"}, "r52"},
    {{"r53"}, "tp"},
    {{"r54"}, "sp"},
    {{"r55"}, "lr"},
    {{"r63"}, "zero"},
};

ArrayRef<TargetInfo::GCCRegAlias> TileGXTargetInfo::getGCCRegAliases() const {
  return llvm::ArrayRef(GCCRegAliases);
}

void TileGXTargetInfo::getTargetDefines(const LangOptions &Opts,
                                        MacroBuilder &Builder) const {
  Builder.defineMacro("__tile__");
  Builder.defineMacro("__tilegx__");
  Builder.defineMacro("__TILEGX__");
  Builder.defineMacro("__tile_chip__", "10");
  Builder.defineMacro("__tile_chip_rev__", "0");

  Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1");
  Builder.defineMacro("__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2");
}
