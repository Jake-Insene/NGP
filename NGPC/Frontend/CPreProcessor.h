/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/CLexer.h"

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
using AsmTokenList = std::vector<CToken>;

struct MacroDefinition
{
    std::vector<StringID> args_name;

    AsmTokenList tokens;
};

struct CPreProcessor
{
    CLexer lexer;

    CToken last;
    CToken current;
    CToken next;

    SourceFileList sources;
    AsmTokenList tokens;
    std::unordered_map<StringID, MacroDefinition> macros;

	void process(const char* file_path);

    void process_source();
    void process_directive();
    void expand_macro(const MacroDefinition& macro);

    void advance();
    bool expected(CTokenType type, const char* format, ...);
};