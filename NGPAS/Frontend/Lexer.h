#pragma once
#include "Header.h"
#include <string_view>

enum TokenType {
    TOKEN_END_OF_FILE,

    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,

    TOKEN_DIRECTIVE_VERSION,
    TOKEN_DIRECTIVE_ENTRY_POINT,
    TOKEN_DIRECTIVE_FUNCTION,
    TOKEN_FUNCTION_ARGS,
    TOKEN_FUNCTION_LOCALS,

    TOKEN_INSTRUCTION,
};

enum TokenInstruction {
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
};

struct Token {
    TokenType type;
    TokenInstruction inst;

    std::string_view str;
    union {
        u64 u;
        i64 i;
        f32 f;
        f64 d;
    };
};

struct Lexer {
    Lexer() {}
    ~Lexer() {}

    void set(const char* file_path);

    Token get_next();

    const char* file_path;
    u8* content;
    u64 size;

    u64 line;
    u64 index;
};
