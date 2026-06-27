; RUN: llc -mtriple=tilegx -filetype=asm < %s | FileCheck %s
;
; TILE-Gx has no FPU. Following GCC-12, float and double add/sub/mul are
; expanded inline into the fsingle/fdouble helper instruction sequences, the
; compares use the same helpers finished in TileExpandPseudo, and everything
; else (division, the integer/float conversions, the widen/narrow, square root)
; goes to the soft-float libgcc helpers. Float literals are loaded from the
; constant pool.

; CHECK-LABEL: fadd_f:
; CHECK: fsingle_add1
; CHECK: fsingle_addsub2
; CHECK: fsingle_pack1
; CHECK: fsingle_pack2
define float @fadd_f(float %a, float %b) {
  %r = fadd float %a, %b
  ret float %r
}

; CHECK-LABEL: fmul_f:
; CHECK: fsingle_mul1
; CHECK: fsingle_mul2
; CHECK: fsingle_pack1
; CHECK: fsingle_pack2
define float @fmul_f(float %a, float %b) {
  %r = fmul float %a, %b
  ret float %r
}

; CHECK-LABEL: dadd_f:
; CHECK-DAG: fdouble_unpack_min
; CHECK-DAG: fdouble_unpack_max
; CHECK-DAG: fdouble_add_flags
; CHECK: fdouble_addsub
; CHECK: fdouble_pack1
; CHECK: fdouble_pack2
define double @dadd_f(double %a, double %b) {
  %r = fadd double %a, %b
  ret double %r
}

; CHECK-LABEL: dsub_f:
; CHECK: fdouble_sub_flags
; CHECK: fdouble_pack2
define double @dsub_f(double %a, double %b) {
  %r = fsub double %a, %b
  ret double %r
}

; CHECK-LABEL: dmul_f:
; CHECK-DAG: fdouble_unpack_max
; CHECK-DAG: fdouble_mul_flags
; CHECK-DAG: mul_lu_lu
; CHECK-DAG: mul_hu_hu
; CHECK: fdouble_pack2
define double @dmul_f(double %a, double %b) {
  %r = fmul double %a, %b
  ret double %r
}

; CHECK-LABEL: ddiv_f:
; CHECK: jal __divdf3
define double @ddiv_f(double %a, double %b) {
  %r = fdiv double %a, %b
  ret double %r
}

; CHECK-LABEL: fdiv_f:
; CHECK: jal __divsf3
define float @fdiv_f(float %a, float %b) {
  %r = fdiv float %a, %b
  ret float %r
}

; CHECK-LABEL: si64_to_f:
; CHECK: jal __floatdisf
define float @si64_to_f(i64 %a) {
  %r = sitofp i64 %a to float
  ret float %r
}

; CHECK-LABEL: d_to_si32:
; CHECK: jal __fixdfsi
define i32 @d_to_si32(double %a) {
  %r = fptosi double %a to i32
  ret i32 %r
}

; CHECK-LABEL: d_to_si64:
; CHECK: jal __fixdfdi
define i64 @d_to_si64(double %a) {
  %r = fptosi double %a to i64
  ret i64 %r
}

; CHECK-LABEL: f_to_d:
; CHECK: jal __extendsfdf2
define double @f_to_d(float %a) {
  %r = fpext float %a to double
  ret double %r
}

; CHECK-LABEL: d_to_f:
; CHECK: jal __truncdfsf2
define float @d_to_f(double %a) {
  %r = fptrunc double %a to float
  ret float %r
}

; CHECK-LABEL: dcmp_olt:
; CHECK: fdouble_add_flags
define i32 @dcmp_olt(double %a, double %b) {
  %c = fcmp olt double %a, %b
  %r = zext i1 %c to i32
  ret i32 %r
}

; CHECK-LABEL: fcmp_une:
; CHECK: fsingle_add1
define i32 @fcmp_une(float %a, float %b) {
  %c = fcmp une float %a, %b
  %r = zext i1 %c to i32
  ret i32 %r
}

; A float literal is materialized from the constant pool, not an fp immediate.
; CHECK-LABEL: dconst:
; CHECK: moveli {{.*}}hw2_last(.LCPI
; CHECK: shl16insli {{.*}}hw1(.LCPI
; CHECK: shl16insli {{.*}}hw0(.LCPI
; CHECK: ld
define double @dconst() {
  ret double 3.5
}

; CHECK-LABEL: dabs:
; CHECK: bfextu
define double @dabs(double %a) {
  %r = call double @llvm.fabs.f64(double %a)
  ret double %r
}

declare double @llvm.fabs.f64(double)
