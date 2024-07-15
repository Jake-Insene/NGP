#pragma once
#include "Frontend/Token.h"
#include <FileFormat/ISA.h>

[[nodiscard]] static constexpr u32 oneArg(u8 opcode, u32 arg) {
    return u32(opcode | (arg << 6));
}

[[nodiscard]] static constexpr u32 bl(i32 disp) {
    return oneArg(NGP_BL, disp);
}

[[nodiscard]] static constexpr u32 b(i32 disp) {
    return oneArg(NGP_BRANCH, disp);
}

[[nodiscard]] static constexpr u32 swi(u32 code) {
    return oneArg(NGP_SWI, code);
}

[[nodiscard]] static constexpr u32 bcond(u8 cond, i32 disp) {
    return u32(NGP_BRANCH_COND | (cond << 6) | (disp << 10));
}

[[nodiscard]] static constexpr u32 pcrel(u8 opcode, u8 dest, i32 disp) {
    return u32(opcode | (dest << 6) | (disp << 11));
}

// Binary

[[nodiscard]] static constexpr u32 logicalAddSub(u8 opcode, u8 dest, u8 src1, u8 src2, u8 src3) {
    return u32(
        NGP_LOGICAL_ADD_SUB
        | (opcode << 6)
        | (dest << 12)
        | (src1 << 17)
        | (src2 << 22)
        | (src3 << 27)
    );
}

[[nodiscard]] static constexpr u32 additional(u8 opcode, u8 dest, u8 src1, u8 src2, u8 src3) {
    return u32(
        NGP_ADDITIONAL
        | (opcode << 6)
        | (dest << 12)
        | (src1 << 17)
        | (src2 << 22)
        | (src3 << 27)
    );
}

[[nodiscard]] static constexpr u32 nonBinary(u8 opcode, u8 dest, u8 src1, u8 src2, u8 src3) {
    return u32(
        NGP_NON_BINARY
        | (opcode << 6)
        | (dest << 12)
        | (src1 << 17)
        | (src2 << 22)
        | (src3 << 27)
    );
}

[[nodiscard]] static constexpr u32 fbinary(u16 opcode, u8 dest, u8 src1, u8 src2, u8 src3) {
    return u32(
        NGP_FBINARY 
        | (opcode << 6)
        | (dest << 12)
        | (src1 << 17)
        | (src2 << 22)
        | (src3 << 27)
    );
}

[[nodiscard]] static constexpr u32 binaryi(u8 opcode, u8 dest, u8 src1, u16 imm) {
    return u32(opcode | (dest << 6) | (src1 << 11) | (imm<<16));
}

// Memory Immediate
[[nodiscard]] static constexpr u32 memoryi(u8 opcode, u8 dest_src, u8 base, u16 offset, u8 sub) {
    return u32(
        NGP_MEMORY_IMMEDIATE 
        | (opcode << 6) 
        | (dest_src << 11)
        | (base << 16) 
        | (sub << 19) 
        | (offset<<20)
    );
}

[[nodiscard]] static constexpr u32 fmemoryi(u8 opcode, u8 dest_src, u8 base, u16 offset, u8 add_sub) {
    return u32(
        NGP_FMEMORY_IMMEDIATE
        | (opcode << 6)
        | (dest_src << 11) | (base << 16)
        | (add_sub << 19)
        | (offset << 20)
    );
}

// Immediate
[[nodiscard]] static constexpr u32 iimmediate(u8 opcode, u8 dest, u16 immediate) {
    return u32(NGP_IMMEDIATE | (opcode << 6) | (dest << 11) | (immediate << 16));
}

// String
inline void encodeString(u8* mem, std::string_view str) {
    u32 i = 0;
    u32 memi = 0;
    while (i < str.size()) {
        switch (str[i]) {
        case '\\':
        {
            i++;
            if (str[i] == '0') {
                mem[memi] = '\0';
                memi++;
                i++;
            }
            else if (str[i] == 'n') {
                mem[memi] = '\n';
                memi++;
                i++;
            }
        }
        break;
        default:
            mem[memi] = str[i];
            memi++;
            i++;
            break;
        }
    }
}
