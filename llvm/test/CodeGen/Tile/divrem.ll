; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; TILE-Gx has no integer divide; sdiv/udiv/srem/urem lower to the libgcc
; helpers through the call machinery.

; CHECK-LABEL: sdiv64:
; CHECK: jal __divdi3
define i64 @sdiv64(i64 %a, i64 %b) {
  %r = sdiv i64 %a, %b
  ret i64 %r
}

; CHECK-LABEL: udiv64:
; CHECK: jal __udivdi3
define i64 @udiv64(i64 %a, i64 %b) {
  %r = udiv i64 %a, %b
  ret i64 %r
}

; CHECK-LABEL: srem64:
; CHECK: jal __moddi3
define i64 @srem64(i64 %a, i64 %b) {
  %r = srem i64 %a, %b
  ret i64 %r
}

; CHECK-LABEL: urem64:
; CHECK: jal __umoddi3
define i64 @urem64(i64 %a, i64 %b) {
  %r = urem i64 %a, %b
  ret i64 %r
}
