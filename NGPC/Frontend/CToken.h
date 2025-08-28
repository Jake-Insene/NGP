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

    TOKEN_COUNT,
};

enum CTokenNumberType
{
    TN_BYTE,
    TN_SIGNED,
    TN_UNSIGNED,
    TN_FLOAT,
};

enum CTokenKeyword
{
    TK_VOID,
    TK_I8,
    TK_U8,
    TK_I16,
    TK_U16,
    TK_I32,
    TK_U32,
    TK_F32,

    TK_FUNC,
    TK_VAR,
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
        u8 byte[4];
        u16 ishort[2];
        u16 ushort[2];
        i32 iword;
        u32 uword;
        f32 s;
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
        return is(TOKEN_KEYWORD) && subtype >= TK_VOID && subtype <= TK_F32;
    }

    [[nodiscard]] constexpr bool is_storage_mod() const
    {
        return is(TOKEN_KEYWORD) 
            && (subtype == TK_STATIC || subtype == TK_INLINE || subtype == TK_CONST);
    }

    [[nodiscard]] constexpr bool is_func() const
    {
        return is(TOKEN_KEYWORD)
            && (subtype == TK_FUNC);
    }

    [[nodiscard]] constexpr bool is_var() const
    {
        return is(TOKEN_KEYWORD)
            && (subtype == TK_VAR);
    }

    [[nodiscard]] constexpr bool is_const() const
    {
        return is(TOKEN_KEYWORD)
            && (subtype == TK_CONST);
    }

    [[nodiscard]] constexpr bool is_keyword(CTokenKeyword kw) const
    {
        return is(TOKEN_KEYWORD)
            && (subtype == kw);
    }
};