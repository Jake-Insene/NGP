// --------------------
// PreProcessor.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "Frontend/Lexer.h"
#include <unordered_map>
#include <string>

struct SourceFile {
    SourceFile() {}

    SourceFile(SourceFile&& s) {
        file_path = std::move(s.file_path);
        source_code = s.source_code;
        source_len = s.source_len;
        index = s.index;

        s.file_path = {};
        s.source_code = nullptr;
        s.source_len = 0;
        s.index = u32(-1);
    }

    SourceFile& operator=(SourceFile&& s) {
        file_path = std::move(s.file_path);
        source_code = s.source_code;
        source_len = s.source_len;
        index = s.index;

        s.file_path = {};
        s.source_code = nullptr;
        s.source_len = 0;
        s.index = u32(-1);

        return *this;
    }
    
    ~SourceFile() {
        delete[] source_code;
    }

    std::string file_path;

    u8* source_code;
    u32 source_len;
    u32 index;
};

using SourceFileList = std::vector<SourceFile>;

struct PreProcessor {
    PreProcessor() {}
    ~PreProcessor() {}
    
    void process(const char* file_path);

    void processSource();
    void processDirective();

    void advance();
    bool expected(TokenType type, const char* format, ...);

    Lexer lexer;

    Token last;
    Token current;
    Token next;

    SourceFileList sources;
    TokenList tokens;
};
