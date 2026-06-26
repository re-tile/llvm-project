; RUN: llc -mtriple=tilegx -filetype=obj %s -o %t.o
; RUN: llvm-objdump -s -j .text %t.o | FileCheck %s --check-prefix=TEXT
; RUN: llvm-readobj -h %t.o | FileCheck %s --check-prefix=ELF

; Each first-light instruction is its own 64-bit VLIW bundle with the unused
; issue slot filled by FNOP (X0 fnop = 0x51483000). These byte patterns are
; verified equal to the real MDE tile-as output and decode under tile-objdump
; as { add r0,r0,r1 } / { sub } / { xor } / { addx } and { jr lr }.

; ELF: Machine: EM_TILEGX

; TEXT:      Contents of section .text:
; TEXT-NEXT: 0000 00304851 00080628 00304851 e0766a28
; TEXT-NEXT: 0010 00304851 00086828 00304851 e0766a28
; TEXT-NEXT: 0020 00304851 0008d628 00304851 e0766a28
; TEXT-NEXT: 0030 00304851 00080428 00304851 e0766a28

define i64 @add64(i64 %a, i64 %b) {
  %r = add i64 %a, %b
  ret i64 %r
}

define i64 @sub64(i64 %a, i64 %b) {
  %r = sub i64 %a, %b
  ret i64 %r
}

define i64 @xor64(i64 %a, i64 %b) {
  %r = xor i64 %a, %b
  ret i64 %r
}

define i32 @addx32(i32 %a, i32 %b) {
  %r = add i32 %a, %b
  ret i32 %r
}
