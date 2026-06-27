; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; Loads and stores of every width, including the sign- and zero-extending
; load variants.

; CHECK-LABEL: ld64:
; CHECK: ld r0, r0
define i64 @ld64(ptr %p) {
  %r = load i64, ptr %p
  ret i64 %r
}

; CHECK-LABEL: ld32s:
; CHECK: ld4s r0, r0
define i64 @ld32s(ptr %p) {
  %t = load i32, ptr %p
  %r = sext i32 %t to i64
  ret i64 %r
}

; CHECK-LABEL: ld16u:
; CHECK: ld2u r0, r0
define i64 @ld16u(ptr %p) {
  %t = load i16, ptr %p
  %r = zext i16 %t to i64
  ret i64 %r
}

; CHECK-LABEL: ld8s:
; CHECK: ld1s r0, r0
define i64 @ld8s(ptr %p) {
  %t = load i8, ptr %p
  %r = sext i8 %t to i64
  ret i64 %r
}

; CHECK-LABEL: st64:
; CHECK: st r0, r1
define void @st64(ptr %p, i64 %v) {
  store i64 %v, ptr %p
  ret void
}

; CHECK-LABEL: st16:
; CHECK: st2 r0, r1
define void @st16(ptr %p, i16 %v) {
  store i16 %v, ptr %p
  ret void
}
