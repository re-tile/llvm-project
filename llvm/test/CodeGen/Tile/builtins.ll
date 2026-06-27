; RUN: llc -mtriple=tilegx < %s | FileCheck %s

declare i32 @llvm.tilegx.addx(i32, i32)
declare i64 @llvm.tilegx.clz(i64)
declare i64 @llvm.tilegx.crc32.32(i64, i64)
declare i64 @llvm.tilegx.crc32.8(i64, i64)
declare i64 @llvm.tilegx.mm(i64, i64, i64, i64)
declare i64 @llvm.tilegx.exch(ptr, i64)
declare i64 @llvm.tilegx.fetchadd(ptr, i64)
declare i64 @llvm.tilegx.cmpexch(ptr, i64)
declare i64 @llvm.tilegx.mfspr(i64)
declare void @llvm.tilegx.mtspr(i64, i64)
declare i64 @llvm.tilegx.v1add(i64, i64)

; CHECK-LABEL: t_addx:
; CHECK: addx r0, r0, r1
define i32 @t_addx(i32 %a, i32 %b) {
  %r = call i32 @llvm.tilegx.addx(i32 %a, i32 %b)
  ret i32 %r
}

; CHECK-LABEL: t_clz:
; CHECK: clz r0, r0
define i64 @t_clz(i64 %a) {
  %r = call i64 @llvm.tilegx.clz(i64 %a)
  ret i64 %r
}

; CHECK-LABEL: t_crc32:
; CHECK: crc32_32 r0, r0, r1
; CHECK: crc32_8 r0, r0, r1
define i64 @t_crc32(i64 %a, i64 %b) {
  %x = call i64 @llvm.tilegx.crc32.32(i64 %a, i64 %b)
  %y = call i64 @llvm.tilegx.crc32.8(i64 %x, i64 %b)
  ret i64 %y
}

; CHECK-LABEL: t_mm:
; CHECK: mm r0, r1, 5, 10
define i64 @t_mm(i64 %a, i64 %b) {
  %r = call i64 @llvm.tilegx.mm(i64 %a, i64 %b, i64 5, i64 10)
  ret i64 %r
}

; CHECK-LABEL: t_exch:
; CHECK: exch r0, r0, r1
define i64 @t_exch(ptr %p, i64 %v) {
  %r = call i64 @llvm.tilegx.exch(ptr %p, i64 %v)
  ret i64 %r
}

; CHECK-LABEL: t_fetchadd:
; CHECK: fetchadd r0, r0, r1
define i64 @t_fetchadd(ptr %p, i64 %v) {
  %r = call i64 @llvm.tilegx.fetchadd(ptr %p, i64 %v)
  ret i64 %r
}

; CHECK-LABEL: t_cmpexch:
; CHECK: cmpexch r0, r0, r1
define i64 @t_cmpexch(ptr %p, i64 %v) {
  %r = call i64 @llvm.tilegx.cmpexch(ptr %p, i64 %v)
  ret i64 %r
}

; CHECK-LABEL: t_spr:
; CHECK: mfspr r0, 2576
; CHECK: mtspr 2577, r0
define i64 @t_spr() {
  %v = call i64 @llvm.tilegx.mfspr(i64 2576)
  call void @llvm.tilegx.mtspr(i64 2577, i64 %v)
  ret i64 %v
}

; CHECK-LABEL: t_v1add:
; CHECK: v1add r0, r0, r1
define i64 @t_v1add(i64 %a, i64 %b) {
  %r = call i64 @llvm.tilegx.v1add(i64 %a, i64 %b)
  ret i64 %r
}
