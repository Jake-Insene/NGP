/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/Lexer.h"
#include <unordered_map>
#include <string>

struct SourceFile
{
    SourceFile()
    {}

    SourceFile(SourceFile&&) = default;
    SourceFile& operator=(SourceFile&&) = default;

    ~SourceFile()
    {
        delete[] source_code;
    }

    std::string file_path;

    u8* source_code = nullptr;
    u32 source_len = 0;
    u32 index = 0;
};

using SourceFileList = std::vector<SourceFile>;

struct PreProcessor
{
    PreProcessor()
    {}
    ~PreProcessor()
    {}

    void process(const char* file_path);

    void process_source();
    void process_directive();

    void advance();
    bool expected(TokenType type, const char* format, ...);

    Lexer lexer;

    Token last;
    Token current;
    Token next;

    SourceFileList sources;
    TokenList tokens;
};
