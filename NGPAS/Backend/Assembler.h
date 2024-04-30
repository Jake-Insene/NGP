#pragma once
#include "Frontend/Lexer.h"
#include "Backend/Function.h"
#include "SymbolTable.h"

struct Constant {
    i64 i;
    u64 u;
    f32 f;
    f64 d;
};

struct Assembler {
    Assembler() : lexer() {}
    ~Assembler() {}

    i32 assemble_file(const char* file_path, const char* output_path);

    void assemble_program();
    void assemble_directive();
    void assemble_function();

    void assemble_function_directives(Function& fn);
    void assemble_instruction(Function& fn);

    void advance();
    void syncronize();
    bool expected(TokenType tk, const char* format, ...);

    // Utility
    Register get_register(Token tk);

    struct {
        bool is_in_function;
    } Context;

    Lexer lexer;

    Token last;
    Token current;
    Token next;

    SymbolTable<Constant> constants;
    SymbolTable<Function> functions;
    i32 current_status;

    std::string_view entry_point;
    u16 major;
    u16 minor;
};
