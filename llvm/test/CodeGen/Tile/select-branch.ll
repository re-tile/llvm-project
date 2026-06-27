; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; setcc, select and conditional branch lowering.

; CHECK-LABEL: seteq:
; CHECK: cmpeq r0, r0, r1
define i64 @seteq(i64 %a, i64 %b) {
  %c = icmp eq i64 %a, %b
  %r = zext i1 %c to i64
  ret i64 %r
}

; CHECK-LABEL: setlt:
; CHECK: cmplts r0, r0, r1
define i64 @setlt(i64 %a, i64 %b) {
  %c = icmp slt i64 %a, %b
  %r = zext i1 %c to i64
  ret i64 %r
}

; CHECK-LABEL: sel:
; CHECK: cmplts
; CHECK: cmovnez
define i64 @sel(i64 %x, i64 %y, i64 %a, i64 %b) {
  %c = icmp slt i64 %x, %y
  %r = select i1 %c, i64 %a, i64 %b
  ret i64 %r
}

; CHECK-LABEL: breq:
; CHECK: beqz
define i64 @breq(i64 %a) {
  %c = icmp eq i64 %a, 0
  br i1 %c, label %x, label %y
x:
  ret i64 1
y:
  ret i64 2
}
