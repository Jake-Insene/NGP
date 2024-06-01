#include "Frontend/Lexer.h"
#include "ErrorManager.h"
#include <string>

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
    {.symbol = "string", .size = 6, .type = TOKEN_DIRECTIVE, .subtype = TD_STRING },
    {.symbol = "byte", .size = 4, .type = TOKEN_DIRECTIVE, .subtype = TD_BYTE },
    {.symbol = "half", .size = 4, .type = TOKEN_DIRECTIVE, .subtype = TD_HALF },
    {.symbol = "word", .size = 4, .type = TOKEN_DIRECTIVE, .subtype = TD_WORD },
    {.symbol = "dword", .size = 5, .type = TOKEN_DIRECTIVE, .subtype = TD_DWORD },
    {.symbol = "zero", .size = 4, .type = TOKEN_DIRECTIVE, .subtype = TD_ZERO },

    // general purporse registers
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
    {.symbol = "r16", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R16},
    {.symbol = "r17", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R17},
    {.symbol = "r18", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R18},
    {.symbol = "r19", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R19},
    {.symbol = "r20", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R20},
    {.symbol = "r21", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R21},
    {.symbol = "r22", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R22},
    {.symbol = "r23", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R23},
    {.symbol = "r24", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R24},
    {.symbol = "r25", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R25},
    {.symbol = "r26", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R26},
    {.symbol = "r27", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R27},
    {.symbol = "r28", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R28},
    {.symbol = "r29", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R29},
    {.symbol = "r30", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R30},
    {.symbol = "r31", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R31},

    // single precision
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
    {.symbol = "s16", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S16},
    {.symbol = "s17", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S17},
    {.symbol = "s18", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S18},
    {.symbol = "s19", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S19},
    {.symbol = "s20", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S20},
    {.symbol = "s21", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S21},
    {.symbol = "s22", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S22},
    {.symbol = "s23", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S23},
    {.symbol = "s24", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S24},
    {.symbol = "s25", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S25},
    {.symbol = "s26", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S26},
    {.symbol = "s27", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S27},
    {.symbol = "s28", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S28},
    {.symbol = "s29", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S29},
    {.symbol = "s30", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S30},
    {.symbol = "s31", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S31},

    // double precision
    {.symbol = "d0", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D0},
    {.symbol = "d1", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D1},
    {.symbol = "d2", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D2},
    {.symbol = "d3", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D3},
    {.symbol = "d4", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D4},
    {.symbol = "d5", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D5},
    {.symbol = "d6", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D6},
    {.symbol = "d7", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D7},
    {.symbol = "d8", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D8},
    {.symbol = "d9", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D9},
    {.symbol = "d10", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D10},
    {.symbol = "d11", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D11},
    {.symbol = "d12", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D12},
    {.symbol = "d13", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D13},
    {.symbol = "d14", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D14},
    {.symbol = "d15", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D15},
    {.symbol = "d16", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D16},
    {.symbol = "d17", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D17},
    {.symbol = "d18", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D18},
    {.symbol = "d19", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D19},
    {.symbol = "d20", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D20},
    {.symbol = "d21", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D21},
    {.symbol = "d22", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D22},
    {.symbol = "d23", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D23},
    {.symbol = "d24", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D24},
    {.symbol = "d25", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D25},
    {.symbol = "d26", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D26},
    {.symbol = "d27", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D27},
    {.symbol = "d28", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D28},
    {.symbol = "d29", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D29},
    {.symbol = "d30", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D30},
    {.symbol = "d31", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D31},

    // extra registers
    {.symbol = "sp", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R31 },

    // instructions
    {.symbol = "mov", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_MOV },
    {.symbol = "movt", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_MOVT },
    {.symbol = "add", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ADD },
    {.symbol = "sub", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SUB },
    {.symbol = "mul", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_MUL },
    {.symbol = "umul", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_UMUL },
    {.symbol = "div", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_DIV },
    {.symbol = "udiv", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_UDIV },

    {.symbol = "and", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_AND },
    {.symbol = "or", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_OR },
    {.symbol = "xor", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_XOR },
    {.symbol = "shl", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SHL },
    {.symbol = "shr", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SHR },
    {.symbol = "not", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_NOT },
    {.symbol = "neg", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_NEG },
    {.symbol = "abs", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ABS },

    {.symbol = "cmp", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_CMP },

    {.symbol = "ld", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_LD },
    {.symbol = "ldsh", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_LDSH },
    {.symbol = "ldh", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_LDH },
    {.symbol = "ldsb", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_LDSB },
    {.symbol = "ldb", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_LDB },

    {.symbol = "st", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_ST },
    {.symbol = "sth", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_STH },
    {.symbol = "stb", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_STB },

    {.symbol = "adr", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ADR },

    {.symbol = "fmov", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FMOV },
    {.symbol = "fmovnc", .size = 6, .type = TOKEN_INSTRUCTION, .subtype = TI_FMOVNC },
    {.symbol = "fcvts", .size = 7, .type = TOKEN_INSTRUCTION, .subtype = TI_FCVTZS },
    {.symbol = "fcvtu", .size = 7, .type = TOKEN_INSTRUCTION, .subtype = TI_FCVTZU },
    {.symbol = "scvtf", .size = 7, .type = TOKEN_INSTRUCTION, .subtype = TI_SCVTF },
    {.symbol = "ucvtf", .size = 7, .type = TOKEN_INSTRUCTION, .subtype = TI_UCVTF },

    {.symbol = "fadd", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FADD },
    {.symbol = "fsub", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FSUB },
    {.symbol = "fmul", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FMUL },
    {.symbol = "fdiv", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FDIV },
    {.symbol = "fneg", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FNEG },
    {.symbol = "fabs", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FABS },

    {.symbol = "beq", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BEQ },
    {.symbol = "bez", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BEQ },
    {.symbol = "bne", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BNE },
    {.symbol = "bnz", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BNE },
    {.symbol = "blt", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BLT },
    {.symbol = "ble", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BLE },
    {.symbol = "bgt", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BGT },
    {.symbol = "bge", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BGE },
    {.symbol = "bcs", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BCS },
    {.symbol = "bhs", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BCS },
    {.symbol = "bnc", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BNC },
    {.symbol = "blo", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BNC },
    {.symbol = "bss", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BSS },
    {.symbol = "bns", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BNS },
    {.symbol = "bos", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BOS },
    {.symbol = "bno", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BNO },
    {.symbol = "bhi", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BHI },
    {.symbol = "bls", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BLS },

    {.symbol = "call", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_CALL },
    {.symbol = "b", .size = 1, .type = TOKEN_INSTRUCTION, .subtype = TI_B },
    {.symbol = "sc", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_SC },
    {.symbol = "ret", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_RET },
    {.symbol = "halt", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_HALT },
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

    switch (current)
    {
    case ';':
    {
        MAKE_TOKEN(TOKEN_NEW_LINE);
        u32 last_line = tk.line;
        while (last_line == line) {
            advance();
        }
    }
        break;
    case '.':
        tk = get_directive();
        break;
    case ',':
        MAKE_TOKEN(TOKEN_COMMA);
        break;
    case '#':
        tk = get_immediate();
        break;
    case '[':
        MAKE_TOKEN(TOKEN_LEFT_KEY);
        break;
    case ']':
        MAKE_TOKEN(TOKEN_RIGHT_KEY);
        break;
    case '"':
        tk = get_string();
        break;
    case '\n':
        MAKE_TOKEN(TOKEN_NEW_LINE);
        break;
    case '\0':
        MAKE_TOKEN(TOKEN_END_OF_FILE);
        break;
    default:
        if (is_alpha() || current == '_') {
            tk = get_symbol_or_label();
        }
        else {
            ErrorManager::error(file_path, line, column, "invalid token '%c'", current);
            MAKE_TOKEN(TOKEN_ERROR);
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
        index = size;
        current = '\0';
    }
}

Token Lexer::get_directive()
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

Token Lexer::get_symbol_or_label()
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

    for (auto& sym : symbols) {
        if (sym.size == tk.str.size()) {
            if (memcmp(sym.symbol, tk.str.data(), sym.size) == 0) {
                tk.type = sym.type;
                tk.subtype = sym.subtype;
            }
        }
    }

    return tk;
}

Token Lexer::get_immediate()
{
    Token tk = {
        .type = TOKEN_IMMEDIATE,
        .source_file = file_path,
        .line = line,
        .column = column,
    };

    advance(); // #

    u64 start = index;
    while (is_hex()) {
        advance();
    }
    tk.str = std::string_view((char*)content + start, index - start);

    std::string str = { tk.str.data(), tk.str.size() };
    int base = 10;
    if (current == 'h' || current == 'H') {
        advance();
        base = 16;
    }
    else if (current == 'b' || current == 'B') {
        advance();
        base = 2;
    }
    
    tk.u = std::stoul(str, nullptr, base);

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

bool Lexer::is_hex() const
{
    return 
        is_num() ||
        (current >= 'A' && current <= 'F') ||
        (current >= 'a' && current <= 'f');
}

Token Lexer::get_string()
{
    Token tk = {
        .type = TOKEN_STRING,
        .source_file = file_path,
        .line = line,
        .column = column,
    };

    advance(); // "
    u32 start = index;
    while (current != '"' && current != '\0') {
        advance();
    }
    tk.str = std::string_view((char*)content + start, index - start);

    if (current == '\0') {
        ErrorManager::error(file_path, line, column, "bad string");
    }
    else {
        advance(); // "
    }

    return tk;
}
