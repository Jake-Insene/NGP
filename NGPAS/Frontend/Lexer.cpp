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
    { .symbol = "function", .size = 8, .type = TOKEN_DIRECTIVE, .subtype = TD_FUNCTION },
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
    case '{':
        MAKE_TOKEN(TOKEN_LEFT_BRACE);
        break;
    case '}':
        MAKE_TOKEN(TOKEN_RIGHT_BRACE);
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
    while (is_alnum() || current == '_') {
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
