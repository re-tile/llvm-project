# RUN: not llvm-mc -triple=tilegx -filetype=obj %s 2>&1 | FileCheck %s

# CHECK: error: invalid instruction mnemonic
notaninsn r0, r1

# A bundle pairs one X0-encoded with one X1-encoded instruction. add and sub are
# both X1-encoded and the backend has no co-encoding for two X1 ops in one
# bundle, so the parser fails loudly rather than silently mis-encoding.
# CHECK: error: TILE-Gx bundle issues two instructions in the same pipe
{ add r0, r0, r1 ; sub r2, r2, r3 }
