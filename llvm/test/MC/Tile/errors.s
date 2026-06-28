# RUN: not llvm-mc -triple=tilegx -filetype=obj %s 2>&1 | FileCheck %s

# CHECK: error: invalid instruction mnemonic
notaninsn r0, r1

# A bundle fills one X0 slot and one X1 slot. Two natively-X0 ops (a multiply, fp
# helper or mm) both need the low slot and the backend has no X1 co-encoding, so
# the parser fails loudly rather than mis-encode.
# CHECK: error: TILE-Gx bundle issues two instructions in the X0 pipe
{ mulx r0, r1, r2 ; mulx r3, r4, r5 }

# Two X1-encoded ops without an X0 twin (moveli has no X0 form) cannot share a
# bundle either.
# CHECK: error: TILE-Gx bundle issues two instructions in the X1 pipe
{ moveli r0, 1 ; moveli r1, 2 }

# Loads and stores use the Y2 bundle mode and cannot be slot-merged.
# CHECK: error: TILE-Gx load/store uses the Y2 bundle mode
{ ld r0, r1 ; add r2, r3, r4 }
