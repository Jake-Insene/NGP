#pragma once
#include "Frontend/Parser.h"

struct Assembler {
    Assembler() : parser() {}
    ~Assembler() {}

    void assemble_file(const char* file_path);
    void assemble_program();

    Parser parser;

    SymbolTable<Function> functions;
    Program program;
};
