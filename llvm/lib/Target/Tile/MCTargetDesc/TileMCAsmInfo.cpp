//===-- TileMCAsmInfo.cpp - Tile Asm Properties ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the TileMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "TileMCAsmInfo.h"
#include "TileBaseInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

StringRef Tile::getSpecifierName(uint16_t Spec) {
  switch (Spec) {
  case TileII::MO_HW0:
    return "hw0";
  case TileII::MO_HW1:
    return "hw1";
  case TileII::MO_HW1_LAST:
    return "hw1_last";
  case TileII::MO_HW2_LAST:
    return "hw2_last";
  case TileII::MO_HW0_GOT:
    return "hw0_got";
  case TileII::MO_HW1_LAST_GOT:
    return "hw1_last_got";
  default:
    return StringRef();
  }
}

void TileMCAsmInfo::printSpecifierExpr(raw_ostream &OS,
                                       const MCSpecifierExpr &Expr) const {
  StringRef S = Tile::getSpecifierName(Expr.getSpecifier());
  if (!S.empty())
    OS << S << '(';
  printExpr(OS, *Expr.getSubExpr());
  if (!S.empty())
    OS << ')';
}

void TileMCAsmInfo::anchor() {}

TileMCAsmInfo::TileMCAsmInfo(const Triple &TheTriple,
                             const MCTargetOptions &Options)
    : MCAsmInfoELF(Options) {
  IsLittleEndian = true;
  CodePointerSize = CalleeSaveStackSlotSize = 8;
  MinInstAlignment = 8;

  Data16bitsDirective = "\t.2byte\t";
  Data32bitsDirective = "\t.4byte\t";
  Data64bitsDirective = "\t.8byte\t";
  ZeroDirective = "\t.space\t";
  CommentString = "#";

  UsesELFSectionDirectiveForBSS = true;

  SupportsDebugInformation = true;
  ExceptionsType = ExceptionHandling::DwarfCFI;
  DwarfRegNumForCFI = true;
}
