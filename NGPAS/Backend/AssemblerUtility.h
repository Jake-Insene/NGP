#pragma once
#include <FileFormat/ISA.h>

[[nodiscard]] static constexpr u32 one_arg(u8 opcode, u32 arg) {
    return u32(opcode | (arg << 6));
}

[[nodiscard]] static constexpr u32 call(i32 disp) {
    return one_arg(NGP_CALL, disp);
}

[[nodiscard]] static constexpr u32 b(i32 disp) {
    return one_arg(NGP_BRANCH, disp);
}

[[nodiscard]] static constexpr u32 swi(u32 code) {
    return one_arg(NGP_SWI, code);
}

[[nodiscard]] static constexpr u32 bcond(u8 cond, i32 disp) {
    return u32(NGP_BRANCH_COND | (cond << 6) | (disp << 10));
}

[[nodiscard]] static constexpr u32 pcrel(u8 opcode, u8 dest, i32 disp) {
    return u32(opcode | (dest << 6) | (disp << 11));
}

// Binary

[[nodiscard]] static constexpr u32 binary(u16 opcode, u8 dest, u8 src1, u8 src2, u8 src3) {
    return u32(
        NGP_BINARY
        | (dest << 6)
        | (src1 << 11)
        | (src2 << 16)
        | (src3 << 21)
        | (opcode << 26)
    );
}

[[nodiscard]] static constexpr u32 fbinary(u16 opcode, u8 dest, u8 src1, u8 src2, u8 src3) {
    return u32(
        NGP_FBINARY 
        | (dest << 6) 
        | (src1 << 11) 
        | (src2 << 16) 
        | (src3 << 21)
        | (opcode << 26)
    );
}

[[nodiscard]] static constexpr u32 binaryi(u8 opcode, u8 dest, u8 src1, u16 imm) {
    return u32(opcode | (dest << 6) | (src1 << 11) | (imm<<16));
}

// Memory Immediate
[[nodiscard]] static constexpr u32 memoryi(u8 opcode, u8 dest_src, u8 base, u16 offset, u8 sub) {
    return u32(
        NGP_MEMORY_IMMEDIATE 
        | (dest_src << 6) 
        | (base << 11) 
        | (opcode << 16) 
        | (sub << 19) 
        | (offset<<20)
    );
}

[[nodiscard]] static constexpr u32 fmemoryi(u8 opcode, u8 dest_src, u8 base, u16 offset, u8 add_sub) {
    return u32(
        NGP_FMEMORY_IMMEDIATE
        | (dest_src << 6) | (base << 11)
        | (opcode << 16) | (add_sub << 19)
        | (offset << 20)
    );
}

// Immediate
[[nodiscard]] static constexpr u32 iimmediate(u8 dest, u8 opcode, u16 immediate) {
    return u32(NGP_IMMEDIATE | (dest << 6) | (opcode << 11) | (immediate << 16));
}

[[nodiscard]] static constexpr u32 cmpi(u8 dest, u16 immediate) {
    return iimmediate(dest, NGP_CMP_IMMEDIATE, immediate);
}

[[nodiscard]] static constexpr u32 movi(u8 dest, u16 immediate) {
    return iimmediate(dest, NGP_MOV_IMMEDIATE, immediate);
}

[[nodiscard]] static constexpr u32 movt(u8 dest, u16 immediate) {
    return iimmediate(dest, NGP_MOVT_IMMEDIATE, immediate);
}

[[nodiscard]] static constexpr u32 ldpc(u8 dest, u8 opcode, i32 disp) {
    return pcrel(opcode, dest, disp);
}

[[nodiscard]] static constexpr u32 adr(u8 dest, i32 disp) {
    return pcrel(NGP_ADR_PC, dest, disp);
}
