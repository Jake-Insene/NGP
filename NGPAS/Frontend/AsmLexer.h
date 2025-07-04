/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/AsmToken.h"


struct AsmLexer
{
    StringID file_path;
    u8* content = nullptr;
    u32 size = 0;

    char current = 0;
    u32 line = 0;
    u32 index = 0;

    [[nodiscard]] std::string_view get_file_path() const { return StringPool::get(file_path); }
    
    void set(StringID source_file, u8* ctn, u32 s);
    AsmToken get_next();

    void skip_white_space();
    char peek(u8 offset);
    void advance();

    AsmToken get_symbol_or_label();
    AsmToken get_immediate();
    AsmToken get_string();

    bool is_alpha(u8 c) const;
    bool is_num(u8 c) const;
    bool is_alnum(u8 c) const;
    bool is_hex(u8 c) const;
    bool is_bin(u8 c) const;
};
