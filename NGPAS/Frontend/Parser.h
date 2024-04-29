#pragma once
#include "Header.h"
#include "Frontend/Lexer.h"
#include <vector>
#include <string_view>

enum NodeType {
    NODE_UNKNOWN = 0,
    NODE_DIRECTIVE,
    NODE_FUNCTION,
    NODE_INSTRUCTION,
};

struct Node {
    NodeType type;
};

struct NodeDirective : Node {
    union {
        u64 constant;
        i64 iconstant;
        f32 f;
        f64 d;
    };
    std::string_view str;
};

struct Program {
    std::vector<Node*> nodes;

    Program() {}
    ~Program() {
        for (auto& node : nodes) {
            if (node) delete node;
        }
    }
};

struct Parser {
    Parser() : lexer() {}
    ~Parser() {}

    Program parse_file(const char* parse_file);
    void advance();

    Lexer lexer;

    Token last;
    Token current;
    Token next;

};

