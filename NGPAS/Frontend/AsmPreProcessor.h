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

struct MacroDefinition
{
    std::vector<StringID> args_name;

    TokenList tokens;
};

struct AsmPreProcessor
{
    AsmPreProcessor() {}
    ~AsmPreProcessor() {}

    void process(const char* file_path);

    void process_source();
    void process_directive();
    void expand_macro(const MacroDefinition& macro);

    void advance();
    bool expected(TokenType type, const char* format, ...);

    AsmLexer lexer;

    Token last;
    Token current;
    Token next;

    SourceFileList sources;
    TokenList tokens;
    std::unordered_map<StringID, MacroDefinition> macros;
};
