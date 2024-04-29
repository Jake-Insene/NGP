#pragma once

#include "SymbolTable.h"

struct Function {
    std::string_view symbol;
    u32 address;
    u8 locals;
    u8 arguments;

    SymbolTable<u32> labels;
};
