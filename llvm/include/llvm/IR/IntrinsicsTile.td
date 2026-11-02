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
} // end TargetPrefix
