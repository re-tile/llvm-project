; Verify that the multi-character tilegx inline-asm constraints bind to the
; right place. Stop after ISel because emitting inline asm to text or object
; needs an MCAsmParser, which the target does not have yet.
; RUN: llc -mtriple=tilegx -stop-after=finalize-isel < %s | FileCheck %s

; R05 names r5 for the output, R00 names r0 for the input.
; CHECK-LABEL: name: regs
; CHECK: INLINEASM {{.*}}implicit-def $r5
; CHECK: COPY $r5
define i64 @regs(i64 %x) {
  %r = call i64 asm "move $0, $1", "=@3R05,@3R00"(i64 %x)
  ret i64 %r
}

; Z0 is the 0xffffffff immediate, Z1 is 0xffffffff00000000.
; CHECK-LABEL: name: z0
; CHECK: INLINEASM {{.*}}imm, 4294967295
define i64 @z0(i64 %x) {
  %r = call i64 asm "and $0, $1, $2", "=r,r,^Z0"(i64 %x, i64 4294967295)
  ret i64 %r
}

; CHECK-LABEL: name: z1
; CHECK: INLINEASM {{.*}}imm, -4294967296
define i64 @z1(i64 %x) {
  %r = call i64 asm "or $0, $1, $2", "=r,r,^Z1"(i64 %x, i64 -4294967296)
  ret i64 %r
}
