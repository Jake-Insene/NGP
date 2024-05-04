#pragma once
#include "Header.h"
#include <string_view>

enum TokenType : u8 {
    TOKEN_END_OF_FILE,

    TOKEN_IMMEDIATE,
    TOKEN_STRING,

    TOKEN_DIRECTIVE,
    TOKEN_INSTRUCTION,
    TOKEN_LABEL,
    TOKEN_SYMBOL,

    TOKEN_NEW_LINE,
    TOKEN_COMMA,

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
    TOKEN_SP = TOKEN_R15,

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
};

enum TokenDirective : u8 {
    TD_ENTRY_POINT,
};

enum TokenInstruction : u8 {
    TI_MOV,
    TI_FMOV,

    TI_ADD,
    TI_SUB,
    TI_MUL,
    TI_UMUL,
    TI_DIV,
    TI_UDIV,

    TI_CMP,
    TI_CALL,
    TI_B,
    TI_BEQ,
    TI_BEZ = TI_BEQ,
    TI_BNE,
    TI_BNZ = TI_BNE,
    TI_BLT,
    TI_BLE,
    TI_BGT,
    TI_BGE,
    
    TI_RET,
};

struct Token {
    TokenType type;
    u8 subtype;
    const char* source_file;
    u32 line;
    u32 column;

    std::string_view str;
    union {
        u16 ushort[4];
        u64 u;
        i64 i;
        f32 f;
        f64 d;
    };

    [[nodiscard]] constexpr bool is(TokenType tk) const { return type == tk; }
    [[nodiscard]] constexpr bool is_one_of(TokenType tk1, TokenType tk2) const { return type == tk1 || type == tk2; }

    [[nodiscard]] constexpr bool is_not(TokenType tk) const { return type != tk; }

};

struct Lexer {
    Lexer() {}
    ~Lexer() {}

    void set(const char* source_file, u8* ctn, u32 s);
    Token get_next();

    void skip_white_space();
    void skip_comment();
    char peek(u8 offset);
    void advance();

    Token parse_directive();
    Token parse_symbol_or_label();
    Token parse_immediate();

    bool is_alpha() const;
    bool is_num() const;
    bool is_alnum() const;

    const char* file_path;
    u8* content;
    u32 size;

    char current;
    u32 line;
    u32 column;
    u32 index;
};
