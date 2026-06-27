; RUN: llc -mtriple=tilegx -relocation-model=pic -filetype=asm < %s | FileCheck %s
; RUN: llc -mtriple=tilegx -relocation-model=pic -filetype=obj < %s | llvm-readobj -r - | FileCheck %s --check-prefix=RELOC
;
; The three thread-local access models match the GCC tilegx sequences: general
; dynamic calls __tls_get_addr via tls_gd_call, initial exec loads the tpoff
; through the GOT with ld_tls, and local exec materializes the tpoff inline and
; adds the thread pointer.

@gd = external thread_local global i64
@ie = external thread_local(initialexec) global i64
@le = external thread_local(localexec) global i64

; CHECK-LABEL: get_gd:
; CHECK: moveli r{{[0-9]+}}, hw1_last_tls_gd(gd)
; CHECK: shl16insli r{{[0-9]+}}, r{{[0-9]+}}, hw0_tls_gd(gd)
; CHECK: addi r{{[0-9]+}}, r51, tls_add(gd)
; CHECK: jal tls_gd_call(gd)
; CHECK: addi r{{[0-9]+}}, r0, tls_gd_add(gd)
; RELOC: R_TILEGX_IMM16_X1_HW1_LAST_TLS_GD gd
; RELOC: R_TILEGX_IMM16_X1_HW0_TLS_GD gd
; RELOC: R_TILEGX_IMM8_X1_TLS_ADD gd
; RELOC: R_TILEGX_TLS_GD_CALL gd
; RELOC: R_TILEGX_IMM8_X1_TLS_GD_ADD gd
define i64 @get_gd() {
  %v = load i64, ptr @gd
  ret i64 %v
}

; CHECK-LABEL: get_ie:
; CHECK: moveli r{{[0-9]+}}, hw1_last_tls_ie(ie)
; CHECK: shl16insli r{{[0-9]+}}, r{{[0-9]+}}, hw0_tls_ie(ie)
; CHECK: addi r{{[0-9]+}}, r51, tls_add(ie)
; CHECK: ld_tls r{{[0-9]+}}, r{{[0-9]+}}, tls_ie_load(ie)
; CHECK: add r{{[0-9]+}}, r{{[0-9]+}}, tp
; RELOC: R_TILEGX_IMM16_X1_HW1_LAST_TLS_IE ie
; RELOC: R_TILEGX_IMM16_X1_HW0_TLS_IE ie
; RELOC: R_TILEGX_IMM8_X1_TLS_ADD ie
; RELOC: R_TILEGX_TLS_IE_LOAD ie
define i64 @get_ie() {
  %v = load i64, ptr @ie
  ret i64 %v
}

; CHECK-LABEL: get_le:
; CHECK: moveli r{{[0-9]+}}, hw1_last_tls_le(le)
; CHECK: shl16insli r{{[0-9]+}}, r{{[0-9]+}}, hw0_tls_le(le)
; CHECK: add r{{[0-9]+}}, r{{[0-9]+}}, tp
; RELOC: R_TILEGX_IMM16_X1_HW1_LAST_TLS_LE le
; RELOC: R_TILEGX_IMM16_X1_HW0_TLS_LE le
define i64 @get_le() {
  %v = load i64, ptr @le
  ret i64 %v
}
