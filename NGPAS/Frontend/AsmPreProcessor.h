/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/AsmLexer.h"
#include "StringPool.h"
#include <unordered_map>
#include <vector>


struct SourceFile
{
    SourceFile() {}
    ~SourceFile() {}

    [[nodiscard]] std::string_view get_file_path() const { return StringPool::get(file_path); }

    StringID file_path;
    u8* source_code = nullptr;
    u32 source_len = 0;
    u32 index = 0;
};

using SourceFileList = std::vector<SourceFile>;
using AsmTokenList = std::vector<AsmToken>;

struct MacroDefinition
{
    std::vector<StringID> args_name;

    AsmTokenList tokens;
};

struct AsmPreProcessor
{
    AsmLexer lexer;

    AsmToken last;
    AsmToken current;
    AsmToken next;

    SourceFileList sources;
    AsmTokenList tokens;
    std::unordered_map<StringID, MacroDefinition> macros;

    void process(const char* file_path);

    void process_source();
    void process_directive();
    void expand_macro(const MacroDefinition& macro);

    void advance();
    bool expected(AsmTokenType type, const char* format, ...);
};
