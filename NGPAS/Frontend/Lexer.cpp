#include "Frontend/Lexer.h"
#include "ErrorManager.h"

void Lexer::set(const char* source_file, u8* ctn, u32 s)
{
    content = ctn;
    size = s;
    file_path = source_file;
    index = 0;
    line = 1;
    column = 1;
    current = content[index];
}

struct SymbolInfo {
    const char* symbol;
    u8 size;
    TokenType type;
    u8 subtype;
};

SymbolInfo symbols[] = {
    {.symbol = "entry_point", .size = 11, .type = TOKEN_DIRECTIVE, .subtype = TD_ENTRY_POINT },

    {.symbol = "r0", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R0},
    {.symbol = "r1", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R1},
    {.symbol = "r2", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R2},
    {.symbol = "r3", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R3},
    {.symbol = "r4", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R4},
    {.symbol = "r5", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R5},
    {.symbol = "r6", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R6},
    {.symbol = "r7", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R7},
    {.symbol = "r8", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R8},
    {.symbol = "r9", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R9},
    {.symbol = "r10", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R10},
    {.symbol = "r11", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R11},
    {.symbol = "r12", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R12},
    {.symbol = "r13", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R13},
    {.symbol = "r14", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R14},
    {.symbol = "r15", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R15},
    {.symbol = "sp", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_SP},

    {.symbol = "s0", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S0},
    {.symbol = "s1", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S1},
    {.symbol = "s2", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S2},
    {.symbol = "s3", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S3},
    {.symbol = "s4", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S4},
    {.symbol = "s5", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S5},
    {.symbol = "s6", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S6},
    {.symbol = "s7", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S7},
    {.symbol = "s8", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S8},
    {.symbol = "s9", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S9},
    {.symbol = "s10", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S10},
    {.symbol = "s11", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S11},
    {.symbol = "s12", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S12},
    {.symbol = "s13", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S13},
    {.symbol = "s14", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S14},
    {.symbol = "s15", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S15},

    // instructions
    {.symbol = "mov", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_MOV },
    {.symbol = "fmov", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FMOV },
    {.symbol = "add", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ADD },
    {.symbol = "sub", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_SUB },
    {.symbol = "mul", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_MUL },
    {.symbol = "umul", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_UMUL },
    {.symbol = "div", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_DIV },
    {.symbol = "udiv", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_UDIV },
    {.symbol = "cmp", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_CMP },
    {.symbol = "B.EQ", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_BEQ },
    {.symbol = "B.EZ", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_BEZ },
    {.symbol = "B.NE", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_BNE },
    {.symbol = "B.NZ", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_BNZ },
    {.symbol = "B.LT", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_BLT },
    {.symbol = "B.LE", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_BLE },
    {.symbol = "B.GT", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_BGT },
    {.symbol = "B.GE", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_BGE },
};

#define MAKE_TOKEN(TYPE) { \
    tk.source_file = file_path;\
    tk.line = line;\
    tk.column = column;\
    tk.type = TYPE;\
    advance();\
}

Token Lexer::get_next()
{
    Token tk = {};

    skip_white_space();
    skip_comment();

    switch (current)
    {
    case '.':
        tk = parse_directive();
        break;
    case ',':
        MAKE_TOKEN(TOKEN_COMMA);
        break;
    case '#':
        tk = parse_immediate();
        break;
    case '\n':
        MAKE_TOKEN(TOKEN_NEW_LINE);
        break;
    case '\0':
        MAKE_TOKEN(TOKEN_END_OF_FILE);
        break;
    default:
        if (is_alpha() || current == '_') {
            tk = parse_symbol_or_label();
        }
        else if (is_num()) {
            tk = parse_immediate();
        }
        else {
            ErrorManager::error(file_path, line, column, "invalid character '%c'", current);
        }
        break;
    }

    return tk;
}

void Lexer::skip_white_space()
{
    while (current ==' '
        || current == '\r'
        || current == '\t') {
        advance();
    }
}

void Lexer::skip_comment()
{
    if (current == ';') {
    loop:
        u32 last_line = line;
        while (last_line == line) {
            advance();
        }

        if (current == ';')
            goto loop;
    }
}

char Lexer::peek(u8 offset)
{
    return (index + offset < size) ? content[index + offset] : '\0';
}

void Lexer::advance()
{
    if (index + 1 < size) {
        column++;
        if (current == '\n')
        {
            line++;
            column = 1;
        }
        current = content[++index];
    }
    else {
        current = '\0';
    }
}

Token Lexer::parse_directive()
{
    Token tk = {
        .type = TOKEN_END_OF_FILE,
        .source_file = file_path,
        .line = line,
        .column = column,
    };

    advance(); // .
    u64 start = index;
    while (is_alpha() || current == '_') {
        advance();
    }
    tk.str = std::string_view((char*)content + start, index - start);

    for (auto& sym : symbols) {
        if (sym.size == tk.str.size()) {
            if (memcmp(sym.symbol, tk.str.data(), sym.size) == 0) {
                tk.type = sym.type;
                tk.subtype = sym.subtype;
            }
        }
    }

    if (tk.type == TOKEN_END_OF_FILE) {
        ErrorManager::error(file_path, line, column, "unknown directive %.*s", tk.str.size(), tk.str.data());
    }

    return tk;
}

Token Lexer::parse_symbol_or_label()
{
    Token tk = {
        .source_file = file_path,
        .line = line,
        .column = column,
    };

    u64 start = index;
    while (is_alnum() || current == '_' || current == '.') {
        advance();
    }
    tk.str = std::string_view((char*)content + start, index - start);

    if (current == ':') {
        advance();
        tk.type = TOKEN_LABEL;
    }
    else {
        tk.type = TOKEN_SYMBOL;
    }

    return tk;
}

Token Lexer::parse_immediate()
{
    Token tk = {
        .source_file = file_path,
        .line = line,
        .column = column,
    };

    advance(); // #

    return tk;
}

bool Lexer::is_alpha() const
{
    return (current >= 'a' && current <= 'z')
        || (current >= 'A' && current <= 'Z');
}

bool Lexer::is_num() const
{
    return (current >= '0' && current <= '9');
}

bool Lexer::is_alnum() const
{
    return is_alpha() || is_num();
}
