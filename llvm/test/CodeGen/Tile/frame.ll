; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; Frame lowering: a leaf with no frame touches neither sp nor lr, while a
; dynamic alloca forces a frame pointer and a runtime stack adjustment.

; CHECK-LABEL: leaf:
; CHECK-NOT: st sp, lr
; CHECK-NOT: addi sp, sp
; CHECK: jr lr
define i64 @leaf(i64 %a, i64 %b) {
  %s = add i64 %a, %b
  ret i64 %s
}

; CHECK-LABEL: dyn:
; CHECK: st sp, lr
; CHECK: add r52, sp, zero
; CHECK: jal sink
; CHECK: ld lr, sp
; CHECK: jr lr
declare void @sink(ptr)
define void @dyn(i64 %n) {
  %p = alloca i64, i64 %n
  call void @sink(ptr %p)
  ret void
}
