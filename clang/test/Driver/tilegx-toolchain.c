// Check that the driver can drive an assemble-and-link for TILE-Gx through the
// generic GNU/Linux toolchain: the integrated assembler is the default and the
// linker is invoked with the tilegx ELF emulation and dynamic linker.

// RUN: %clang --target=tilegx-unknown-linux-gnu -### %s 2>&1 \
// RUN:   | FileCheck %s
// CHECK: "-cc1"
// CHECK-SAME: "-triple" "tilegx-unknown-linux-gnu"
// CHECK: "-m" "elf64tilegx"
// CHECK-SAME: "-dynamic-linker" "/lib/ld.so.1"

// A static link drops the dynamic linker but keeps the tilegx emulation.
// RUN: %clang --target=tilegx-unknown-linux-gnu -static -### %s 2>&1 \
// RUN:   | FileCheck --check-prefix=STATIC %s
// STATIC: "-m" "elf64tilegx"
// STATIC: "-static"
