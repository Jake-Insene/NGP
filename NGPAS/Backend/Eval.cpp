// --------------------
// Eval.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "Backend/Assembler.h"
#include "ErrorManager.h"

struct ParseFn {
    Token(Assembler::* prefix)(Token, Token);
    Token(Assembler::* infix)(Token, Token);

    ParsePrecedence precedence;
};

static inline ParseFn rules[TOKEN_COUNT] = {
    {}, // ERROR
    {}, // END OF FILE
    {&Assembler::parseImmediate, nullptr, ParsePrecedence::None},
    {&Assembler::parseString, nullptr, ParsePrecedence::None},
    {}, // .directive
    {}, // inst
    {}, // label:
    {&Assembler::parseSymbol, nullptr, ParsePrecedence::None},

    {}, // \n

    {&Assembler::parseGroup, nullptr, ParsePrecedence::None},
    {}, // )
    {}, // =
    {}, // ==
    {}, // !=
    {}, // <
    {}, // <=
    {}, // >
    {}, // >=
    {}, // $
    {}, // Comma
    {}, // [
    {}, // ]

    {nullptr, &Assembler::parseAdd, ParsePrecedence::Term},
    {&Assembler::parseMinus, &Assembler::parseSub, ParsePrecedence::Term},
    {nullptr, &Assembler::parseMul, ParsePrecedence::Term},
    {nullptr, &Assembler::parseDiv, ParsePrecedence::Factor},
    
    {nullptr, &Assembler::parseXor, ParsePrecedence::BitwiseXor},
    {&Assembler::parseNot, nullptr, ParsePrecedence::None},
    {nullptr, &Assembler::parseAnd, ParsePrecedence::BitwiseAnd},
    {nullptr, &Assembler::parseOr, ParsePrecedence::BitwiseOr},
    {nullptr, &Assembler::parseShl, ParsePrecedence::Shift},
    {nullptr, &Assembler::parseShr, ParsePrecedence::Shift},
    {}, // Asr
    {nullptr, &Assembler::parseAsr, ParsePrecedence::Shift},
    {&Assembler::parseRegister, nullptr, ParsePrecedence::None},
};

static inline ParseFn getRule(TokenType type) {
    return rules[type];
}

// Prefix
#define CHECK_IMMEDIATE(tk) \
    if(tk.type != TOKEN_IMMEDIATE) {\
        MAKE_ERROR(tk, return tk, "invalid expresion");\
    }

Token Assembler::parseMinus(Token, Token) {
    Token tk = parseExpresion(ParsePrecedence::Unary);
    tk.i = -tk.i;
    return tk;
}

Token Assembler::parseNot(Token, Token) {
    Token tk = parseExpresion(ParsePrecedence::Unary);
    tk.u = ~tk.u;
    return tk;
}

Token Assembler::parseImmediate(Token, Token) {
    return *last;
}

Token Assembler::parseSymbol(Token, Token) {
    auto it = findLabel(last->str);
    if (it != symbols.end()) {
        if (it->second.ivalue == -1) {
            context.undefined_label = true;
        }

        return Token{
            .source_file = last->source_file,
            .line = last->line,
            .type = TOKEN_IMMEDIATE,
            .u = it->second.uvalue,
        };
    }

    ErrorManager::error(
        last->source_file, last->line, 
        "undefined reference to %.*s", last->str.size(), last->str.data()
    );
    context.unknown_label = true;
    return Token{
        nullptr,
        0,
        TOKEN_ERROR,
    };
}

Token Assembler::parseRegister(Token, Token) {
    return *last;
}

Token Assembler::parseGroup(Token, Token) {
    Token result = parseExpresion(ParsePrecedence::Start);
    expected(TOKEN_RIGHT_PARENT, "')' was expected");
    return result;
}

Token Assembler::parseString(Token, Token) {
    return *last;
}

// Infix

#define CHECK_INFIX() \
    ParseFn rule = getRule(last->type);\
    rhs = parseExpresion(ParsePrecedence(u32(rule.precedence) + 1));\
    CHECK_IMMEDIATE(lhs);\
    CHECK_IMMEDIATE(rhs);\

#define OP_IN_CASE(op) \
        lhs.i = lhs.i op rhs.i;

#define OP_IN_CASE_BIN(field, op) \
        lhs.field = lhs.field op rhs.field;\

Token Assembler::parseAdd(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE(+);
    return lhs;
}

Token Assembler::parseSub(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE(-);
    return lhs;
}

Token Assembler::parseAnd(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, &);
    return lhs;
}

Token Assembler::parseOr(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, |);
    return lhs;
}

Token Assembler::parseXor(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, ^);
    return lhs;
}

Token Assembler::parseShl(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, <<);
    return lhs;
}

Token Assembler::parseShr(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, >>);
    return lhs;
}

Token Assembler::parseAsr(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE_BIN(i, >>);
    return lhs;
}

Token Assembler::parseMul(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE(*);
    return lhs;
}

Token Assembler::parseDiv(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE(/);
    return lhs;
}

Token Assembler::parseExpresion(ParsePrecedence precedence) {
    ParseFn rule = getRule(current->type);
    advance();

    Token result = {};
    if (rule.prefix) {
        result = (this->*rule.prefix)(Token{}, Token{});
    }
    else         {
        MAKE_ERROR((*last), return result, "invalid expresion");
    }

    while (u32(precedence) <= u32((rule = getRule(current->type)).precedence)) {
        advance();
        result = (this->*rule.infix)(result, {});
    }

    return result;
}
