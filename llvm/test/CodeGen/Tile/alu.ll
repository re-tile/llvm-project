; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; Scalar integer ALU selection: register ops, immediates, the synthesized
; 64-bit multiply, sub-word sign extension and the bit-count primitives.

; CHECK-LABEL: sub64:
; CHECK: sub r0, r0, r1
define i64 @sub64(i64 %a, i64 %b) {
  %r = sub i64 %a, %b
  ret i64 %r
}

; CHECK-LABEL: shli:
; CHECK: shli r0, r0, 5
define i64 @shli(i64 %a) {
  %r = shl i64 %a, 5
  ret i64 %r
}

; CHECK-LABEL: ashr:
; CHECK: shrs r0, r0, r1
define i64 @ashr(i64 %a, i64 %b) {
  %r = ashr i64 %a, %b
  ret i64 %r
}

; TILE-Gx has no single 64-bit multiply; it is a multiply-add chain.
; CHECK-LABEL: mul64:
; CHECK: mul_hu_lu
; CHECK: mula_hu_lu
; CHECK: shli {{.*}}, 32
; CHECK: mula_lu_lu
define i64 @mul64(i64 %a, i64 %b) {
  %r = mul i64 %a, %b
  ret i64 %r
}

; CHECK-LABEL: mul32:
; CHECK: mulx r0, r0, r1
define i32 @mul32(i32 %a, i32 %b) {
  %r = mul i32 %a, %b
  ret i32 %r
}

; CHECK-LABEL: sext8:
; CHECK: shli
; CHECK: shrsi
define i64 @sext8(i64 %a) {
  %t = trunc i64 %a to i8
  %r = sext i8 %t to i64
  ret i64 %r
}

; CHECK-LABEL: bswap64:
; CHECK: revbytes r0, r0
define i64 @bswap64(i64 %a) {
  %r = call i64 @llvm.bswap.i64(i64 %a)
  ret i64 %r
}

; CHECK-LABEL: clz64:
; CHECK: clz r0, r0
define i64 @clz64(i64 %a) {
  %r = call i64 @llvm.ctlz.i64(i64 %a, i1 true)
  ret i64 %r
}

; CHECK-LABEL: pcnt64:
; CHECK: pcnt r0, r0
define i64 @pcnt64(i64 %a) {
  %r = call i64 @llvm.ctpop.i64(i64 %a)
  ret i64 %r
}

declare i64 @llvm.bswap.i64(i64)
declare i64 @llvm.ctlz.i64(i64, i1)
declare i64 @llvm.ctpop.i64(i64)
