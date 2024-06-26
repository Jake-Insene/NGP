#pragma once
#include "Header.h"
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
    TOKEN_COMMA,
    TOKEN_LEFT_KEY,
    TOKEN_RIGHT_KEY,

    TOKEN_REGISTER,
};

enum TokenRegister : u8 {
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
};

enum TokenDirective : u8 {
    TD_ENTRY,
    TD_STRING,
    TD_BYTE,
    TD_HALF,
    TD_WORD,
    TD_DWORD,
    TD_ZERO,
};

enum TokenInstruction : u8 {
    TI_MOV,
    TI_MOVT,

    TI_ADD,
    TI_SUB,
    TI_MUL,
    TI_UMUL,
    TI_DIV,
    TI_UDIV,

    TI_AND,
    TI_OR,
    TI_EOR,
    TI_SHL,
    TI_SHR,

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

    TI_LD,
    TI_LDH,
    TI_LDSH,
    TI_LDB,
    TI_LDSB,

    TI_ST,
    TI_STH,
    TI_STB,

    TI_CMP,
    TI_NOT,
    TI_NEG,
    TI_ABS,

    TI_BEQ,
    TI_BEZ = TI_BEQ,
    TI_BNE,
    TI_BNZ = TI_BNE,
    TI_BLT,
    TI_BLE,
    TI_BGT,
    TI_BGE,
    TI_BCS,
    TI_BNC,
    TI_BSS,
    TI_BNS,
    TI_BOS,
    TI_BNO,
    TI_BHI,
    TI_BLS,

    TI_CALL,
    TI_B,
    TI_SWI,
    TI_RET,
    TI_HALT,

    TI_ADR,
};

struct Token {
    TokenType type = TOKEN_ERROR;
    u8 subtype = 0;
    const char* source_file = nullptr;
    u32 line = 0;
    u32 column = 0;

    std::string_view str = {};
    union {
        u16 ishort[2] = {};
        u16 ushort[2];
        i32 i;
        u32 u;
        f32 single;
    };

    [[nodiscard]] constexpr bool is(TokenType tk) const { return type == tk; }
    [[nodiscard]] constexpr bool is_one_of(TokenType tk1, TokenType tk2) const { return type == tk1 || type == tk2; }

    [[nodiscard]] constexpr bool is_not(TokenType tk) const { return type != tk; }

    [[nodiscard]] constexpr bool is_single() const 
    { 
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_S0 && subtype <= TOKEN_S31); 
    }

    [[nodiscard]] constexpr bool is_double() const
    {
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_D0 && subtype <= TOKEN_D31);
    }

};

struct Lexer {
    Lexer() {}
    ~Lexer() {}

    void set(const char* source_file, u8* ctn, u32 s);
    Token get_next();

    void skip_white_space();
    char peek(u8 offset);
    void advance();

    Token get_directive();
    Token get_symbol_or_label();
    Token get_immediate();
    Token get_string();

    bool is_alpha() const;
    bool is_num() const;
    bool is_alnum() const;
    bool is_hex() const;

    const char* file_path = nullptr;
    u8* content = nullptr;
    u32 size = 0;

    char current = 0;
    u32 line = 0;
    u32 column = 0;
    u32 index = 0;
};
