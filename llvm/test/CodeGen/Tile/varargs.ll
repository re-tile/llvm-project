; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; Variadic functions: the unused argument registers are spilled into the save
; area in the prologue, and va_arg walks the va_list, stepping over the 16-byte
; reserve zone once it crosses the entry stack pointer.

declare void @llvm.va_start(ptr)
declare void @llvm.va_end(ptr)

; CHECK-LABEL: sum:
; The fixed argument is r0; r1-r9 are spilled for the variadic tail.
; CHECK: st {{.*}}, r9
; CHECK: jr lr
define i64 @sum(i64 %n, ...) {
  %ap = alloca ptr
  call void @llvm.va_start(ptr %ap)
  %v1 = va_arg ptr %ap, i64
  %v2 = va_arg ptr %ap, i64
  call void @llvm.va_end(ptr %ap)
  %s = add i64 %v1, %v2
  ret i64 %s
}
