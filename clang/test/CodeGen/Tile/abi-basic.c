// REQUIRES: tile-registered-target
// RUN: %clang_cc1 -triple tilegx-unknown-linux-gnu -O2 -emit-llvm -o - %s | FileCheck %s

// The clang datalayout must match what the TILE-Gx backend computes.
// CHECK: target datalayout = "e-m:e-p:64:64-i8:8:32-i16:16:32-i64:64-n32:64-S128"

// A small struct is returned in registers, coerced to integer doublewords.
struct two { long a; long b; };

// CHECK-LABEL: define {{.*}}[2 x i64] @ret_small(
struct two ret_small(long x, long y) {
  struct two r = {x, y};
  return r;
}

// A large struct is returned through an sret pointer.
struct big { long v[16]; };

// CHECK-LABEL: define {{.*}}void @ret_big(ptr {{.*}}sret
struct big ret_big(void) {
  struct big b = {};
  return b;
}

// Small integers are extended to a register on the way in.
// CHECK-LABEL: define {{.*}}i64 @addc(
// CHECK-SAME: i8 noundef signext %a, i8 noundef signext %b
long addc(char a, char b) {
  return (long)a + (long)b;
}
