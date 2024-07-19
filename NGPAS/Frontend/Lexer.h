// --------------------
// Lexer.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "Frontend/Token.h"
#include <vector>

using TokenList = std::vector<Token>;

struct Lexer {
    void set(const char* source_file, u8* ctn, u32 s);
    Token getNext();

    void skipWhiteSpace();
    char peek(u8 offset);
    void advance();

    Token getSymbolOrLabel();
    Token getImmediate();
    Token getString();

    bool isAlpha(u8 c) const;
    bool isNum(u8 c) const;
    bool isAlnum(u8 c) const;
    bool isHex(u8 c) const;

    const char* file_path = nullptr;
    u8* content = nullptr;
    u32 size = 0;

    char current = 0;
    u32 line = 0;
    u32 index = 0;
};
