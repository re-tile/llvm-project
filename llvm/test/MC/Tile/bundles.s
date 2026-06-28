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
