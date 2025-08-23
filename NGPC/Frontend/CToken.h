/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "StringPool.h"


enum CTokenType
{
    TOKEN_ERROR,
    TOKEN_END_OF_FILE,

    TOKEN_STRING,
    TOKEN_NUMBER,

    TOKEN_DIRECTIVE,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,

    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_LEFT_PARENT,
    TOKEN_RIGHT_PARENT,
    TOKEN_LEFT_KEY,
    TOKEN_RIGHT_KEY,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_EQUAL,
    TOKEN_EQUALEQUAL,
    TOKEN_NOTEQUAL,
    TOKEN_GREATEREQUAL,
    TOKEN_GREATER,
    TOKEN_LESSEQUAL,
    TOKEN_LESS,
    TOKEN_OR,
    TOKEN_AND,
    TOKEN_NOT,
    TOKEN_XOR,
    TOKEN_SHR,
    TOKEN_SHL,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
};

enum CTokenNumberType
{
    TN_BYTE,
    TN_SHORT,
    TN_INT,
    TN_LONG,
    TN_UNSIGNED,
    TN_FLOAT,
    TN_DOUBLE,
};

enum CTokenKeyword
{
    TK_VOID,
    TK_CHAR,
    TK_SHORT,
    TK_INT,
    TK_LONG,
    TK_UNSIGNED,
    TK_SIGNED,
    TK_FLOAT,
    TK_DOUBLE,

    TK_CONST,
    TK_STATIC,
    TK_INLINE,
    TK_STRUCT,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_FOR,
    TK_WHILE,
    TK_DO,
    TK_GOTO,
    TK_TYPEDEF,
};

enum CTokenDirective
{
    TD_DEFINE,
    TD_INCLUDE,
};

struct CToken
{
	CTokenType type;
	StringID source_file;
	u32 line;
    u8 subtype;

    StringID str;

    union
    {
        u8 byte[8];
        u16 ishort[4];
        u16 ushort[4];
        i32 iword;
        u32 uword;
        i64 i;
        u64 u;
        f32 s;
        f64 d;
    };

    [[nodiscard]] std::string_view get_source_file() const { return StringPool::get(source_file); }
    [[nodiscard]] std::string_view get_str() const { return StringPool::get(str); }

    [[nodiscard]] constexpr bool is(CTokenType tk) const
    {
        return type == tk;
    }

    [[nodiscard]] constexpr bool is_one_of(CTokenType tk1, CTokenType tk2) const
    {
        return type == tk1 || type == tk2;
    }

    [[nodiscard]] constexpr bool is_primitive_type() const
    {
        return is(TOKEN_KEYWORD) && subtype >= TK_VOID && subtype <= TK_DOUBLE;
    }

    [[nodiscard]] constexpr bool is_storage_mod() const
    {
        return is(TOKEN_KEYWORD) 
            && (subtype == TK_STATIC || subtype == TK_INLINE || subtype == TK_CONST);
    }

    [[nodiscard]] constexpr bool is_sign_specifier() const
    {
        return is(TOKEN_KEYWORD)
            && (subtype == TK_SIGNED || subtype == TK_UNSIGNED);
    }
};