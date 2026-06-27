; RUN: llc -mtriple=tilegx < %s | FileCheck %s

declare i64 @ext(i64)
declare i64 @ext2(i64, i64)

; A direct sibling call jumps with j (no lr save, no frame) and leaves the
; return value in place for the callee to return to our caller.
; CHECK-LABEL: tailext:
; CHECK-NOT: st sp, lr
; CHECK-NOT: jal
; CHECK: j ext
; CHECK-NOT: jr lr
define i64 @tailext(i64 %a) {
  %x = add i64 %a, 1
  %r = tail call i64 @ext(i64 %x)
  ret i64 %r
}

; CHECK-LABEL: tail2:
; CHECK: j ext2
define i64 @tail2(i64 %a, i64 %b) {
  %r = tail call i64 @ext2(i64 %b, i64 %a)
  ret i64 %r
}

; A call whose result is used afterwards is not in tail position: a normal jal
; with a real frame and an epilogue return.
; CHECK-LABEL: notail:
; CHECK: st sp, lr
; CHECK: jal ext
; CHECK: jr lr
define i64 @notail(i64 %a) {
  %r = call i64 @ext(i64 %a)
  %s = add i64 %r, 7
  ret i64 %s
}
