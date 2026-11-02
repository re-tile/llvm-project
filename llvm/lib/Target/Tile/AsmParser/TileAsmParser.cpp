//===-- TileAsmParser.cpp - Parse Tile assembly to MCInst -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The textual TILE-Gx assembly parser, driven by the tile-as opcode table in
// TileOpcodeTable.inc. Each statement is checked against every pipe its
// mnemonic issues in. A { ... } bundle of one or two instructions goes to the
// X0/X1 pipes, and one holding a Y2 load or store, or three instructions, to
// Y0/Y1/Y2. Unused X0, X1, Y0 and Y1 slots get an fnop.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TileBaseInfo.h"
#include "MCTargetDesc/TileMCTargetDesc.h"
#include "MCTargetDesc/TileOpcodeTable.h"
#include "TargetInfo/TileTargetInfo.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MathExtras.h"
#include <memory>

using namespace llvm;
using namespace llvm::TileISA;

namespace {
// Every statement pushes one token so the generic parser calls
// matchAndEmitInstruction, which is where bundles are collected.
class TileOperand : public MCParsedAsmOperand {
  StringRef Token;
  SMLoc Loc;

public:
  TileOperand(StringRef Token, SMLoc Loc) : Token(Token), Loc(Loc) {}

  bool isToken() const override { return true; }
  bool isReg() const override { return false; }
  bool isImm() const override { return false; }
  bool isMem() const override { return false; }
  MCRegister getReg() const override { llvm_unreachable("not a register"); }
  SMLoc getStartLoc() const override { return Loc; }
  SMLoc getEndLoc() const override { return Loc; }
  void print(raw_ostream &OS, const MCAsmInfo &) const override {
    OS << "Token:" << Token;
  }
};

struct ParsedOperand {
  enum { Register, Value } Kind;
  unsigned Reg;
  const MCExpr *Expr;
  SMLoc Loc;
};

struct ParsedInst {
  unsigned Opcode;
  SMLoc Loc;
  SmallVector<ParsedOperand, 4> Operands;
};

class TileAsmParser : public MCTargetAsmParser {
  MCAsmParser &Parser;
  const MCRegisterInfo &MRI;

  bool InBundle = false;
  ParsedInst Current;
  SmallVector<ParsedInst, 3> Bundle;

  bool parseRegister(MCRegister &Reg, SMLoc &StartLoc, SMLoc &EndLoc) override;
  ParseStatus tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                               SMLoc &EndLoc) override;
  bool parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  bool matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;
  // tile-as modes that only relax its own checks.
  ParseStatus parseDirective(AsmToken DirectiveID) override {
    StringRef Name = DirectiveID.getString();
    if (Name == ".no_require_canonical_reg_names" ||
        Name == ".require_canonical_reg_names" ||
        Name == ".allow_suspicious_bundles")
      return ParseStatus::Success;
    return ParseStatus::NoMatch;
  }
  void convertToMapAndConstraints(unsigned Kind,
                                  const OperandVector &Operands) override {
    llvm_unreachable("TILE-Gx has no MS-style inline assembly");
  }
  bool tokenIsStartOfStatement(AsmToken::TokenKind Token) override {
    return Token == AsmToken::LCurly || Token == AsmToken::RCurly;
  }

  std::optional<unsigned> registerNumber(StringRef Name) const;
  bool parseOperand(ParsedOperand &Op);
  bool fits(ParsedInst &Inst, Pipe P, bool Report);
  bool emitBundle(MCStreamer &Out, SMLoc Loc);

public:
  TileAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
                const MCInstrInfo &MII)
      : MCTargetAsmParser(STI, MII), Parser(Parser),
        MRI(*Parser.getContext().getRegisterInfo()) {}
};
} // namespace

std::optional<unsigned> TileAsmParser::registerNumber(StringRef Name) const {
  std::string Lower = Name.lower();
  unsigned N;
  if (Lower.size() > 1 && Lower[0] == 'r' &&
      !StringRef(Lower).substr(1).getAsInteger(10, N) && N < 64)
    return N;
  return StringSwitch<std::optional<unsigned>>(Lower)
      .Case("fp", 52)
      .Case("tp", 53)
      .Case("sp", 54)
      .Case("lr", 55)
      .Case("sn", 56)
      .Case("idn0", 57)
      .Case("idn1", 58)
      .Case("udn0", 59)
      .Case("udn1", 60)
      .Case("udn2", 61)
      .Case("udn3", 62)
      .Case("zero", 63)
      .Default(std::nullopt);
}

bool TileAsmParser::parseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                  SMLoc &EndLoc) {
  if (!tryParseRegister(Reg, StartLoc, EndLoc).isSuccess())
    return Error(StartLoc, "invalid register name");
  return false;
}

// CFI directives name registers through here. Encoding 56 (sn) has no
// MCRegister and cannot appear in CFI.
ParseStatus TileAsmParser::tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                            SMLoc &EndLoc) {
  const AsmToken &Tok = Parser.getTok();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();
  if (!Tok.is(AsmToken::Identifier))
    return ParseStatus::NoMatch;
  std::optional<unsigned> N = registerNumber(Tok.getString());
  if (!N)
    return ParseStatus::NoMatch;
  for (MCRegister R : MRI.getRegClass(Tile::CPURegsRegClassID))
    if (MRI.getEncodingValue(R) == *N) {
      Reg = R;
      Parser.Lex();
      return ParseStatus::Success;
    }
  return ParseStatus::NoMatch;
}

static uint16_t operatorSpecifier(StringRef Name) {
  return StringSwitch<uint16_t>(Name)
      .Case("hw0", TileII::MO_HW0)
      .Case("hw1", TileII::MO_HW1)
      .Case("hw2", TileII::MO_HW2)
      .Case("hw3", TileII::MO_HW3)
      .Case("hw0_last", TileII::MO_HW0_LAST)
      .Case("hw1_last", TileII::MO_HW1_LAST)
      .Case("hw2_last", TileII::MO_HW2_LAST)
      .Case("hw0_got", TileII::MO_HW0_GOT)
      .Case("hw0_last_got", TileII::MO_HW0_LAST_GOT)
      .Case("hw1_last_got", TileII::MO_HW1_LAST_GOT)
      .Case("plt", TileII::MO_PLT_CALL)
      .Case("hw0_plt", TileII::MO_HW0_PLT)
      .Case("hw1_plt", TileII::MO_HW1_PLT)
      .Case("hw1_last_plt", TileII::MO_HW1_LAST_PLT)
      .Case("hw2_last_plt", TileII::MO_HW2_LAST_PLT)
      .Case("hw0_tls_gd", TileII::MO_HW0_TLS_GD)
      .Case("hw0_last_tls_gd", TileII::MO_HW0_LAST_TLS_GD)
      .Case("hw1_last_tls_gd", TileII::MO_HW1_LAST_TLS_GD)
      .Case("hw0_tls_ie", TileII::MO_HW0_TLS_IE)
      .Case("hw0_last_tls_ie", TileII::MO_HW0_LAST_TLS_IE)
      .Case("hw1_last_tls_ie", TileII::MO_HW1_LAST_TLS_IE)
      .Case("hw0_tls_le", TileII::MO_HW0_TLS_LE)
      .Case("hw0_last_tls_le", TileII::MO_HW0_LAST_TLS_LE)
      .Case("hw1_last_tls_le", TileII::MO_HW1_LAST_TLS_LE)
      .Case("tls_gd_call", TileII::MO_TLS_GD_CALL)
      .Case("tls_gd_add", TileII::MO_TLS_GD_ADD)
      .Case("tls_ie_load", TileII::MO_TLS_IE_LOAD)
      .Case("tls_add", TileII::MO_TLS_ADD)
      .Default(0);
}

bool TileAsmParser::parseOperand(ParsedOperand &Op) {
  const AsmToken &Tok = Parser.getTok();
  Op.Loc = Tok.getLoc();

  if (Tok.is(AsmToken::Identifier)) {
    if (std::optional<unsigned> N = registerNumber(Tok.getString())) {
      Op.Kind = ParsedOperand::Register;
      Op.Reg = *N;
      Parser.Lex();
      return false;
    }

    if (uint16_t Spec = operatorSpecifier(Tok.getString().lower());
        Spec && getLexer().peekTok().is(AsmToken::LParen)) {
      Parser.Lex();
      Parser.Lex();
      const MCExpr *Sub;
      SMLoc End;
      if (getParser().parseParenExpression(Sub, End))
        return true;
      Op.Kind = ParsedOperand::Value;
      Op.Expr = MCSpecifierExpr::create(Sub, Spec, getContext());
      return false;
    }
  }

  Op.Kind = ParsedOperand::Value;
  return getParser().parseExpression(Op.Expr);
}

bool TileAsmParser::parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                     SMLoc NameLoc, OperandVector &Operands) {
  if (Name == "{" || Name == "}") {
    if ((Name == "{") == InBundle)
      return Error(NameLoc, InBundle ? "nested '{'" : "'}' without '{'");
    InBundle = Name == "{";
    return false;
  }

  std::optional<unsigned> Opcode = findOpcode(Name.lower());
  if (!Opcode) {
    Parser.eatToEndOfStatement();
    return Error(NameLoc, "invalid instruction mnemonic");
  }

  Current = ParsedInst{*Opcode, NameLoc, {}};
  auto atEnd = [&] {
    return getLexer().is(AsmToken::EndOfStatement) ||
           getLexer().is(AsmToken::RCurly);
  };
  if (!atEnd()) {
    do {
      ParsedOperand Op;
      if (parseOperand(Op)) {
        Parser.eatToEndOfStatement();
        return true;
      }
      Current.Operands.push_back(Op);
    } while (parseOptionalToken(AsmToken::Comma));
  }
  if (!atEnd()) {
    SMLoc Loc = getLexer().getLoc();
    Parser.eatToEndOfStatement();
    return Error(Loc, "unexpected token in argument list");
  }
  if (Current.Operands.size() != opcode(*Opcode).NumOperands)
    return Error(NameLoc, Twine("'") + opcode(*Opcode).Name + "' takes " +
                              Twine(opcode(*Opcode).NumOperands) +
                              " operands");

  // A closing '}' must start the next statement to reach parseInstruction.
  if (getLexer().is(AsmToken::EndOfStatement))
    Parser.Lex();
  Operands.push_back(std::make_unique<TileOperand>(Name, NameLoc));
  return false;
}

// Whether every operand of Inst can be encoded in pipe P. With Report set, the
// first mismatch is reported against its operand.
bool TileAsmParser::fits(ParsedInst &Inst, Pipe P, bool Report) {
  const Opcode &Op = opcode(Inst.Opcode);
  if (!Op.hasPipe(P))
    return false;
  for (auto [I, Parsed] : enumerate(Inst.Operands)) {
    const Operand &Info = operand(Op, P, I);
    if (Info.Kind == OperandKind::Spr && Parsed.Kind == ParsedOperand::Value)
      if (const auto *Ref = dyn_cast<MCSymbolRefExpr>(Parsed.Expr))
        if (std::optional<unsigned> N = findSpr(Ref->getSymbol().getName()))
          Parsed.Expr = MCConstantExpr::create(*N, getContext());
    bool WantsReg = Info.Kind == OperandKind::Register;
    if (WantsReg != (Parsed.Kind == ParsedOperand::Register)) {
      if (Report)
        Error(Parsed.Loc, WantsReg ? "expected a register" : "expected a value");
      return false;
    }
    if (WantsReg)
      continue;

    int64_t Value;
    if (!Parsed.Expr->evaluateAsAbsolute(Value))
      continue;
    if (Info.Kind == OperandKind::Address) {
      if (Report)
        Error(Parsed.Loc, "branch target must be a symbol");
      return false;
    }
    bool InRange = Info.Signed ? isIntN(Info.NumBits, Value)
                               : isUIntN(Info.NumBits, Value);
    if (!InRange) {
      if (Report)
        Error(Parsed.Loc, "operand out of range");
      return false;
    }
  }
  return true;
}

bool TileAsmParser::emitBundle(MCStreamer &Out, SMLoc Loc) {
  if (Bundle.empty())
    return false;
  if (Bundle.size() > 3) {
    Bundle.clear();
    return Error(Loc, "a TILE-Gx bundle holds at most three instructions");
  }
  if (Bundle.size() > 1) {
    auto isFiller = [](const ParsedInst &Inst) {
      StringRef Name = opcode(Inst.Opcode).Name;
      return Name == "nop" || Name == "info" || Name == "infol";
    };
    auto Lone = find_if(Bundle, [](const ParsedInst &Inst) {
      return !opcode(Inst.Opcode).CanBundle;
    });
    if (Lone != Bundle.end() && !all_of(Bundle, [&](const ParsedInst &Inst) {
          return &Inst == &*Lone || isFiller(Inst);
        })) {
      SMLoc At = Lone->Loc;
      Bundle.clear();
      return Error(At, "instruction may not be bundled with others");
    }
  }

  // tile-as's template order. A lone instruction is paired with an fnop, so
  // a load or store prefers Y2 and anything else X1.
  static const SmallVector<SmallVector<Pipe, 3>> One = {
      {PipeY2}, {PipeX1}, {PipeX0}};
  static const SmallVector<SmallVector<Pipe, 3>> Two = {
      {PipeY0, PipeY2}, {PipeY1, PipeY2}, {PipeY2, PipeY0},
      {PipeY2, PipeY1}, {PipeX0, PipeX1}, {PipeX1, PipeX0}};
  static const SmallVector<SmallVector<Pipe, 3>> Three = {
      {PipeY0, PipeY1, PipeY2}, {PipeY0, PipeY2, PipeY1},
      {PipeY1, PipeY0, PipeY2}, {PipeY1, PipeY2, PipeY0},
      {PipeY2, PipeY0, PipeY1}, {PipeY2, PipeY1, PipeY0}};
  const auto &Candidates =
      Bundle.size() == 1 ? One : Bundle.size() == 2 ? Two : Three;

  const SmallVector<Pipe, 3> *Chosen = nullptr;
  for (const auto &Pipes : Candidates)
    if (all_of(enumerate(Bundle), [&](auto Entry) {
          return fits(Entry.value(), Pipes[Entry.index()], false);
        })) {
      Chosen = &Pipes;
      break;
    }

  if (!Chosen) {
    if (Bundle.size() == 1) {
      const Opcode &Op = opcode(Bundle[0].Opcode);
      Pipe P = Op.hasPipe(PipeX1) ? PipeX1 : PipeX0;
      fits(Bundle[0], P, true);
    } else {
      Error(Loc, "no TILE-Gx pipe assignment encodes this bundle");
    }
    Bundle.clear();
    return true;
  }

  MCInst Packet;
  Packet.setOpcode(Tile::BUNDLE);
  Packet.setLoc(Bundle[0].Loc);
  for (auto [Inst, P] : zip(Bundle, *Chosen)) {
    MCInst *Sub = getContext().createMCInst();
    Sub->setOpcode(Tile::TABLE_INST);
    Sub->setLoc(Inst.Loc);
    Sub->addOperand(MCOperand::createImm(Inst.Opcode));
    Sub->addOperand(MCOperand::createImm(P));
    for (const ParsedOperand &Op : Inst.Operands) {
      int64_t Value;
      if (Op.Kind == ParsedOperand::Register)
        Sub->addOperand(MCOperand::createImm(Op.Reg));
      else if (Op.Expr->evaluateAsAbsolute(Value))
        Sub->addOperand(MCOperand::createImm(Value));
      else
        Sub->addOperand(MCOperand::createExpr(Op.Expr));
    }
    Packet.addOperand(MCOperand::createInst(Sub));
  }
  Bundle.clear();
  Out.emitInstruction(Packet, getSTI());
  return false;
}

bool TileAsmParser::matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                            OperandVector &Operands,
                                            MCStreamer &Out,
                                            uint64_t &ErrorInfo,
                                            bool MatchingInlineAsm) {
  if (Operands.empty())
    return InBundle ? false : emitBundle(Out, IDLoc);
  Bundle.push_back(std::move(Current));
  return InBundle ? false : emitBundle(Out, IDLoc);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTileAsmParser() {
  RegisterMCAsmParser<TileAsmParser> X(getTheTileTarget());
}
