; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
; RUN: llc -mtriple=tilegx -filetype=obj < %s | llvm-readobj -r - | FileCheck %s --check-prefix=RELOC
;
; Absolute 64-bit global addresses use the moveli/shl16insli halfword sequence
; and record the matching hw0/hw1/hw2_last relocations.

@g = external global i64

; CHECK-LABEL: ld:
; CHECK: moveli r0, hw2_last(g)
; CHECK: shl16insli r0, r0, hw1(g)
; CHECK: shl16insli r0, r0, hw0(g)
; CHECK: ld r0, r0

; RELOC: R_TILEGX_IMM16_X1_HW2_LAST g
; RELOC: R_TILEGX_IMM16_X1_HW1 g
; RELOC: R_TILEGX_IMM16_X1_HW0 g
define i64 @ld() {
  %v = load i64, ptr @g
  ret i64 %v
}
