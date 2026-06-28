# RUN: llvm-mc -triple=tilegx -filetype=obj %s | llvm-readobj -r - | FileCheck %s

# Each relocation-specifier operator records the matching R_TILEGX_* relocation,
# identical to what the codegen path emits.

# CHECK: R_TILEGX_IMM16_X1_HW2_LAST g
moveli r0, hw2_last(g)
# CHECK: R_TILEGX_IMM16_X1_HW1 g
shl16insli r0, r0, hw1(g)
# CHECK: R_TILEGX_IMM16_X1_HW0 g
shl16insli r0, r0, hw0(g)
# CHECK: R_TILEGX_IMM16_X1_HW1_LAST_GOT g
moveli r1, hw1_last_got(g)
# CHECK: R_TILEGX_IMM16_X1_HW0_GOT g
shl16insli r1, r1, hw0_got(g)
# CHECK: R_TILEGX_IMM16_X1_HW1_LAST_TLS_GD tv
moveli r2, hw1_last_tls_gd(tv)
# CHECK: R_TILEGX_IMM16_X1_HW0_TLS_GD tv
shl16insli r2, r2, hw0_tls_gd(tv)
# CHECK: R_TILEGX_IMM16_X1_HW1_LAST_TLS_IE tv
moveli r3, hw1_last_tls_ie(tv)
# CHECK: R_TILEGX_IMM16_X1_HW0_TLS_IE tv
shl16insli r3, r3, hw0_tls_ie(tv)
# CHECK: R_TILEGX_IMM16_X1_HW1_LAST_TLS_LE tv
moveli r4, hw1_last_tls_le(tv)
# CHECK: R_TILEGX_IMM16_X1_HW0_TLS_LE tv
shl16insli r4, r4, hw0_tls_le(tv)
# CHECK: R_TILEGX_IMM8_X1_TLS_ADD tv
addi r0, r51, tls_add(tv)
# CHECK: R_TILEGX_IMM8_X1_TLS_GD_ADD tv
addi r0, r0, tls_gd_add(tv)
# CHECK: R_TILEGX_JUMPOFF_X1_PLT ext
jal plt(ext)
# CHECK: R_TILEGX_TLS_GD_CALL tv
jal tls_gd_call(tv)
# CHECK: R_TILEGX_TLS_IE_LOAD tv
ld_tls r3, r3, tls_ie_load(tv)
