// RUN: %clang_cc1 -triple tilegx-unknown-linux-gnu -emit-llvm -O0 -o - %s | FileCheck %s

typedef unsigned long long u64;

// CHECK-LABEL: define{{.*}} i32 @t_addx
// CHECK: call i32 @llvm.tilegx.addx
unsigned t_addx(unsigned a, unsigned b) { return __insn_addx(a, b); }

// CHECK-LABEL: define{{.*}} i64 @t_crc
// CHECK: call i64 @llvm.tilegx.crc32.32
// CHECK: call i64 @llvm.tilegx.crc32.8
u64 t_crc(u64 a, u64 b) { return __insn_crc32_32(a, b) + __insn_crc32_8(a, b); }

// CHECK-LABEL: define{{.*}} i64 @t_mm
// CHECK: call i64 @llvm.tilegx.mm(i64 %{{.*}}, i64 %{{.*}}, i64 5, i64 10)
u64 t_mm(u64 a, u64 b) { return __insn_mm(a, b, 5, 10); }

// CHECK-LABEL: define{{.*}} i64 @t_spr
// CHECK: call i64 @llvm.tilegx.mfspr(i64 10112)
// CHECK: call void @llvm.tilegx.mtspr(i64 10113
u64 t_spr(void) {
  u64 v = __insn_mfspr(0x2780);
  __insn_mtspr(0x2781, v);
  return v;
}

// CHECK-LABEL: define{{.*}} i64 @t_atomic
// CHECK: call i64 @llvm.tilegx.exch
// CHECK: call i64 @llvm.tilegx.fetchadd
// CHECK: call i64 @llvm.tilegx.cmpexch
long t_atomic(void *p, long v) {
  return __insn_exch(p, v) + __insn_fetchadd(p, v) + __insn_cmpexch(p, v);
}

// CHECK-LABEL: define{{.*}} i64 @t_simd
// CHECK: call i64 @llvm.tilegx.v1add
// CHECK: call i64 @llvm.tilegx.v2add
u64 t_simd(u64 a, u64 b) { return __insn_v1add(a, b) + __insn_v2add(a, b); }
