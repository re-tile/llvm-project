# RUN: not llvm-mc -triple=tilegx -filetype=obj %s 2>&1 | FileCheck %s

# CHECK: error: invalid instruction mnemonic
notaninsn r0, r1

# Multi-instruction bundle packing (the VLIW packetizer) is not implemented, so
# the parser fails loudly rather than silently mis-encoding.
# CHECK: error: TILE-Gx multi-instruction bundle packing is not yet supported
{ add r0, r0, r1 ; sub r2, r2, r3 }
