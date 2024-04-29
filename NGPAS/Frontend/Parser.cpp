#include "Frontend/Parser.h"
#include "Frontend/Lexer.h"

Program Parser::parse_file(const char* parse_file)
{
    Program program = {};

    return program;
}

void Parser::advance()
{
    last = current;
    current = next;
    next = lexer.get_next();
}
