# RUN: not llvm-mc -triple=tilegx -filetype=obj %s 2>&1 | FileCheck %s

# CHECK: error: invalid instruction mnemonic
notaninsn r0, r1

# Two multiplies both need X0 or Y0, and X mode has one X0 slot.
# CHECK: error: no TILE-Gx pipe assignment encodes this bundle
{ mulx r0, r1, r2 ; mulx r3, r4, r5 }

# CHECK: error: instruction may not be bundled with others
{ swint1 ; add r0, r1, r2 }

# CHECK: error: operand out of range
addi r0, r0, 300

# CHECK: error: operator may only be applied to a symbol
moveli r1, hw0_got(g + 8)
