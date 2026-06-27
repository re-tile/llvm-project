; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
; RUN: llc -mtriple=tilegx -filetype=obj < %s | llvm-readobj -r - | FileCheck %s --check-prefix=RELOC
;
; A dense switch lowers to a block-address jump table: the table base is formed
; with the halfword sequence, the selected entry is loaded and jumped to, and
; the .rodata entries carry R_TILEGX_64 relocations.

; CHECK-LABEL: sw:
; CHECK: moveli {{r[0-9]+}}, hw2_last(.LJTI0_0)
; CHECK: shl16insli {{r[0-9]+}}, {{r[0-9]+}}, hw1(.LJTI0_0)
; CHECK: shl16insli {{r[0-9]+}}, {{r[0-9]+}}, hw0(.LJTI0_0)
; CHECK: jrp

; RELOC: R_TILEGX_64
define i64 @sw(i64 %x) {
  switch i64 %x, label %d [
    i64 0, label %c0
    i64 1, label %c1
    i64 2, label %c2
    i64 3, label %c3
    i64 4, label %c4
    i64 5, label %c5
    i64 6, label %c6
    i64 7, label %c7
  ]
c0:
  ret i64 100
c1:
  ret i64 101
c2:
  ret i64 102
c3:
  ret i64 103
c4:
  ret i64 104
c5:
  ret i64 105
c6:
  ret i64 106
c7:
  ret i64 107
d:
  ret i64 0
}
