//===- IntrinsicsTile.td - Defines TILE-Gx intrinsics ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the TILE-Gx intrinsics.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//

let TargetPrefix = "tilegx" in {  // All intrinsics start with "llvm.tilegx.".

// v1XXX
def int_tilegx_v1add : ClangBuiltin<"__insn_v1add">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [Commutative]>;

def int_tilegx_v1addi : ClangBuiltin<"__insn_v1addi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1adduc : ClangBuiltin<"__insn_v1adduc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1adiffu : ClangBuiltin<"__insn_v1adiffu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1avgu : ClangBuiltin<"__insn_v1avgu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmpeq : ClangBuiltin<"__insn_v1cmpeq">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmpeqi : ClangBuiltin<"__insn_v1cmpeqi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmples : ClangBuiltin<"__insn_v1cmples">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmpleu : ClangBuiltin<"__insn_v1cmpleu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmplts : ClangBuiltin<"__insn_v1cmplts">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmpltsi : ClangBuiltin<"__insn_v1cmpltsi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmpltu : ClangBuiltin<"__insn_v1cmpltu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmpltui : ClangBuiltin<"__insn_v1cmpltui">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1cmpne : ClangBuiltin<"__insn_v1cmpne">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1ddotpu : ClangBuiltin<"__insn_v1ddotpu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1ddotpua : ClangBuiltin<"__insn_v1ddotpua">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1ddotpus : ClangBuiltin<"__insn_v1ddotpus">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1ddotpusa : ClangBuiltin<"__insn_v1ddotpusa">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1dotp : ClangBuiltin<"__insn_v1dotp">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1dotpa : ClangBuiltin<"__insn_v1dotpa">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1dotpu : ClangBuiltin<"__insn_v1dotpu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1dotpua : ClangBuiltin<"__insn_v1dotpua">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1dotpus : ClangBuiltin<"__insn_v1dotpus">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1dotpusa : ClangBuiltin<"__insn_v1dotpusa">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1int_h : ClangBuiltin<"__insn_v1int_h">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1int_l : ClangBuiltin<"__insn_v1int_l">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1maxu : ClangBuiltin<"__insn_v1maxu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1maxui : ClangBuiltin<"__insn_v1maxui">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1minu : ClangBuiltin<"__insn_v1minu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1minui : ClangBuiltin<"__insn_v1minui">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1mnz : ClangBuiltin<"__insn_v1mnz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1multu : ClangBuiltin<"__insn_v1multu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1mulu : ClangBuiltin<"__insn_v1mulu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1mulus : ClangBuiltin<"__insn_v1mulus">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1mz : ClangBuiltin<"__insn_v1mz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1sadau : ClangBuiltin<"__insn_v1sadau">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1sadu : ClangBuiltin<"__insn_v1sadu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1shl : ClangBuiltin<"__insn_v1shl">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1shli : ClangBuiltin<"__insn_v1shli">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1shrs : ClangBuiltin<"__insn_v1shrs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1shrsi : ClangBuiltin<"__insn_v1shrsi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1shru : ClangBuiltin<"__insn_v1shru">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1shrui : ClangBuiltin<"__insn_v1shrui">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1sub : ClangBuiltin<"__insn_v1sub">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v1subuc : ClangBuiltin<"__insn_v1subuc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

// v2XXX
def int_tilegx_v2add : ClangBuiltin<"__insn_v2add">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [Commutative]>;

def int_tilegx_v2addi : ClangBuiltin<"__insn_v2addi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2addsc : ClangBuiltin<"__insn_v2addsc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2adiffs : ClangBuiltin<"__insn_v2adiffs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2avgs : ClangBuiltin<"__insn_v2avgs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmpeq : ClangBuiltin<"__insn_v2cmpeq">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmpeqi : ClangBuiltin<"__insn_v2cmpeqi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmples : ClangBuiltin<"__insn_v2cmples">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmpleu : ClangBuiltin<"__insn_v2cmpleu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmplts : ClangBuiltin<"__insn_v2cmplts">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmpltsi : ClangBuiltin<"__insn_v2cmpltsi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmpltu : ClangBuiltin<"__insn_v2cmpltu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmpltui : ClangBuiltin<"__insn_v2cmpltui">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2cmpne : ClangBuiltin<"__insn_v2cmpne">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2dotp : ClangBuiltin<"__insn_v2dotp">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2dotpa : ClangBuiltin<"__insn_v2dotpa">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2int_h : ClangBuiltin<"__insn_v2int_h">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2int_l : ClangBuiltin<"__insn_v2int_l">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2maxs : ClangBuiltin<"__insn_v2maxs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2maxsi : ClangBuiltin<"__insn_v2maxsi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2mins : ClangBuiltin<"__insn_v2mins">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2minsi : ClangBuiltin<"__insn_v2minsi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2mnz : ClangBuiltin<"__insn_v2mnz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2mulfsc : ClangBuiltin<"__insn_v2mulfsc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2muls : ClangBuiltin<"__insn_v2muls">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2mults : ClangBuiltin<"__insn_v2mults">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2mz : ClangBuiltin<"__insn_v2mz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2packh : ClangBuiltin<"__insn_v2packh">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2packl : ClangBuiltin<"__insn_v2packl">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2packuc : ClangBuiltin<"__insn_v2packuc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2sadas : ClangBuiltin<"__insn_v2sadas">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2sadau : ClangBuiltin<"__insn_v2sadau">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2sads : ClangBuiltin<"__insn_v2sads">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2sadu : ClangBuiltin<"__insn_v2sadu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2shl : ClangBuiltin<"__insn_v2shl">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2shli : ClangBuiltin<"__insn_v2shli">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2shlsc : ClangBuiltin<"__insn_v2shlsc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2shrs : ClangBuiltin<"__insn_v2shrs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2shrsi : ClangBuiltin<"__insn_v2shrsi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2shru : ClangBuiltin<"__insn_v2shru">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2shrui : ClangBuiltin<"__insn_v2shrui">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2sub : ClangBuiltin<"__insn_v2sub">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v2subsc : ClangBuiltin<"__insn_v2subsc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

// v4XXX
def int_tilegx_v4add : ClangBuiltin<"__insn_v4add">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [Commutative]>;

def int_tilegx_v4addsc : ClangBuiltin<"__insn_v4addsc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4int_h : ClangBuiltin<"__insn_v4int_h">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4int_l : ClangBuiltin<"__insn_v4int_l">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4packsc : ClangBuiltin<"__insn_v4packsc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4shl : ClangBuiltin<"__insn_v4shl">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4shlsc : ClangBuiltin<"__insn_v4shlsc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4shrs : ClangBuiltin<"__insn_v4shrs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4shru : ClangBuiltin<"__insn_v4shru">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4sub : ClangBuiltin<"__insn_v4sub">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

def int_tilegx_v4subsc : ClangBuiltin<"__insn_v4subsc">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], []>;

// Network
def int_tilegx_idn0r : ClangBuiltin<"__tile_idn0_receive">,
  Intrinsic<[llvm_i64_ty], [], []>;

def int_tilegx_idn1r : ClangBuiltin<"__tile_idn1_receive">,
  Intrinsic<[llvm_i64_ty], [], []>;

def int_tilegx_udn0r : ClangBuiltin<"__tile_udn0_receive">,
  Intrinsic<[llvm_i64_ty], [], []>;

def int_tilegx_udn1r : ClangBuiltin<"__tile_udn1_receive">,
  Intrinsic<[llvm_i64_ty], [], []>;

def int_tilegx_udn2r : ClangBuiltin<"__tile_udn2_receive">,
  Intrinsic<[llvm_i64_ty], [], []>;

def int_tilegx_udn3r : ClangBuiltin<"__tile_udn3_receive">,
  Intrinsic<[llvm_i64_ty], [], []>;

def int_tilegx_netbarrier : ClangBuiltin<"__tile_network_barrier">,
  Intrinsic<[], [], []>;

def int_tilegx_usend : ClangBuiltin<"__tile_udn_send">,
  Intrinsic<[], [llvm_i64_ty], []>;

def int_tilegx_isend : ClangBuiltin<"__tile_idn_send">,
  Intrinsic<[], [llvm_i64_ty], []>;

// Others
def int_tilegx_mfspr : ClangBuiltin<"__insn_mfspr">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], []>;

def int_tilegx_mtspr : ClangBuiltin<"__insn_mtspr">,
  Intrinsic<[], [llvm_i64_ty, llvm_i64_ty], []>;

// Scalar arithmetic and bit manipulation
def int_tilegx_addx : ClangBuiltin<"__insn_addx">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem, Commutative]>;
def int_tilegx_clz : ClangBuiltin<"__insn_clz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_ctz : ClangBuiltin<"__insn_ctz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_pcnt : ClangBuiltin<"__insn_pcnt">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_revbytes : ClangBuiltin<"__insn_revbytes">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_revbits : ClangBuiltin<"__insn_revbits">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_crc32_32 : ClangBuiltin<"__insn_crc32_32">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_crc32_8 : ClangBuiltin<"__insn_crc32_8">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mm : ClangBuiltin<"__insn_mm">,
  Intrinsic<[llvm_i64_ty],
            [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;

// Atomic memory operations. The first argument is the address.
def int_tilegx_exch : ClangBuiltin<"__insn_exch">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty, llvm_i64_ty], [IntrArgMemOnly]>;
def int_tilegx_exch4 : ClangBuiltin<"__insn_exch4">,
  Intrinsic<[llvm_i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrArgMemOnly]>;
def int_tilegx_fetchadd : ClangBuiltin<"__insn_fetchadd">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty, llvm_i64_ty], [IntrArgMemOnly]>;
def int_tilegx_fetchadd4 : ClangBuiltin<"__insn_fetchadd4">,
  Intrinsic<[llvm_i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrArgMemOnly]>;
def int_tilegx_fetchand : ClangBuiltin<"__insn_fetchand">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty, llvm_i64_ty], [IntrArgMemOnly]>;
def int_tilegx_fetchand4 : ClangBuiltin<"__insn_fetchand4">,
  Intrinsic<[llvm_i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrArgMemOnly]>;
def int_tilegx_fetchor : ClangBuiltin<"__insn_fetchor">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty, llvm_i64_ty], [IntrArgMemOnly]>;
def int_tilegx_fetchor4 : ClangBuiltin<"__insn_fetchor4">,
  Intrinsic<[llvm_i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrArgMemOnly]>;
def int_tilegx_fetchaddgez : ClangBuiltin<"__insn_fetchaddgez">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty, llvm_i64_ty], [IntrArgMemOnly]>;
def int_tilegx_fetchaddgez4 : ClangBuiltin<"__insn_fetchaddgez4">,
  Intrinsic<[llvm_i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrArgMemOnly]>;
def int_tilegx_cmpexch : ClangBuiltin<"__insn_cmpexch">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty, llvm_i64_ty], [IntrArgMemOnly]>;
def int_tilegx_cmpexch4 : ClangBuiltin<"__insn_cmpexch4">,
  Intrinsic<[llvm_i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrArgMemOnly]>;

// Cache, prefetch and alignment helpers lowered through the opcode table.
def int_tilegx_dblalign : ClangBuiltin<"__insn_dblalign">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_ptr_ty], [IntrNoMem]>;
foreach N = ["2", "4", "6"] in
def int_tilegx_dblalign#N : ClangBuiltin<"__insn_dblalign"#N>,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_shufflebytes : ClangBuiltin<"__insn_shufflebytes">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_shl : ClangBuiltin<"__insn_shl">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
foreach Name = ["prefetch", "prefetch_l1", "prefetch_l1_fault", "prefetch_l2",
                "prefetch_l2_fault", "prefetch_l3", "prefetch_l3_fault",
                "wh64", "finv", "flush", "inv", "icoh"] in
def int_tilegx_#Name : ClangBuiltin<"__insn_"#Name>,
  Intrinsic<[], [llvm_ptr_ty], [IntrHasSideEffects]>;
foreach Name = ["drain", "flushwb", "mf", "nap"] in
def int_tilegx_#Name : ClangBuiltin<"__insn_"#Name>,
  Intrinsic<[], [], [IntrHasSideEffects]>;

def int_tilegx_add : ClangBuiltin<"__insn_add">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_addi : ClangBuiltin<"__insn_addi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_addli : ClangBuiltin<"__insn_addli">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_addxi : ClangBuiltin<"__insn_addxi">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_addxli : ClangBuiltin<"__insn_addxli">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_addxsc : ClangBuiltin<"__insn_addxsc">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_and : ClangBuiltin<"__insn_and">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_andi : ClangBuiltin<"__insn_andi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_bfexts : ClangBuiltin<"__insn_bfexts">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_bfextu : ClangBuiltin<"__insn_bfextu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_bfins : ClangBuiltin<"__insn_bfins">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmoveqz : ClangBuiltin<"__insn_cmoveqz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmovnez : ClangBuiltin<"__insn_cmovnez">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmpeq : ClangBuiltin<"__insn_cmpeq">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmpeqi : ClangBuiltin<"__insn_cmpeqi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmples : ClangBuiltin<"__insn_cmples">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmpleu : ClangBuiltin<"__insn_cmpleu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmplts : ClangBuiltin<"__insn_cmplts">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmpltsi : ClangBuiltin<"__insn_cmpltsi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmpltu : ClangBuiltin<"__insn_cmpltu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmpltui : ClangBuiltin<"__insn_cmpltui">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmpne : ClangBuiltin<"__insn_cmpne">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmul : ClangBuiltin<"__insn_cmul">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmula : ClangBuiltin<"__insn_cmula">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmulaf : ClangBuiltin<"__insn_cmulaf">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmulf : ClangBuiltin<"__insn_cmulf">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmulfr : ClangBuiltin<"__insn_cmulfr">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmulh : ClangBuiltin<"__insn_cmulh">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_cmulhr : ClangBuiltin<"__insn_cmulhr">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_dtlbpr : ClangBuiltin<"__insn_dtlbpr">,
  Intrinsic<[], [llvm_i64_ty], [IntrHasSideEffects]>;
def int_tilegx_fdouble_add_flags : ClangBuiltin<"__insn_fdouble_add_flags">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fdouble_addsub : ClangBuiltin<"__insn_fdouble_addsub">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fdouble_mul_flags : ClangBuiltin<"__insn_fdouble_mul_flags">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fdouble_pack1 : ClangBuiltin<"__insn_fdouble_pack1">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fdouble_pack2 : ClangBuiltin<"__insn_fdouble_pack2">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fdouble_sub_flags : ClangBuiltin<"__insn_fdouble_sub_flags">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fdouble_unpack_max : ClangBuiltin<"__insn_fdouble_unpack_max">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fdouble_unpack_min : ClangBuiltin<"__insn_fdouble_unpack_min">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fnop : ClangBuiltin<"__insn_fnop">,
  Intrinsic<[], [], [IntrHasSideEffects]>;
def int_tilegx_fsingle_add1 : ClangBuiltin<"__insn_fsingle_add1">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fsingle_addsub2 : ClangBuiltin<"__insn_fsingle_addsub2">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fsingle_mul1 : ClangBuiltin<"__insn_fsingle_mul1">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fsingle_mul2 : ClangBuiltin<"__insn_fsingle_mul2">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fsingle_pack1 : ClangBuiltin<"__insn_fsingle_pack1">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fsingle_pack2 : ClangBuiltin<"__insn_fsingle_pack2">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_fsingle_sub1 : ClangBuiltin<"__insn_fsingle_sub1">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_ill : ClangBuiltin<"__insn_ill">,
  Intrinsic<[], [], [IntrHasSideEffects]>;
def int_tilegx_info : ClangBuiltin<"__insn_info">,
  Intrinsic<[], [llvm_i64_ty], [IntrHasSideEffects]>;
def int_tilegx_infol : ClangBuiltin<"__insn_infol">,
  Intrinsic<[], [llvm_i64_ty], [IntrHasSideEffects]>;
def int_tilegx_ld : ClangBuiltin<"__insn_ld">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld1s : ClangBuiltin<"__insn_ld1s">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld1u : ClangBuiltin<"__insn_ld1u">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld2s : ClangBuiltin<"__insn_ld2s">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld2u : ClangBuiltin<"__insn_ld2u">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld4s : ClangBuiltin<"__insn_ld4s">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld4u : ClangBuiltin<"__insn_ld4u">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldna : ClangBuiltin<"__insn_ldna">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt : ClangBuiltin<"__insn_ldnt">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt1s : ClangBuiltin<"__insn_ldnt1s">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt1u : ClangBuiltin<"__insn_ldnt1u">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt2s : ClangBuiltin<"__insn_ldnt2s">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt2u : ClangBuiltin<"__insn_ldnt2u">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt4s : ClangBuiltin<"__insn_ldnt4s">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt4u : ClangBuiltin<"__insn_ldnt4u">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld_l2 : ClangBuiltin<"__insn_ld_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld1s_l2 : ClangBuiltin<"__insn_ld1s_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld1u_l2 : ClangBuiltin<"__insn_ld1u_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld2s_l2 : ClangBuiltin<"__insn_ld2s_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld2u_l2 : ClangBuiltin<"__insn_ld2u_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld4s_l2 : ClangBuiltin<"__insn_ld4s_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld4u_l2 : ClangBuiltin<"__insn_ld4u_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldna_l2 : ClangBuiltin<"__insn_ldna_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt_l2 : ClangBuiltin<"__insn_ldnt_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt1s_l2 : ClangBuiltin<"__insn_ldnt1s_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt1u_l2 : ClangBuiltin<"__insn_ldnt1u_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt2s_l2 : ClangBuiltin<"__insn_ldnt2s_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt2u_l2 : ClangBuiltin<"__insn_ldnt2u_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt4s_l2 : ClangBuiltin<"__insn_ldnt4s_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt4u_l2 : ClangBuiltin<"__insn_ldnt4u_L2">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld_miss : ClangBuiltin<"__insn_ld_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld1s_miss : ClangBuiltin<"__insn_ld1s_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld1u_miss : ClangBuiltin<"__insn_ld1u_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld2s_miss : ClangBuiltin<"__insn_ld2s_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld2u_miss : ClangBuiltin<"__insn_ld2u_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld4s_miss : ClangBuiltin<"__insn_ld4s_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ld4u_miss : ClangBuiltin<"__insn_ld4u_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldna_miss : ClangBuiltin<"__insn_ldna_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt_miss : ClangBuiltin<"__insn_ldnt_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt1s_miss : ClangBuiltin<"__insn_ldnt1s_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt1u_miss : ClangBuiltin<"__insn_ldnt1u_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt2s_miss : ClangBuiltin<"__insn_ldnt2s_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt2u_miss : ClangBuiltin<"__insn_ldnt2u_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt4s_miss : ClangBuiltin<"__insn_ldnt4s_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_ldnt4u_miss : ClangBuiltin<"__insn_ldnt4u_miss">,
  Intrinsic<[llvm_i64_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
def int_tilegx_lnk : ClangBuiltin<"__insn_lnk">,
  Intrinsic<[llvm_i64_ty], [], [IntrNoMem]>;
def int_tilegx_mnz : ClangBuiltin<"__insn_mnz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_move : ClangBuiltin<"__insn_move">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_movei : ClangBuiltin<"__insn_movei">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_moveli : ClangBuiltin<"__insn_moveli">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_hs_hs : ClangBuiltin<"__insn_mul_hs_hs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_hs_hu : ClangBuiltin<"__insn_mul_hs_hu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_hs_ls : ClangBuiltin<"__insn_mul_hs_ls">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_hs_lu : ClangBuiltin<"__insn_mul_hs_lu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_hu_hu : ClangBuiltin<"__insn_mul_hu_hu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_hu_ls : ClangBuiltin<"__insn_mul_hu_ls">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_hu_lu : ClangBuiltin<"__insn_mul_hu_lu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_ls_ls : ClangBuiltin<"__insn_mul_ls_ls">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_ls_lu : ClangBuiltin<"__insn_mul_ls_lu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mul_lu_lu : ClangBuiltin<"__insn_mul_lu_lu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_hs_hs : ClangBuiltin<"__insn_mula_hs_hs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_hs_hu : ClangBuiltin<"__insn_mula_hs_hu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_hs_ls : ClangBuiltin<"__insn_mula_hs_ls">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_hs_lu : ClangBuiltin<"__insn_mula_hs_lu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_hu_hu : ClangBuiltin<"__insn_mula_hu_hu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_hu_ls : ClangBuiltin<"__insn_mula_hu_ls">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_hu_lu : ClangBuiltin<"__insn_mula_hu_lu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_ls_ls : ClangBuiltin<"__insn_mula_ls_ls">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_ls_lu : ClangBuiltin<"__insn_mula_ls_lu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mula_lu_lu : ClangBuiltin<"__insn_mula_lu_lu">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_mulax : ClangBuiltin<"__insn_mulax">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_mulx : ClangBuiltin<"__insn_mulx">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_mz : ClangBuiltin<"__insn_mz">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_nop : ClangBuiltin<"__insn_nop">,
  Intrinsic<[], [], [IntrHasSideEffects]>;
def int_tilegx_nor : ClangBuiltin<"__insn_nor">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_or : ClangBuiltin<"__insn_or">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_ori : ClangBuiltin<"__insn_ori">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_rotl : ClangBuiltin<"__insn_rotl">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_rotli : ClangBuiltin<"__insn_rotli">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shl16insli : ClangBuiltin<"__insn_shl16insli">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_shl1add : ClangBuiltin<"__insn_shl1add">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_shl1addx : ClangBuiltin<"__insn_shl1addx">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shl2add : ClangBuiltin<"__insn_shl2add">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_shl2addx : ClangBuiltin<"__insn_shl2addx">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shl3add : ClangBuiltin<"__insn_shl3add">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_shl3addx : ClangBuiltin<"__insn_shl3addx">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shli : ClangBuiltin<"__insn_shli">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shlx : ClangBuiltin<"__insn_shlx">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shlxi : ClangBuiltin<"__insn_shlxi">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shrs : ClangBuiltin<"__insn_shrs">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shrsi : ClangBuiltin<"__insn_shrsi">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shru : ClangBuiltin<"__insn_shru">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shrui : ClangBuiltin<"__insn_shrui">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shrux : ClangBuiltin<"__insn_shrux">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shruxi : ClangBuiltin<"__insn_shruxi">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_shufflebytes1 : ClangBuiltin<"__insn_shufflebytes1">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_st : ClangBuiltin<"__insn_st">,
  Intrinsic<[], [llvm_ptr_ty, llvm_i64_ty], [IntrWriteMem, IntrArgMemOnly]>;
def int_tilegx_st1 : ClangBuiltin<"__insn_st1">,
  Intrinsic<[], [llvm_ptr_ty, llvm_i64_ty], [IntrWriteMem, IntrArgMemOnly]>;
def int_tilegx_st2 : ClangBuiltin<"__insn_st2">,
  Intrinsic<[], [llvm_ptr_ty, llvm_i64_ty], [IntrWriteMem, IntrArgMemOnly]>;
def int_tilegx_st4 : ClangBuiltin<"__insn_st4">,
  Intrinsic<[], [llvm_ptr_ty, llvm_i64_ty], [IntrWriteMem, IntrArgMemOnly]>;
def int_tilegx_stnt : ClangBuiltin<"__insn_stnt">,
  Intrinsic<[], [llvm_ptr_ty, llvm_i64_ty], [IntrWriteMem, IntrArgMemOnly]>;
def int_tilegx_stnt1 : ClangBuiltin<"__insn_stnt1">,
  Intrinsic<[], [llvm_ptr_ty, llvm_i64_ty], [IntrWriteMem, IntrArgMemOnly]>;
def int_tilegx_stnt2 : ClangBuiltin<"__insn_stnt2">,
  Intrinsic<[], [llvm_ptr_ty, llvm_i64_ty], [IntrWriteMem, IntrArgMemOnly]>;
def int_tilegx_stnt4 : ClangBuiltin<"__insn_stnt4">,
  Intrinsic<[], [llvm_ptr_ty, llvm_i64_ty], [IntrWriteMem, IntrArgMemOnly]>;
def int_tilegx_sub : ClangBuiltin<"__insn_sub">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_subx : ClangBuiltin<"__insn_subx">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_subxsc : ClangBuiltin<"__insn_subxsc">,
  Intrinsic<[llvm_i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
def int_tilegx_tblidxb0 : ClangBuiltin<"__insn_tblidxb0">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_tblidxb1 : ClangBuiltin<"__insn_tblidxb1">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_tblidxb2 : ClangBuiltin<"__insn_tblidxb2">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_tblidxb3 : ClangBuiltin<"__insn_tblidxb3">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_xor : ClangBuiltin<"__insn_xor">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
def int_tilegx_xori : ClangBuiltin<"__insn_xori">,
  Intrinsic<[llvm_i64_ty], [llvm_i64_ty, llvm_i64_ty], [IntrNoMem]>;
} // end TargetPrefix
