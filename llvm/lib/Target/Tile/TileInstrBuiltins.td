//===- TileInstrBuiltins.td - TILE-Gx GCC builtin selection -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Selection for the GCC __insn_* builtins that have no codegen instruction of
// their own. Each issues through the tile-as opcode table. The immediate forms
// fall back to the register form when the operand is not a fitting constant,
// as GCC expands them through the generic patterns.
//
//===----------------------------------------------------------------------===//

def tileImmU6 : PatLeaf<(imm), [{ return isUInt<6>(N->getZExtValue()); }]>;

def TBL_ADD : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_add CPURegs:$a0, CPURegs:$a1))]>;
def TBL_ADDI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_addi CPURegs:$a0, (i64 immSExt8:$a1)))]>;
def TBL_ADDLI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_addli CPURegs:$a0, (i64 immSExt16:$a1)))]>;
def TBL_ADDXI : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, i32imm:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_addxi CPU32Regs:$a0, (i32 immSExt8:$a1)))]>;
def TBL_ADDXLI : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, i32imm:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_addxli CPU32Regs:$a0, (i32 immSExt16:$a1)))]>;
def TBL_ADDXSC : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_addxsc CPU32Regs:$a0, CPU32Regs:$a1))]>;
def TBL_AND : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_and CPURegs:$a0, CPURegs:$a1))]>;
def TBL_ANDI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_andi CPURegs:$a0, (i64 immSExt8:$a1)))]>;
def TBL_BFEXTS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1, i64imm:$a2),
    [(set CPURegs:$rd, (int_tilegx_bfexts CPURegs:$a0, (i64 imm:$a1), (i64 imm:$a2)))]>;
def TBL_BFEXTU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1, i64imm:$a2),
    [(set CPURegs:$rd, (int_tilegx_bfextu CPURegs:$a0, (i64 imm:$a1), (i64 imm:$a2)))]>;
let Constraints = "$a0 = $rd" in
def TBL_BFINS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, i64imm:$a2, i64imm:$a3),
    [(set CPURegs:$rd, (int_tilegx_bfins CPURegs:$a0, CPURegs:$a1, (i64 imm:$a2), (i64 imm:$a3)))]>;
let Constraints = "$a0 = $rd" in
def TBL_CMOVEQZ : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_cmoveqz CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_CMOVNEZ : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_cmovnez CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
def TBL_CMPEQ : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmpeq CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMPEQI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmpeqi CPURegs:$a0, (i64 immSExt8:$a1)))]>;
def TBL_CMPLES : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmples CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMPLEU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmpleu CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMPLTS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmplts CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMPLTSI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmpltsi CPURegs:$a0, (i64 immSExt8:$a1)))]>;
def TBL_CMPLTU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmpltu CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMPLTUI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmpltui CPURegs:$a0, (i64 immSExt8:$a1)))]>;
def TBL_CMPNE : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmpne CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMUL : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmul CPURegs:$a0, CPURegs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_CMULA : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_cmula CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_CMULAF : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_cmulaf CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
def TBL_CMULF : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmulf CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMULFR : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmulfr CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMULH : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmulh CPURegs:$a0, CPURegs:$a1))]>;
def TBL_CMULHR : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_cmulhr CPURegs:$a0, CPURegs:$a1))]>;
let hasSideEffects = 1 in
def TBL_DTLBPR : TileTableOp<(outs), (ins CPURegs:$a0),
    [(int_tilegx_dtlbpr CPURegs:$a0)]>;
def TBL_FDOUBLE_ADD_FLAGS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fdouble_add_flags CPURegs:$a0, CPURegs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_FDOUBLE_ADDSUB : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_fdouble_addsub CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
def TBL_FDOUBLE_MUL_FLAGS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fdouble_mul_flags CPURegs:$a0, CPURegs:$a1))]>;
def TBL_FDOUBLE_PACK1 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fdouble_pack1 CPURegs:$a0, CPURegs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_FDOUBLE_PACK2 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_fdouble_pack2 CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
def TBL_FDOUBLE_SUB_FLAGS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fdouble_sub_flags CPURegs:$a0, CPURegs:$a1))]>;
def TBL_FDOUBLE_UNPACK_MAX : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fdouble_unpack_max CPURegs:$a0, CPURegs:$a1))]>;
def TBL_FDOUBLE_UNPACK_MIN : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fdouble_unpack_min CPURegs:$a0, CPURegs:$a1))]>;
let hasSideEffects = 1 in
def TBL_FNOP : TileTableOp<(outs), (ins ),
    [(int_tilegx_fnop)]>;
def TBL_FSINGLE_ADD1 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fsingle_add1 CPURegs:$a0, CPURegs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_FSINGLE_ADDSUB2 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_fsingle_addsub2 CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
def TBL_FSINGLE_MUL1 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fsingle_mul1 CPURegs:$a0, CPURegs:$a1))]>;
def TBL_FSINGLE_MUL2 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fsingle_mul2 CPURegs:$a0, CPURegs:$a1))]>;
def TBL_FSINGLE_PACK1 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_fsingle_pack1 CPURegs:$a0))]>;
def TBL_FSINGLE_PACK2 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fsingle_pack2 CPURegs:$a0, CPURegs:$a1))]>;
def TBL_FSINGLE_SUB1 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_fsingle_sub1 CPURegs:$a0, CPURegs:$a1))]>;
let hasSideEffects = 1 in
def TBL_ILL : TileTableOp<(outs), (ins ),
    [(int_tilegx_ill)]>;
let hasSideEffects = 1 in
def TBL_INFO : TileTableOp<(outs), (ins i64imm:$a0),
    [(int_tilegx_info (i64 imm:$a0))]>;
let hasSideEffects = 1 in
def TBL_INFOL : TileTableOp<(outs), (ins i64imm:$a0),
    [(int_tilegx_infol (i64 imm:$a0))]>;
let mayLoad = 1 in
def TBL_LD : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ld CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LD1S : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ld1s CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LD1U : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ld1u CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LD2S : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ld2s CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LD2U : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ld2u CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LD4S : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ld4s CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LD4U : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ld4u CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LDNA : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ldna CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LDNT : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ldnt CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LDNT1S : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ldnt1s CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LDNT1U : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ldnt1u CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LDNT2S : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ldnt2s CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LDNT2U : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ldnt2u CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LDNT4S : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ldnt4s CPURegs:$a0))]>;
let mayLoad = 1 in
def TBL_LDNT4U : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0),
    [(set CPURegs:$rd, (int_tilegx_ldnt4u CPURegs:$a0))]>;
def TBL_LNK : TileTableOp<(outs CPURegs:$rd), (ins ),
    [(set CPURegs:$rd, (int_tilegx_lnk))]>;
def TBL_MNZ : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mnz CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_HS_HS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_hs_hs CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_HS_HU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_hs_hu CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_HS_LS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_hs_ls CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_HS_LU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_hs_lu CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_HU_HU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_hu_hu CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_HU_LS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_hu_ls CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_HU_LU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_hu_lu CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_LS_LS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_ls_ls CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_LS_LU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_ls_lu CPURegs:$a0, CPURegs:$a1))]>;
def TBL_MUL_LU_LU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mul_lu_lu CPURegs:$a0, CPURegs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_HS_HS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_hs_hs CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_HS_HU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_hs_hu CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_HS_LS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_hs_ls CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_HS_LU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_hs_lu CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_HU_HU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_hu_hu CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_HU_LS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_hu_ls CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_HU_LU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_hu_lu CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_LS_LS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_ls_ls CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_LS_LU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_ls_lu CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULA_LU_LU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1, CPURegs:$a2),
    [(set CPURegs:$rd, (int_tilegx_mula_lu_lu CPURegs:$a0, CPURegs:$a1, CPURegs:$a2))]>;
let Constraints = "$a0 = $rd" in
def TBL_MULAX : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1, CPU32Regs:$a2),
    [(set CPU32Regs:$rd, (int_tilegx_mulax CPU32Regs:$a0, CPU32Regs:$a1, CPU32Regs:$a2))]>;
def TBL_MULX : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_mulx CPU32Regs:$a0, CPU32Regs:$a1))]>;
def TBL_MZ : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_mz CPURegs:$a0, CPURegs:$a1))]>;
let hasSideEffects = 1 in
def TBL_NOP : TileTableOp<(outs), (ins ),
    [(int_tilegx_nop)]>;
def TBL_NOR : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_nor CPURegs:$a0, CPURegs:$a1))]>;
def TBL_OR : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_or CPURegs:$a0, CPURegs:$a1))]>;
def TBL_ORI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_ori CPURegs:$a0, (i64 immSExt8:$a1)))]>;
def TBL_ROTL : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPU32Regs:$a1),
    [(set CPURegs:$rd, (int_tilegx_rotl CPURegs:$a0, CPU32Regs:$a1))]>;
def TBL_ROTLI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i32imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_rotli CPURegs:$a0, (i32 tileImmU6:$a1)))]>;
def TBL_SHL16INSLI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_shl16insli CPURegs:$a0, (i64 imm:$a1)))]>;
def TBL_SHL1ADD : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_shl1add CPURegs:$a0, CPURegs:$a1))]>;
def TBL_SHL1ADDX : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_shl1addx CPU32Regs:$a0, CPU32Regs:$a1))]>;
def TBL_SHL2ADD : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_shl2add CPURegs:$a0, CPURegs:$a1))]>;
def TBL_SHL2ADDX : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_shl2addx CPU32Regs:$a0, CPU32Regs:$a1))]>;
def TBL_SHL3ADD : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_shl3add CPURegs:$a0, CPURegs:$a1))]>;
def TBL_SHL3ADDX : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_shl3addx CPU32Regs:$a0, CPU32Regs:$a1))]>;
def TBL_SHLI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i32imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_shli CPURegs:$a0, (i32 tileImmU6:$a1)))]>;
def TBL_SHLX : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_shlx CPU32Regs:$a0, CPU32Regs:$a1))]>;
def TBL_SHLXI : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, i32imm:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_shlxi CPU32Regs:$a0, (i32 tileImmU6:$a1)))]>;
def TBL_SHRS : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPU32Regs:$a1),
    [(set CPURegs:$rd, (int_tilegx_shrs CPURegs:$a0, CPU32Regs:$a1))]>;
def TBL_SHRSI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i32imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_shrsi CPURegs:$a0, (i32 tileImmU6:$a1)))]>;
def TBL_SHRU : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPU32Regs:$a1),
    [(set CPURegs:$rd, (int_tilegx_shru CPURegs:$a0, CPU32Regs:$a1))]>;
def TBL_SHRUI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i32imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_shrui CPURegs:$a0, (i32 tileImmU6:$a1)))]>;
def TBL_SHRUX : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_shrux CPU32Regs:$a0, CPU32Regs:$a1))]>;
def TBL_SHRUXI : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, i32imm:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_shruxi CPU32Regs:$a0, (i32 tileImmU6:$a1)))]>;
def SHUFFLEBYTES1 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_shufflebytes1 CPURegs:$a0, CPURegs:$a1))]>;
let mayStore = 1 in
def TBL_ST : TileTableOp<(outs), (ins CPURegs:$a0, CPURegs:$a1),
    [(int_tilegx_st CPURegs:$a0, CPURegs:$a1)]>;
let mayStore = 1 in
def TBL_ST1 : TileTableOp<(outs), (ins CPURegs:$a0, CPURegs:$a1),
    [(int_tilegx_st1 CPURegs:$a0, CPURegs:$a1)]>;
let mayStore = 1 in
def TBL_ST2 : TileTableOp<(outs), (ins CPURegs:$a0, CPURegs:$a1),
    [(int_tilegx_st2 CPURegs:$a0, CPURegs:$a1)]>;
let mayStore = 1 in
def TBL_ST4 : TileTableOp<(outs), (ins CPURegs:$a0, CPURegs:$a1),
    [(int_tilegx_st4 CPURegs:$a0, CPURegs:$a1)]>;
let mayStore = 1 in
def TBL_STNT : TileTableOp<(outs), (ins CPURegs:$a0, CPURegs:$a1),
    [(int_tilegx_stnt CPURegs:$a0, CPURegs:$a1)]>;
let mayStore = 1 in
def TBL_STNT1 : TileTableOp<(outs), (ins CPURegs:$a0, CPURegs:$a1),
    [(int_tilegx_stnt1 CPURegs:$a0, CPURegs:$a1)]>;
let mayStore = 1 in
def TBL_STNT2 : TileTableOp<(outs), (ins CPURegs:$a0, CPURegs:$a1),
    [(int_tilegx_stnt2 CPURegs:$a0, CPURegs:$a1)]>;
let mayStore = 1 in
def TBL_STNT4 : TileTableOp<(outs), (ins CPURegs:$a0, CPURegs:$a1),
    [(int_tilegx_stnt4 CPURegs:$a0, CPURegs:$a1)]>;
def TBL_SUB : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_sub CPURegs:$a0, CPURegs:$a1))]>;
def TBL_SUBX : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_subx CPU32Regs:$a0, CPU32Regs:$a1))]>;
def TBL_SUBXSC : TileTableOp<(outs CPU32Regs:$rd), (ins CPU32Regs:$a0, CPU32Regs:$a1),
    [(set CPU32Regs:$rd, (int_tilegx_subxsc CPU32Regs:$a0, CPU32Regs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_TBLIDXB0 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_tblidxb0 CPURegs:$a0, CPURegs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_TBLIDXB1 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_tblidxb1 CPURegs:$a0, CPURegs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_TBLIDXB2 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_tblidxb2 CPURegs:$a0, CPURegs:$a1))]>;
let Constraints = "$a0 = $rd" in
def TBL_TBLIDXB3 : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_tblidxb3 CPURegs:$a0, CPURegs:$a1))]>;
def TBL_XOR : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPURegs:$a1),
    [(set CPURegs:$rd, (int_tilegx_xor CPURegs:$a0, CPURegs:$a1))]>;
def TBL_XORI : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, i64imm:$a1),
    [(set CPURegs:$rd, (int_tilegx_xori CPURegs:$a0, (i64 immSExt8:$a1)))]>;
def TBL_SHL : TileTableOp<(outs CPURegs:$rd), (ins CPURegs:$a0, CPU32Regs:$a1),
    []>;

def : Pat<(int_tilegx_addi CPURegs:$a0, CPURegs:$a1), (TBL_ADD CPURegs:$a0, CPURegs:$a1)>;
def : Pat<(int_tilegx_addli CPURegs:$a0, CPURegs:$a1), (TBL_ADD CPURegs:$a0, CPURegs:$a1)>;
def : Pat<(int_tilegx_addxi CPU32Regs:$a0, CPU32Regs:$a1), (ADDX CPU32Regs:$a0, CPU32Regs:$a1)>;
def : Pat<(int_tilegx_addxli CPU32Regs:$a0, CPU32Regs:$a1), (ADDX CPU32Regs:$a0, CPU32Regs:$a1)>;
def : Pat<(int_tilegx_andi CPURegs:$a0, CPURegs:$a1), (TBL_AND CPURegs:$a0, CPURegs:$a1)>;
def : Pat<(int_tilegx_cmpeqi CPURegs:$a0, CPURegs:$a1), (TBL_CMPEQ CPURegs:$a0, CPURegs:$a1)>;
def : Pat<(int_tilegx_cmpltsi CPURegs:$a0, CPURegs:$a1), (TBL_CMPLTS CPURegs:$a0, CPURegs:$a1)>;
def : Pat<(int_tilegx_cmpltui CPURegs:$a0, CPURegs:$a1), (TBL_CMPLTU CPURegs:$a0, CPURegs:$a1)>;
def : Pat<(int_tilegx_ld_l2 CPURegs:$a0), (TBL_LD CPURegs:$a0)>;
def : Pat<(int_tilegx_ld1s_l2 CPURegs:$a0), (TBL_LD1S CPURegs:$a0)>;
def : Pat<(int_tilegx_ld1u_l2 CPURegs:$a0), (TBL_LD1U CPURegs:$a0)>;
def : Pat<(int_tilegx_ld2s_l2 CPURegs:$a0), (TBL_LD2S CPURegs:$a0)>;
def : Pat<(int_tilegx_ld2u_l2 CPURegs:$a0), (TBL_LD2U CPURegs:$a0)>;
def : Pat<(int_tilegx_ld4s_l2 CPURegs:$a0), (TBL_LD4S CPURegs:$a0)>;
def : Pat<(int_tilegx_ld4u_l2 CPURegs:$a0), (TBL_LD4U CPURegs:$a0)>;
def : Pat<(int_tilegx_ldna_l2 CPURegs:$a0), (TBL_LDNA CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt_l2 CPURegs:$a0), (TBL_LDNT CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt1s_l2 CPURegs:$a0), (TBL_LDNT1S CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt1u_l2 CPURegs:$a0), (TBL_LDNT1U CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt2s_l2 CPURegs:$a0), (TBL_LDNT2S CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt2u_l2 CPURegs:$a0), (TBL_LDNT2U CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt4s_l2 CPURegs:$a0), (TBL_LDNT4S CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt4u_l2 CPURegs:$a0), (TBL_LDNT4U CPURegs:$a0)>;
def : Pat<(int_tilegx_ld_miss CPURegs:$a0), (TBL_LD CPURegs:$a0)>;
def : Pat<(int_tilegx_ld1s_miss CPURegs:$a0), (TBL_LD1S CPURegs:$a0)>;
def : Pat<(int_tilegx_ld1u_miss CPURegs:$a0), (TBL_LD1U CPURegs:$a0)>;
def : Pat<(int_tilegx_ld2s_miss CPURegs:$a0), (TBL_LD2S CPURegs:$a0)>;
def : Pat<(int_tilegx_ld2u_miss CPURegs:$a0), (TBL_LD2U CPURegs:$a0)>;
def : Pat<(int_tilegx_ld4s_miss CPURegs:$a0), (TBL_LD4S CPURegs:$a0)>;
def : Pat<(int_tilegx_ld4u_miss CPURegs:$a0), (TBL_LD4U CPURegs:$a0)>;
def : Pat<(int_tilegx_ldna_miss CPURegs:$a0), (TBL_LDNA CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt_miss CPURegs:$a0), (TBL_LDNT CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt1s_miss CPURegs:$a0), (TBL_LDNT1S CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt1u_miss CPURegs:$a0), (TBL_LDNT1U CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt2s_miss CPURegs:$a0), (TBL_LDNT2S CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt2u_miss CPURegs:$a0), (TBL_LDNT2U CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt4s_miss CPURegs:$a0), (TBL_LDNT4S CPURegs:$a0)>;
def : Pat<(int_tilegx_ldnt4u_miss CPURegs:$a0), (TBL_LDNT4U CPURegs:$a0)>;
def : Pat<(int_tilegx_move CPURegs:$a), (i64 CPURegs:$a)>;
def : Pat<(int_tilegx_movei CPURegs:$a), (i64 CPURegs:$a)>;
def : Pat<(int_tilegx_moveli CPURegs:$a), (i64 CPURegs:$a)>;
def : Pat<(int_tilegx_ori CPURegs:$a0, CPURegs:$a1), (TBL_OR CPURegs:$a0, CPURegs:$a1)>;
def : Pat<(int_tilegx_rotli CPURegs:$a0, CPU32Regs:$a1), (TBL_ROTL CPURegs:$a0, CPU32Regs:$a1)>;
def : Pat<(int_tilegx_shli CPURegs:$a0, CPU32Regs:$a1), (TBL_SHL CPURegs:$a0, CPU32Regs:$a1)>;
def : Pat<(int_tilegx_shlxi CPU32Regs:$a0, CPU32Regs:$a1), (TBL_SHLX CPU32Regs:$a0, CPU32Regs:$a1)>;
def : Pat<(int_tilegx_shrsi CPURegs:$a0, CPU32Regs:$a1), (TBL_SHRS CPURegs:$a0, CPU32Regs:$a1)>;
def : Pat<(int_tilegx_shrui CPURegs:$a0, CPU32Regs:$a1), (TBL_SHRU CPURegs:$a0, CPU32Regs:$a1)>;
def : Pat<(int_tilegx_shruxi CPU32Regs:$a0, CPU32Regs:$a1), (TBL_SHRUX CPU32Regs:$a0, CPU32Regs:$a1)>;
def : Pat<(int_tilegx_xori CPURegs:$a0, CPURegs:$a1), (TBL_XOR CPURegs:$a0, CPURegs:$a1)>;
