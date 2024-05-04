#pragma once
#include "Frontend/Lexer.h"
#include "Backend/Instruction.h"
#include "SymbolTable.h"

struct Constant {
    i32 i;
    u32 u;
    f32 f;
};

struct Label {
    std::string_view symbol;
    u32 address;

    const char* source_file;
    u32 line;
    u32 column;
};

struct Assembler {
    Assembler() : lexer() {}
    ~Assembler() {}

    i32 assemble_file(const char* file_path, const char* output_path);

    // first fase
    void assemble_program();
    void assemble_directive();
    void assemble_label();
    void assemble_instruction();

    // second fase
    void resolve_labels();
    void encode();

    void advance();
    void syncronize();
    bool expected(TokenType tk, const char* format, ...);
    void skip_whitespaces();

    // Utility
    Register get_register(Token tk);

    Lexer lexer;

    Token last;
    Token current;
    Token next;

    SymbolTable<Constant> constants;
    SymbolTable<Label> labels;
    std::vector<Instruction> instructions;

    std::string_view entry_point;
    u32 entry_point_address;
    i32 current_status;
};
