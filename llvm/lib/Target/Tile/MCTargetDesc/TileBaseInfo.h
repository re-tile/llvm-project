//===-- TileBaseInfo.h - Top level definitions for TILE MC ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the Tile target useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef TILEBASEINFO_H
#define TILEBASEINFO_H

#include "TileFixupKinds.h"
#include "TileMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

// This namespace holds all of the target specific flags.
namespace TileII {
// Tile Specific MachineOperand flags.
enum TOF {

  MO_NO_FLAG,
  MO_NO_FLAG_PIC,

  /// plt(Symbol) relocation
  MO_PLT_CALL,

  MO_HW0,
  MO_HW0_GOT,
  MO_HW0_PIC,
  MO_HW0_LAST,
  MO_HW1,
  MO_HW1_LAST,
  MO_HW1_LAST_PIC,
  MO_HW1_LAST_GOT,
  MO_HW2_LAST,

  // TLS
  MO_TLS_ADD,
  MO_TLS_GD_ADD,
  MO_TLS_GD_CALL,
  MO_TLS_IE_LOAD,
  MO_HW0_TLS_GD,
  MO_HW0_TLS_IE,
  MO_HW0_TLS_LE,
  MO_HW1_LAST_TLS_GD,
  MO_HW1_LAST_TLS_IE,
  MO_HW1_LAST_TLS_LE
};

// Tile instruction encoding formats.
enum {
  FrmRRR = 0,

  FrmImm8 = 1,

  FrmMTImm14 = 2,

  FrmMFImm14 = 3,

  FrmImm16 = 4,

  FrmUnary = 5,

  FrmShift = 6,

  FrmBr = 7,

  FrmJmp = 8,

  FrmMM = 9,

  FrmLS = 10,

  FrmPseudo = 11
};

// Tile instruction issue type.
enum TileIssueType {
  IT_None = 0,
  IT_X0 = 1,
  IT_X1 = 2,
  IT_X0X1 = 3,
  IT_X0Y0 = 4,
  IT_X1Y1 = 5,
  IT_X1Y2 = 6,
  IT_X0X1Y0Y1 = 7,
  IT_Num = 8
};

enum {
  IssueTypePos = 0,
  IssueTypeMask = 0xF,

  SoloPos = 4,
  SoloMask = 0x1,

  FormatTypePos = 5,
  FormatTypeMask = 0xF,

  // 1 = the instruction is encoded for the X0 pipe (low slot, X1 slot is FNOP);
  // 0 = encoded for the X1 pipe (high slot, X0 slot is FNOP). Set by the X0
  // instruction format classes in TileInstrFormats.td.
  EncPipePos = 9,
  EncPipeMask = 0x1,

  // 1 = the instruction is emitted in the Y2 load/store bundle mode and cannot
  // be slot-merged with another instruction.
  Y2BundlePos = 10,
  Y2BundleMask = 0x1
};

// FNOP fill for the empty issue slot of a single-instruction bundle. A standard
// X-format instruction bakes one of these into its complementary slot, so two
// such instructions merge into one bundle by ORing their real slots. Loads and
// stores are emitted in the Y2 bundle mode (Inst{63-62} != 0) with a different
// layout, so they are NOT mergeable this way.
static constexpr uint64_t FnopX0 = 0x51483000ULL;       // fills bits 30-0
static constexpr uint64_t FnopX1 = 0x50d46000ULL << 31; // fills bits 61-31

// The instruction is encoded in a standard X-format bundle (one real slot, the
// other FNOP) and so can be merged with a complementary-slot instruction.
inline bool isBundleMergeable(uint64_t TSFlags) {
  return !((TSFlags >> Y2BundlePos) & Y2BundleMask);
}

// Tile instruction issue slot.
// This describe the final slot inst issued to.
enum TileSlotType {
  ST_None = 0,
  ST_X0 = 1,
  ST_X1 = 2,
  ST_Y0 = 3,
  ST_Y1 = 4,
  ST_Y2 = 5,
  ST_Solo = 6
};
}

inline static std::pair<const MCSymbolRefExpr *, int64_t>
TileGetSymAndOffset(const MCFixup &Fixup) {
  MCFixupKind FixupKind = Fixup.getKind();

  if ((FixupKind < FirstTargetFixupKind) ||
      (FixupKind >= MCFixupKind(Tile::LastTargetFixupKind)))
    return std::make_pair((const MCSymbolRefExpr *)0, (int64_t) 0);

  const MCExpr *Expr = Fixup.getValue();
  MCExpr::ExprKind Kind = Expr->getKind();

  if (Kind == MCExpr::Binary) {
    const MCBinaryExpr *BE = static_cast<const MCBinaryExpr *>(Expr);
    const MCExpr *LHS = BE->getLHS();
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());

    if ((LHS->getKind() != MCExpr::SymbolRef) || !CE)
      return std::make_pair((const MCSymbolRefExpr *)0, (int64_t) 0);

    return std::make_pair(cast<MCSymbolRefExpr>(LHS), CE->getValue());
  }

  if (Kind != MCExpr::SymbolRef)
    return std::make_pair((const MCSymbolRefExpr *)0, (int64_t) 0);

  return std::make_pair(cast<MCSymbolRefExpr>(Expr), 0);
}
}

#endif
