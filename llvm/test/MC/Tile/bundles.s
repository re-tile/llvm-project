# RUN: llvm-mc -triple=tilegx --show-encoding %s | FileCheck %s

# A single-instruction { } bundle encodes identically to the bare instruction
# (the complementary issue slot already carries FNOP).

# CHECK: add r0, r0, r1                          # encoding: [0x00,0x30,0x48,0x51,0x00,0x08,0x06,0x28]
{ add r0, r0, r1 }

# CHECK: jr lr                                   # encoding: [0x00,0x30,0x48,0x51,0xe0,0x76,0x6a,0x28]
{ jr lr }

# CHECK: addi sp, sp, -16                        # encoding: [0x00,0x30,0x48,0x51,0xdb,0x86,0x0f,0x18]
{
	addi sp, sp, -16
}

# A two-instruction bundle packs one X0-encoded op (mulx) with one X1-encoded op
# (add). The 64-bit word is the X1 slot of the X1 op merged with the X0 slot of
# the X0 op, byte-identical to GNU tile-as. Slot order in the text is irrelevant.
# CHECK: { mulx r5, r6, r7 ; add r0, r1, r2 }    # encoding: [0x85,0x71,0xd0,0x50,0x20,0x10,0x06,0x28]
{ mulx r5, r6, r7 ; add r0, r1, r2 }

# CHECK: { add r3, r4, r5 ; mulx r0, r0, r1 }    # encoding: [0x00,0x10,0xd0,0xd0,0x81,0x28,0x06,0x28]
{ add r3, r4, r5 ; mulx r0, r0, r1 }

# Two integer ALU ops both match the X1 encoding; the first is re-encoded into
# its X0 twin so the pair occupies complementary slots, byte-identical to GNU
# tile-as. This is the case that lets ordinary integer code pack.
# CHECK: { add r0, r1, r2 ; sub r3, r4, r5 }     # encoding: [0x40,0x20,0x0c,0xd0,0x81,0x28,0x68,0x28]
{ add r0, r1, r2 ; sub r3, r4, r5 }

# CHECK: { shl r3, r4, r5 ; or r0, r1, r2 }      # encoding: [0x03,0x51,0x28,0x51,0x20,0x10,0x3a,0x28]
{ shl r3, r4, r5 ; or r0, r1, r2 }
