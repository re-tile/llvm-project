; RUN: llc -mtriple=tilegx < %s | FileCheck %s

declare i64 @g(i64)

; A function with calls and callee-saved spills gets a full FDE: lr at the
; entry CFA, the frame-allocation CFA offset, and each callee-saved slot.
; CHECK-LABEL: f:
; CHECK: .cfi_startproc
; CHECK-DAG: .cfi_offset 55, 0
; CHECK-DAG: .cfi_def_cfa_offset 32
; CHECK-DAG: .cfi_offset 31, -8
; CHECK-DAG: .cfi_offset 30, -16
; CHECK: .cfi_endproc
define i64 @f(i64 %a, i64 %b) {
  %x = call i64 @g(i64 %a)
  %y = call i64 @g(i64 %b)
  %z = add i64 %x, %y
  ret i64 %z
}

; A frame-pointer function switches the CFA to the frame pointer (r52). The
; caller's r52 is spilled before r52 takes the new frame address.
; CHECK-LABEL: dynframe:
; CHECK: .cfi_def_cfa_offset 48
; CHECK: st r10, r52
; CHECK: add r52, sp, zero
; CHECK: .cfi_def_cfa_register 52
declare void @use(ptr)
define i64 @dynframe(i64 %n) {
  %p = alloca i64, i64 %n
  call void @use(ptr %p)
  %v = load i64, ptr %p
  ret i64 %v
}

; A leaf needs no FDE rows.
; CHECK-LABEL: leaf:
; CHECK-NOT: .cfi_offset
; CHECK-NOT: .cfi_def_cfa_offset
define i64 @leaf(i64 %a) {
  %r = mul i64 %a, 3
  %s = add i64 %r, 1
  ret i64 %s
}
