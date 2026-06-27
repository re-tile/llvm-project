; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; Immediate materialization: small immediates inline, wider constants through
; the moveli / shl16insli halfword chain.

; CHECK-LABEL: small:
; CHECK: moveli r0, 1000
define i64 @small() {
  ret i64 1000
}

; CHECK-LABEL: word32:
; CHECK: moveli r0, 4660
; CHECK: shl16insli r0, r0, 22136
define i64 @word32() {
  ret i64 305419896
}

; CHECK-LABEL: full64:
; CHECK: moveli r0,
; CHECK: shl16insli r0, r0,
; CHECK: shl16insli r0, r0,
; CHECK: shl16insli r0, r0,
define i64 @full64() {
  ret i64 81985529216486895
}
