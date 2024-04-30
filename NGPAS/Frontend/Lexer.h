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
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,

    TOKEN_REGISTER,
};

enum TokenRegister : u8 {
    TOKEN_X0 = 0,
    TOKEN_X1,
    TOKEN_X2,
    TOKEN_X3,
    TOKEN_X4,
    TOKEN_X5,
    TOKEN_X6,
    TOKEN_X7,
    TOKEN_X8,
    TOKEN_X9,
    TOKEN_X10,
    TOKEN_X11,
    TOKEN_X12,
    TOKEN_X13,
    TOKEN_X14,
    TOKEN_SP,
    TOKEN_X15 = TOKEN_SP,

    TOKEN_W0,
    TOKEN_W1,
    TOKEN_W2,
    TOKEN_W3,
    TOKEN_W4,
    TOKEN_W5,
    TOKEN_W6,
    TOKEN_W7,
    TOKEN_W8,
    TOKEN_W9,
    TOKEN_W10,
    TOKEN_W11,
    TOKEN_W12,
    TOKEN_W13,
    TOKEN_W14,
    TOKEN_WSP,
    TOKEN_W15 = TOKEN_WSP,

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
};

enum TokenDirective : u8 {
    TD_ENTRY_POINT,
    TD_FUNCTION,
    TD_FUNCTION_ARGS,
    TD_FUNCTION_LOCALS,
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
    
    TI_RET,
    TI_RET_VOID,
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
