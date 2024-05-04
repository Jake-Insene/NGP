#pragma once
#include "SymbolTable.h"
#include "Frontend/Lexer.h"

struct Register {
    u8 is_float : 4;
    u8 index : 4;
};

struct Instruction {
    TokenInstruction type;
    Register dest;
    Register src1;
    Register src2;

    u32 imm;

    const char* source_file;
    u32 line;
    u32 column;

    u32 address;
    u32 encoded;

    std::string_view symbol;

    [[nodiscard]] constexpr bool is_branch() {
        return
            type == TI_B ||
            type == TI_BEQ ||
            type == TI_BEZ ||
            type == TI_BNE ||
            type == TI_BNZ ||
            type == TI_BLT ||
            type == TI_BLE ||
            type == TI_BGT ||
            type == TI_BGE;
    }

    [[nodiscard]] static constexpr Instruction mov(Register dest, Register src) {
        return Instruction(TI_MOV, dest, src);
    }

    [[nodiscard]] static constexpr Instruction movi(Register dest, u16 imm) {
        return Instruction(TI_MOV, dest, {}, {}, imm);
    }

    [[nodiscard]] static constexpr Instruction ret() {
        return Instruction(TI_RET);
    }

};
