/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"
#include <string_view>

enum TokenType : u8 {
    TOKEN_ERROR,
    TOKEN_END_OF_FILE,

    TOKEN_IMMEDIATE,
    TOKEN_STRING,

    TOKEN_DIRECTIVE,
    TOKEN_INSTRUCTION,
    TOKEN_LABEL,
    TOKEN_SYMBOL,

    TOKEN_NEW_LINE,

    TOKEN_LEFT_PARENT,
    TOKEN_RIGHT_PARENT,
    TOKEN_EQUAL,
    TOKEN_EQUALEQUAL,
    TOKEN_NOTEQUAL,
    TOKEN_LESS,
    TOKEN_LESSEQUAL,
    TOKEN_GREATER,
    TOKEN_GREATEREQUAL,
    TOKEN_DOLLAR,
    TOKEN_COMMA,
    TOKEN_LEFT_KEY,
    TOKEN_RIGHT_KEY,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_XOR,
    TOKEN_NOT,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_SHL,
    TOKEN_SHR,
    TOKEN_ROR,
    TOKEN_ASR,

    TOKEN_REGISTER,

    TOKEN_COUNT,
};

enum TokenRegister : u8
{
    TOKEN_R0 = 0,
    TOKEN_R1,
    TOKEN_R2,
    TOKEN_R3,
    TOKEN_R4,
    TOKEN_R5,
    TOKEN_R6,
    TOKEN_R7,
    TOKEN_R8,
    TOKEN_R9,
    TOKEN_R10,
    TOKEN_R11,
    TOKEN_R12,
    TOKEN_R13,
    TOKEN_R14,
    TOKEN_R15,
    TOKEN_R16,
    TOKEN_R17,
    TOKEN_R18,
    TOKEN_R19,
    TOKEN_R20,
    TOKEN_R21,
    TOKEN_R22,
    TOKEN_R23,
    TOKEN_R24,
    TOKEN_R25,
    TOKEN_R26,
    TOKEN_R27,
    TOKEN_R28,
    TOKEN_R29,
    TOKEN_R30,
    TOKEN_R31,

    TOKEN_S0,
    TOKEN_S1,
    TOKEN_S2,
    TOKEN_S3,
    TOKEN_S4,
    TOKEN_S5,
    TOKEN_S6,
    TOKEN_S7,
    TOKEN_S8,
    TOKEN_S9,
    TOKEN_S10,
    TOKEN_S11,
    TOKEN_S12,
    TOKEN_S13,
    TOKEN_S14,
    TOKEN_S15,
    TOKEN_S16,
    TOKEN_S17,
    TOKEN_S18,
    TOKEN_S19,
    TOKEN_S20,
    TOKEN_S21,
    TOKEN_S22,
    TOKEN_S23,
    TOKEN_S24,
    TOKEN_S25,
    TOKEN_S26,
    TOKEN_S27,
    TOKEN_S28,
    TOKEN_S29,
    TOKEN_S30,
    TOKEN_S31,

    TOKEN_D0,
    TOKEN_D1,
    TOKEN_D2,
    TOKEN_D3,
    TOKEN_D4,
    TOKEN_D5,
    TOKEN_D6,
    TOKEN_D7,
    TOKEN_D8,
    TOKEN_D9,
    TOKEN_D10,
    TOKEN_D11,
    TOKEN_D12,
    TOKEN_D13,
    TOKEN_D14,
    TOKEN_D15,
    TOKEN_D16,
    TOKEN_D17,
    TOKEN_D18,
    TOKEN_D19,
    TOKEN_D20,
    TOKEN_D21,
    TOKEN_D22,
    TOKEN_D23,
    TOKEN_D24,
    TOKEN_D25,
    TOKEN_D26,
    TOKEN_D27,
    TOKEN_D28,
    TOKEN_D29,
    TOKEN_D30,
    TOKEN_D31,

    TOKEN_Q0,
    TOKEN_Q1,
    TOKEN_Q2,
    TOKEN_Q3,
    TOKEN_Q4,
    TOKEN_Q5,
    TOKEN_Q6,
    TOKEN_Q7,
    TOKEN_Q8,
    TOKEN_Q9,
    TOKEN_Q10,
    TOKEN_Q11,
    TOKEN_Q12,
    TOKEN_Q13,
    TOKEN_Q14,
    TOKEN_Q15,
    TOKEN_Q16,
    TOKEN_Q17,
    TOKEN_Q18,
    TOKEN_Q19,
    TOKEN_Q20,
    TOKEN_Q21,
    TOKEN_Q22,
    TOKEN_Q23,
    TOKEN_Q24,
    TOKEN_Q25,
    TOKEN_Q26,
    TOKEN_Q27,
    TOKEN_Q28,
    TOKEN_Q29,
    TOKEN_Q30,
    TOKEN_Q31,
};

enum TokenDirective : u8
{
    TD_FORMAT,
    TD_FORMAT_RAW,
    TD_FORMAT_ROM,
    TD_AS,
    TD_ORG,
    TD_INCLUDE,
    TD_STRING,
    TD_BYTE,
    TD_HALF,
    TD_WORD,
    TD_DWORD,
    TD_ZERO,
    TD_SPACE,
};

enum TokenInstruction : u8
{
    TI_NOP,

    TI_BL,
    TI_B,
    TI_ADR,

    TI_BEQ,
    TI_BEZ = TI_BEQ,
    TI_BNE,
    TI_BNZ = TI_BNE,
    TI_BLT,
    TI_BLE,
    TI_BGT,
    TI_BGE,
    TI_BCS,
    TI_BCC,
    TI_BMI,
    TI_BPL,
    TI_BVS,
    TI_BVC,
    TI_BHI,
    TI_BLS,
    TI_BAL,

    TI_ADD,
    TI_ADDS,
    TI_SUB,
    TI_SUBS,

    TI_ADC,
    TI_ADCS,
    TI_SBC,
    TI_SBCS,

    TI_AND,
    TI_ANDS,
    TI_OR,
    TI_ORN,
    TI_EOR,
    TI_SHL,
    TI_SHR,
    TI_ASR,
    TI_ROR,

    TI_BIC,
    TI_BICS,

    TI_CMP,
    TI_CMN,
    TI_TST,
    TI_NOT,
    TI_NEG,
    TI_ABS,

    TI_FMOV,
    TI_FMOVNC,
    TI_FCVTZS,
    TI_FCVTZU,
    TI_SCVTF,
    TI_UCVTF,

    TI_FADD,
    TI_FSUB,
    TI_FMUL,
    TI_FDIV,

    TI_FNEG,
    TI_FABS,

    TI_LDP,
    TI_LD,
    TI_LDH,
    TI_LDSH,
    TI_LDB,
    TI_LDSB,

    TI_STP,
    TI_ST,
    TI_STH,
    TI_STB,

    TI_TBZ,
    TI_TBNZ,

    TI_CBZ,
    TI_CBNZ,

    TI_MADD,
    TI_MSUB,
    TI_MUL,
    TI_DIV,
    TI_UDIV,

    TI_MOV,
    TI_MOVT,
    TI_MVN,

    TI_RET,

    TI_BLR,
    TI_BR,

    TI_ERET,
    TI_BRK,
    TI_SVC,
    TI_EVC,
    TI_SMC,
    TI_HALT,
};

struct Token
{
    const char* source_file = nullptr;
    u32 line = 0;
    TokenType type = TOKEN_END_OF_FILE;
    u8 subtype = 0;

    std::string_view str;

    union
    {
        u8 byte[8] = {};
        u16 ishort[4];
        u16 ushort[4];
        i32 iword;
        u32 uword;
        i64 i;
        u64 u;
        f32 s;
        f64 d;
    };

    [[nodiscard]] constexpr bool is(TokenType tk) const
    {
        return type == tk;
    }

    [[nodiscard]] constexpr bool is_one_of(TokenType tk1, TokenType tk2) const
    {
        return type == tk1 || type == tk2;
    }

    [[nodiscard]] constexpr bool is_fpreg() const
    {
        return is_single_reg() || is_double_reg() || is_qword_reg();
    }

    [[nodiscard]] constexpr bool is_single_reg() const
    {
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_S0 && subtype <= TOKEN_S31);
    }

    [[nodiscard]] constexpr bool is_double_reg() const
    {
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_D0 && subtype <= TOKEN_D31);
    }

    [[nodiscard]] constexpr bool is_qword_reg() const
    {
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_Q0 && subtype <= TOKEN_Q31);
    }

};

