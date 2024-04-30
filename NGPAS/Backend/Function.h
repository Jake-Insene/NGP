#pragma once
#include "SymbolTable.h"
#include "Frontend/Lexer.h"

enum RegisterSize {
    REG_DWORD = 0,
    REG_WORD,
    REG_QWORD,
};

struct Register {
    u8 is_float : 1;
    u8 size : 2;
    u8 index : 4;
};

struct Instruction {
    TokenInstruction type;

    Register dest;
    Register src1;
    Register src2;

    u32 Imm;

    [[nodiscard]] static constexpr Instruction mov(Register dest, Register src) {
        return Instruction(TI_MOV, dest, src);
    }

    [[nodiscard]] static constexpr Instruction movi(Register dest, u16 imm) {
        return Instruction(TI_MOV, dest, {}, {}, imm);
    }

    [[nodiscard]] static constexpr Instruction ret(Register src) {
        return Instruction(TI_RET, {}, src);
    }

    [[nodiscard]] static constexpr Instruction retv() {
        return Instruction(TI_RET_VOID);
    }
};

struct Function {
    std::string_view symbol;
    u32 address;
    u8 locals;
    u8 arguments;

    std::vector<Instruction> body;
    SymbolTable<u32> labels;
};
