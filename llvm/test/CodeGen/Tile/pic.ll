; RUN: llc -mtriple=tilegx -relocation-model=pic -filetype=asm < %s | FileCheck %s
; RUN: llc -mtriple=tilegx -relocation-model=pic -filetype=obj < %s | llvm-readobj -r - | FileCheck %s --check-prefix=RELOC
;
; Under PIC the GOT base is recomputed PC-relatively (lnk + the
; _GLOBAL_OFFSET_TABLE_ - label difference, which assembles to the PCREL
; halfword relocations), globals are loaded through their GOT entry, and calls
; to preemptible symbols go through the PLT.

@g = external global i64
declare void @ext()

; CHECK-LABEL: load_g:
; CHECK: lnk r50
; CHECK: moveli r51, hw1_last(_GLOBAL_OFFSET_TABLE_-[[L:.Lgot_pc[0-9]+]])
; CHECK: shl16insli r51, r51, hw0(_GLOBAL_OFFSET_TABLE_-[[L]])
; CHECK: add r51, r51, r50
; CHECK: moveli r0, hw1_last_got(g)
; CHECK: shl16insli r0, r0, hw0_got(g)
; CHECK: add r0, r51, r0
; CHECK: ld r0, r0
; CHECK: ld r0, r0

; RELOC-LABEL: Relocations [
; RELOC: R_TILEGX_IMM16_X1_HW1_LAST_PCREL _GLOBAL_OFFSET_TABLE_ 0x0
; RELOC: R_TILEGX_IMM16_X1_HW0_PCREL _GLOBAL_OFFSET_TABLE_ 0x8
; RELOC: R_TILEGX_IMM16_X1_HW1_LAST_GOT g 0x0
; RELOC: R_TILEGX_IMM16_X1_HW0_GOT g 0x0
define i64 @load_g() {
  %v = load i64, ptr @g
  ret i64 %v
}

; CHECK-LABEL: call_ext:
; CHECK: jal plt(ext)
; RELOC: R_TILEGX_JUMPOFF_X1_PLT ext
define void @call_ext() {
  call void @ext()
  ret void
}
