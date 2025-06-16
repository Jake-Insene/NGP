/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Backend/Assembler.h"

#include "ErrorManager.h"

struct ParseFn
{
    Token(Assembler::* prefix)(Token, Token);
    Token(Assembler::* infix)(Token, Token);

    ParsePrecedence precedence;
};

static inline ParseFn rules[TOKEN_COUNT] =
{
    {}, // ERROR
    {}, // END OF FILE
    {&Assembler::parse_immediate, nullptr, ParsePrecedence::None},
    {&Assembler::parse_string, nullptr, ParsePrecedence::None},
    {}, // .directive
    {}, // inst
    {}, // label:
    {&Assembler::parse_symbol, nullptr, ParsePrecedence::None},

    {}, // \n

    {&Assembler::parse_group, nullptr, ParsePrecedence::None},
    {}, // )
    {}, // {
    {}, // }
    {}, // =
    {}, // ==
    {}, // !=
    {}, // <
    {}, // <=
    {}, // >
    {}, // >=
    {&Assembler::parse_dollar, nullptr, ParsePrecedence::None}, // $
    {}, // Comma
    {}, // [
    {}, // ]

    {nullptr, &Assembler::parse_add, ParsePrecedence::Term},
    {&Assembler::parse_minus, &Assembler::parse_sub, ParsePrecedence::Term},
    {nullptr, &Assembler::parse_mul, ParsePrecedence::Term},
    {nullptr, &Assembler::parse_div, ParsePrecedence::Factor},

    {nullptr, &Assembler::parse_xor, ParsePrecedence::BitwiseXor},
    {&Assembler::parse_not, nullptr, ParsePrecedence::None},
    {nullptr, &Assembler::parse_and, ParsePrecedence::BitwiseAnd},
    {nullptr, &Assembler::parse_or, ParsePrecedence::BitwiseOr},
    {nullptr, &Assembler::parse_shl, ParsePrecedence::Shift},
    {nullptr, &Assembler::parse_shr, ParsePrecedence::Shift},
    {}, // Asr
    {nullptr, &Assembler::parse_asr, ParsePrecedence::Shift},
    {&Assembler::parse_register, nullptr, ParsePrecedence::None},
};

static inline ParseFn get_rule(TokenType type)
{
    return rules[type];
}

// Prefix

Token Assembler::parse_minus(Token, Token)
{
    Token tk = parse_expression(ParsePrecedence::Unary);
    tk.i = -tk.i;
    return tk;
}

Token Assembler::parse_not(Token, Token)
{
    Token tk = parse_expression(ParsePrecedence::Unary);
    tk.u = ~tk.u;
    return tk;
}

Token Assembler::parse_immediate(Token, Token)
{
    return *last;
}

Token Assembler::parse_symbol(Token, Token)
{
    auto it = find_label(last->str);
    if (it != symbols.end())
    {
        if (it->second.ivalue == -1)
        {
            context.undefined_label = true;
        }

        return Token
        {
            .source_file = last->source_file,
            .line = last->line,
            .type = TOKEN_IMMEDIATE,
            .u = it->second.uvalue,
        };
    }

    if(context.is_in_resolve)
    {
        MAKE_ERROR((*last), {}, "undefined reference to %.*s", last->get_str().size(), last->get_str().data());
    }
    context.undefined_label = true;
    context.unknown_label = true;
    return Token
    {
        current->source_file,
        current->line,
        TOKEN_ERROR,
    };
}

Token Assembler::parse_register(Token, Token)
{
    return *last;
}

Token Assembler::parse_group(Token, Token)
{
    Token result = parse_expression(ParsePrecedence::Start);
    expected(TOKEN_RIGHT_PARENT, "')' was expected");
    return result;
}

Token Assembler::parse_dollar(Token, Token)
{
    return Token{
        .type = TOKEN_IMMEDIATE,
        .u = program_index
    };
}

Token Assembler::parse_string(Token, Token)
{
    return *last;
}

// Infix

#define CHECK_IMMEDIATE(TK) \
    if(TK.type != TOKEN_IMMEDIATE)\
    {\
        MAKE_ERROR(TK, return TK, "invalid expression");\
    }

#define CHECK_INFIX() \
    ParseFn rule = get_rule(last->type);\
    rhs = parse_expression(ParsePrecedence(u32(rule.precedence) + 1));\
    CHECK_IMMEDIATE(lhs);\
    CHECK_IMMEDIATE(rhs);\

#define OP_IN_CASE(OP) \
        lhs.i = lhs.i OP rhs.i;

#define OP_IN_CASE_BIN(field, OP) \
        lhs.field = lhs.field OP rhs.field;\

Token Assembler::parse_add(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE(+);
    return lhs;
}

Token Assembler::parse_sub(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE(-);
    return lhs;
}

Token Assembler::parse_and(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, &);
    return lhs;
}

Token Assembler::parse_or(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, | );
    return lhs;
}

Token Assembler::parse_xor(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, ^);
    return lhs;
}

Token Assembler::parse_shl(Token lhs, Token rhs) {
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, <<);
    return lhs;
}

Token Assembler::parse_shr(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE_BIN(u, >> );
    return lhs;
}

Token Assembler::parse_asr(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE_BIN(i, >> );
    return lhs;
}

Token Assembler::parse_mul(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE(*);
    return lhs;
}

Token Assembler::parse_div(Token lhs, Token rhs)
{
    CHECK_INFIX();
    OP_IN_CASE(/ );
    return lhs;
}

Token Assembler::parse_expression(ParsePrecedence precedence)
{
    ParseFn rule = get_rule(current->type);
    advance();

    Token result = *current;
    if (rule.prefix)
    {
        result = (this->*rule.prefix)(Token(), Token());
    }
    else
    {
        MAKE_ERROR((*last), return result, "invalid expression");
    }

    while (u32(precedence) <= u32((rule = get_rule(current->type)).precedence))
    {
        advance();
        result = (this->*rule.infix)(result, {});
    }

    return result;
}
