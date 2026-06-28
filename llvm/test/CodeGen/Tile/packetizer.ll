; RUN: llc -mtriple=tilegx -O2 < %s | FileCheck %s --check-prefix=PACK
; RUN: llc -mtriple=tilegx -O2 -tile-no-packetizer < %s | FileCheck %s --check-prefix=NOPACK

; An X0-encoded op (the i32 multiply, mulx) and an independent X1-encoded op
; (the i64 add) pack into one bundle. With the packetizer disabled they emit as
; two single-instruction bundles.

; PACK:   { add {{.*}} ; mulx {{.*}} }
; NOPACK: mulx
; NOPACK: add
define i64 @pack_independent(i32 %a, i32 %b, i64 %c, i64 %d) {
  %m = mul i32 %a, %b
  %me = zext i32 %m to i64
  %s = add i64 %c, %d
  %r = add i64 %me, %s
  ret i64 %r
}

; A true data dependence (the multiply result feeds the add) must not share a
; bundle: the consumer has to read the producer's result, which a parallel-issue
; bundle cannot provide.

; PACK-NOT: { {{.*}}mulx{{.*}};{{.*}}addx
define i32 @dependent_no_pack(i32 %a, i32 %b) {
  %m = mul i32 %a, %b
  %r = add i32 %m, %b
  ret i32 %r
}
