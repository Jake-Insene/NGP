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

[[nodiscard]] static constexpr u32 sc(u32 code) {
    return one_arg(NGP_SC, code);
}

[[nodiscard]] static constexpr u32 bcond(u8 cond, i32 disp) {
    return u32(NGP_BRANCH_COND | (cond << 6) | (disp << 10));
}

[[nodiscard]] static constexpr u32 pcrel(u8 opcode, u8 dest, i32 disp) {
    return u32(opcode | (dest << 6) | (disp << 11));
}

// Binary

[[nodiscard]] static constexpr u32 binary(u16 opcode, u8 dest, u8 src1, u8 src2) {
    return u32(NGP_BINARY | (dest << 6) | (src1 << 11) | (src2 << 16) | (opcode << 21));
}

[[nodiscard]] static constexpr u32 binaryi(u8 opcode, u8 dest, u8 src1, u16 imm) {
    return u32(opcode | (dest << 6) | (src1 << 11) | (imm<<16));
}

// Immediate
[[nodiscard]] static constexpr u32 iimmediate(u8 dest, u8 opcode, u16 immediate) {
    return u32(NGP_IMMEDIATE | (dest << 6) | (opcode << 11) | (immediate << 16));
}

[[nodiscard]] static constexpr u32 cmpi(u8 dest, u16 immediate) {
    return iimmediate(dest, NGP_CMP_IMMEDIATE, immediate);
}

[[nodiscard]] static constexpr u32 cmpi_shl16(u8 dest, u16 immediate) {
    return iimmediate(dest, NGP_CMP_IMMEDIATE_SHL16, immediate);
}

[[nodiscard]] static constexpr u32 movi(u8 dest, u16 immediate) {
    return iimmediate(dest, NGP_MOV_IMMEDIATE, immediate);
}

[[nodiscard]] static constexpr u32 movt(u8 dest, u16 immediate) {
    return iimmediate(dest, NGP_MOVT_IMMEDIATE, immediate);
}

[[nodiscard]] static constexpr u32 fmovi(u8 dest, u16 immediate) {
    return iimmediate(dest, NGP_FMOV_IMMEDIATE, immediate);
}

[[nodiscard]] static constexpr u32 adr(u8 dest, i32 disp) {
    return pcrel(NGP_ADR_PC, dest, disp);
}
