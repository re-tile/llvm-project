; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; Divide and modulo by a constant lower to the magic-number reciprocal multiply,
; whose high half is built from the 32-bit partial products (mul_hu_*/mul_hs_*)
; with explicit carry. A variable divisor still goes through the libgcc helper.

; CHECK-LABEL: udiv_const:
; CHECK-NOT: jal __udivdi3
; CHECK: mul_hu_lu
; CHECK: mul_hu_hu
; CHECK: shrui
define i64 @udiv_const(i64 %x) {
  %r = udiv i64 %x, 10
  ret i64 %r
}

; CHECK-LABEL: urem_const:
; CHECK-NOT: jal __umoddi3
; CHECK: mul_hu_lu
define i64 @urem_const(i64 %x) {
  %r = urem i64 %x, 7
  ret i64 %r
}

; CHECK-LABEL: sdiv_const:
; CHECK-NOT: jal __divdi3
; CHECK: mul_hs_lu
; CHECK: mul_hs_hs
define i64 @sdiv_const(i64 %x) {
  %r = sdiv i64 %x, 3
  ret i64 %r
}

; CHECK-LABEL: srem_const:
; CHECK-NOT: jal __moddi3
; CHECK: mul_hs_lu
define i64 @srem_const(i64 %x) {
  %r = srem i64 %x, 7
  ret i64 %r
}

; A power of two is a shift, no multiply.
; CHECK-LABEL: udiv_pow2:
; CHECK: shrui
; CHECK-NOT: mul_
define i64 @udiv_pow2(i64 %x) {
  %r = udiv i64 %x, 8
  ret i64 %r
}

; i32 constant divide uses a single low-half multiply of the 32-bit operands.
; CHECK-LABEL: udiv_const32:
; CHECK-NOT: jal
; CHECK: mul_lu_lu
define i32 @udiv_const32(i32 %x) {
  %r = udiv i32 %x, 10
  ret i32 %r
}

; CHECK-LABEL: sdiv_const32:
; CHECK-NOT: jal
; CHECK: mul_ls_ls
define i32 @sdiv_const32(i32 %x) {
  %r = sdiv i32 %x, 7
  ret i32 %r
}

; A variable divisor still uses the runtime helper.
; CHECK-LABEL: udiv_var:
; CHECK: jal __udivdi3
define i64 @udiv_var(i64 %a, i64 %b) {
  %r = udiv i64 %a, %b
  ret i64 %r
}
