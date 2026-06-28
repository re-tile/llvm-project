// Inline asm now flows all the way to a TILE-Gx object: the integrated
// assembler drives the new MCAsmParser to encode the asm block. Before the
// parser existed clang aborted with "we don't have an asm parser for this
// target".
// REQUIRES: tile-registered-target
// RUN: %clang_cc1 -triple tilegx-unknown-linux-gnu -O2 -S -o - %s | FileCheck %s --check-prefix=ASM
// RUN: %clang_cc1 -triple tilegx-unknown-linux-gnu -O2 -emit-obj -o %t.o %s
// RUN: llvm-readobj -h %t.o | FileCheck %s --check-prefix=OBJ

// ASM-LABEL: add_asm:
// ASM: add r0, r0, r1
long add_asm(long a, long b) {
  long r;
  asm("add %0, %1, %2" : "=r"(r) : "r"(a), "r"(b));
  return r;
}

// The R0n fixed-register constraints bind to the named physical registers.
// ASM-LABEL: fixed_reg:
// ASM: addi r5, r3, 7
long fixed_reg(long a) {
  long r;
  asm("addi %0, %1, 7" : "=R05"(r) : "R03"(a));
  return r;
}

// OBJ: Machine: EM_TILEGX
