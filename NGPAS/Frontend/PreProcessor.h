#pragma once
#include "Frontend/Lexer.h"
#include <unordered_map>
#include <string>
#include <string_view>

struct Label {
    TokenView symbol;
    u32 address;

    const char* source_file;
    u32 line;
    u32 column;
};

struct SourceScope {
    std::string file_path;
    std::unordered_map<std::string_view, Label> labels;
    TokenList tokens;

    u8* source_code;
    u32 source_len;

    u32 token_index;
    u32 parent_scope_index;
};

using SourceScopeList = std::vector<SourceScope>;

struct PreProcessor {
    PreProcessor() {}
    ~PreProcessor() {}
    
    void process(const char* file_path);

    void process_source();
    void process_directive();

    void advance();
    bool expected(TokenType type, const char* format, ...);

    [[nodiscard]] constexpr SourceScope& global_scope() { return sources[global_scope_index]; }
    [[nodiscard]] constexpr SourceScope& current_scope() { return sources[current_scope_index]; }

    Lexer lexer;

    Token last;
    Token current;
    Token next;

    SourceScopeList sources;
    u32 global_scope_index = 0;
    u32 current_scope_index = 0;
    u32 source_index = 0;
};
