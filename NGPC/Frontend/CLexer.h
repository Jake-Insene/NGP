/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/CToken.h"

struct CLexer
{
    StringID file_path;
    u8* content;
    u32 size;

    char current;
    u32 line;
    u32 index;

    [[nodiscard]] std::string_view get_file_path() const { return StringPool::get(file_path); }

    void set(StringID source_file, u8* ctn, u32 s);
    CToken get_next();

    void skip_white_space();
    char peek(u8 offset);
    void advance();

    CToken get_identifier();
    CToken get_number();
    CToken get_string();

    bool is_alpha(u8 c) const;
    bool is_num(u8 c) const;
    bool is_alnum(u8 c) const;
    bool is_hex(u8 c) const;
    bool is_bin(u8 c) const;
};