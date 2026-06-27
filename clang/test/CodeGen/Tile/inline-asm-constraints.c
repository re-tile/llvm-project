// Check that the multi-character tilegx asm constraints are accepted and
// lowered to the length-tagged forms LLVM expects: "@N" for an N-character
// register constraint, "^" for a two-character one.
// RUN: %clang_cc1 -triple tilegx-unknown-linux-gnu -emit-llvm -O1 -o - %s \
// RUN:   | FileCheck %s

// CHECK-LABEL: @regs
// CHECK: call i64 asm "move $0, $1", "=@3R05,@3R00"
long regs(long x) {
  long r;
  __asm__("move %0, %1" : "=R05"(r) : "R00"(x));
  return r;
}

// CHECK-LABEL: @mask
// CHECK: call i64 asm "and $0, $1, $2", "=r,r,^Z0"
long mask(long x) {
  long r;
  __asm__("and %0, %1, %2" : "=r"(r) : "r"(x), "Z0"(0xffffffffL));
  return r;
}
