/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/AsmLexer.h"
#include <unordered_map>
#include <string>

struct SourceFile
{
    SourceFile() {}

    SourceFile(SourceFile&&) = default;
    SourceFile& operator=(SourceFile&&) = default;

    ~SourceFile() {}

    std::string file_path;
    u8* source_code = nullptr;
    u32 source_len = 0;
    u32 index = 0;
};

using SourceFileList = std::vector<SourceFile>;

struct MacroDefinition
{
    std::vector<std::string> args_name;

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
    std::unordered_map<std::string, MacroDefinition> macros;
};
