; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
; RUN: llc -mtriple=tilegx -filetype=obj < %s | llvm-readobj -r - | FileCheck %s --check-prefix=RELOC
;
; Function calls: argument registers r0-r9, the stack-arg overflow above the
; 16-byte reserve zone, the jal call sequence and the return-value copy-out.

declare i64 @ext(i64, i64, i64)

; CHECK-LABEL: c:
; A non-leaf saves lr into the caller reserve slot and chains the incoming sp.
; CHECK: st sp, lr
; CHECK: addi sp, sp, -{{[0-9]+}}
; CHECK: st r10, r11
; CHECK: jal ext
; CHECK: ld lr, sp
; CHECK: jr lr
define i64 @c(i64 %a, i64 %b, i64 %c) {
  %r = call i64 @ext(i64 %a, i64 %b, i64 %c)
  %t = add i64 %r, %a
  ret i64 %t
}

; The eleventh argument lands on the stack at sp+24 (8-byte slot + 16 reserve).
; CHECK-LABEL: passmany:
; CHECK: addi r1, sp, 24
; CHECK: movei r2, 11
; CHECK: st r1, r2
; CHECK: jal many
declare i64 @many(i64,i64,i64,i64,i64,i64,i64,i64,i64,i64,i64,i64)
define i64 @passmany(i64 %a) {
  %r = call i64 @many(i64 %a, i64 1, i64 2, i64 3, i64 4, i64 5, i64 6, i64 7, i64 8, i64 9, i64 10, i64 11)
  ret i64 %r
}

; A direct call to a global emits the X1 jump-offset relocation; PLT/GOT are PIC.
; RELOC: R_TILEGX_JUMPOFF_X1 ext
; RELOC: R_TILEGX_JUMPOFF_X1 many
