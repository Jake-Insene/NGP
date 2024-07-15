#include "Backend/Assembler.h"
#include "ErrorManager.h"

struct ParseFn {
    Token(Assembler::* prefix)(Token, Token);
    Token(Assembler::* infix)(Token, Token);

    TokenType tk;
    ParsePrecedence precedence;
};

static inline ParseFn rules[] = {
    {&Assembler::parseMinus, &Assembler::parseSub, TOKEN_MINUS, ParsePrecedence::Term},
    {&Assembler::parseNot, nullptr, TOKEN_NOT, ParsePrecedence::None},
    {&Assembler::parseInteger, nullptr, TOKEN_IMMEDIATE, ParsePrecedence::None},
    {&Assembler::parseSingle, nullptr, TOKEN_IMMEDIATE_SINGLE, ParsePrecedence::None},
    {&Assembler::parseDouble, nullptr, TOKEN_IMMEDIATE_DOUBLE, ParsePrecedence::None},
    {&Assembler::parseSymbol, nullptr, TOKEN_SYMBOL, ParsePrecedence::None},
    {&Assembler::parseRegister, nullptr, TOKEN_REGISTER, ParsePrecedence::None},

    {nullptr, &Assembler::parseAdd, TOKEN_PLUS, ParsePrecedence::Term},
    {nullptr, &Assembler::parseAnd, TOKEN_AND, ParsePrecedence::BitwiseAnd},
    {nullptr, &Assembler::parseOr, TOKEN_OR, ParsePrecedence::BitwiseOr},
    {nullptr, &Assembler::parseXor, TOKEN_XOR, ParsePrecedence::BitwiseXor},
    {nullptr, &Assembler::parseShl, TOKEN_SHL, ParsePrecedence::Shift},
    {nullptr, &Assembler::parseShr, TOKEN_SHR, ParsePrecedence::Shift},
    {nullptr, &Assembler::parseAsr, TOKEN_ASR, ParsePrecedence::Shift},
    {nullptr, &Assembler::parseMul, TOKEN_STAR, ParsePrecedence::Term},
    {nullptr, &Assembler::parseDiv, TOKEN_SLASH, ParsePrecedence::Factor},
};

static inline ParseFn getRule(TokenType type) {
    for (auto& fn : rules) {
        if (fn.tk == type) {
            return fn;
        }
    }

    return {};
}

// Prefix
#define CHECK_IMMEDIATE(tk) \
    if(tk.type != TOKEN_IMMEDIATE && \
        tk.type != TOKEN_IMMEDIATE_SINGLE &&\
        tk.type != TOKEN_IMMEDIATE_DOUBLE) {\
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

Token Assembler::parseInteger(Token, Token) {
    return *last;
}

Token Assembler::parseSingle(Token, Token) {
    return *last;
}

Token Assembler::parseDouble(Token, Token) {
    return *last;
}

Token Assembler::parseSymbol(Token, Token) {
    return *last;
}

Token Assembler::parseRegister(Token, Token) {
    return *last;
}

// Infix

static inline Token convert(Token from, TokenType to) {
    if (to == TOKEN_IMMEDIATE) {
        from.type = TOKEN_IMMEDIATE;
        if (from.is(TOKEN_IMMEDIATE_SINGLE)) {
            from.i = (i64)from.s;
        }
        else if (from.is(TOKEN_IMMEDIATE_DOUBLE)) {
            from.i = (i64)from.d;
        }
    }
    else if(to == TOKEN_IMMEDIATE_SINGLE) {
        from.type = TOKEN_IMMEDIATE_SINGLE;
        if (from.is(TOKEN_IMMEDIATE)) {
            from.s = (f32)from.i;
        }
        else if (from.is(TOKEN_IMMEDIATE_DOUBLE)) {
            from.s = (f32)from.d;
        }
    }
    else if (to == TOKEN_IMMEDIATE_DOUBLE) {
        from.type = TOKEN_IMMEDIATE_DOUBLE;
        if (from.is(TOKEN_IMMEDIATE)) {
            from.d = (f64)from.i;
        }
        else if (from.is(TOKEN_IMMEDIATE_SINGLE)) {
            from.d = (f64)from.s;
        }
    }

    return from;
}

#define CHECK_INFIX() \
    ParseFn rule = getRule(last->type);\
    rhs = parseExpresion(ParsePrecedence(u32(rule.precedence) + 1));\
    if(lhs.type != rhs.type) {\
        rhs = convert(rhs, lhs.type);\
    }\
    CHECK_IMMEDIATE(lhs);\
    CHECK_IMMEDIATE(rhs);\

#define OP_IN_CASE(op) \
    if (lhs.type == TOKEN_IMMEDIATE) {\
        lhs.i = lhs.i op rhs.i;\
    }\
    else if (lhs.type == TOKEN_IMMEDIATE_SINGLE) {\
        lhs.s = lhs.s op rhs.s;\
    }\
    else if (lhs.type == TOKEN_IMMEDIATE_DOUBLE) {\
        lhs.d = lhs.d op rhs.d;\
    }\

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

    while (u32(precedence) <= u32((rule = getRule(current->type)).precedence)) {
        advance();
        result = (this->*rule.infix)(result, {});
    }

    return result;
}
