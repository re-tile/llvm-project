// RUN: %clang_cc1 -E -dM -ffreestanding -triple=tilegx-unknown-linux-gnu < /dev/null | FileCheck -match-full-lines -check-prefix TILEGX %s

// TILEGX:#define _LP64 1
// TILEGX:#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
// TILEGX:#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 1
// TILEGX:#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 1
// TILEGX:#define __LITTLE_ENDIAN__ 1
// TILEGX:#define __LP64__ 1
// TILEGX:#define __SIZEOF_INT__ 4
// TILEGX:#define __SIZEOF_LONG__ 8
// TILEGX:#define __SIZEOF_POINTER__ 8
// TILEGX:#define __TILEGX__ 1
// TILEGX:#define __tile__ 1
// TILEGX:#define __tile_chip__ 10
// TILEGX:#define __tile_chip_rev__ 0
// TILEGX:#define __tilegx__ 1
