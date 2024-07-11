#include "Backend/Assembler.h"

Token Assembler::parse_expresion(ParsePrecedence precedence) {
    Token result = *current;
    advance();

    return result;
}
