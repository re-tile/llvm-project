; RUN: llc -mtriple=tilegx -O2 < %s | FileCheck %s --check-prefix=PACK
; RUN: llc -mtriple=tilegx -O2 -tile-no-packetizer < %s | FileCheck %s --check-prefix=NOPACK

; An X0-encoded op (the i32 multiply, mulx) and an independent X1-encoded op
; (the i64 add) pack into one bundle. With the packetizer disabled they emit as
; two single-instruction bundles.

; PACK:   { mulx {{.*}} ; add {{.*}} }
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

; Two independent integer ALU ops both match the X1 encoding, so one is rewritten
; to its X0 twin to free the complementary slot. The first op in the bundle takes
; X0. Without the twins these would emit as two single-instruction bundles.

; PACK:   { add {{.*}} ; sub {{.*}} }
; NOPACK: sub
; NOPACK: add
define i64 @pack_two_alu(i64 %a, i64 %b, i64 %c, i64 %d) {
  %x = sub i64 %a, %b
  %y = add i64 %c, %d
  %z = add i64 %x, %y
  ret i64 %z
}
