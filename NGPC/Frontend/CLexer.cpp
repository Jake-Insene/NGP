/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Frontend/CLexer.h"
#include "ErrorManager.h"


struct SymbolInfo
{
    const char* symbol;
    u8 size;
    CTokenType type;
    u8 subtype;
};

SymbolInfo symbols[] =
{
    // directives
    {.symbol = "include", .size = 7, .type = TOKEN_DIRECTIVE, .subtype = TD_INCLUDE },
    {.symbol = "define", .size = 6, .type = TOKEN_DIRECTIVE, .subtype = TD_DEFINE },

    // keywords
    {.symbol = "void", .size = 4, .type = TOKEN_KEYWORD, .subtype = TK_VOID },
    {.symbol = "i8", .size = 2, .type = TOKEN_KEYWORD, .subtype = TK_I8 },
    {.symbol = "u8", .size = 2, .type = TOKEN_KEYWORD, .subtype = TK_U8 },
    {.symbol = "i16", .size = 3, .type = TOKEN_KEYWORD, .subtype = TK_I16 },
    {.symbol = "u16", .size = 3, .type = TOKEN_KEYWORD, .subtype = TK_U16 },
    {.symbol = "i32", .size = 3, .type = TOKEN_KEYWORD, .subtype = TK_I32 },
    {.symbol = "u32", .size = 3, .type = TOKEN_KEYWORD, .subtype = TK_U32 },
    {.symbol = "f32", .size = 3, .type = TOKEN_KEYWORD, .subtype = TK_F32 },
    
    {.symbol = "func", .size = 4, .type = TOKEN_KEYWORD, .subtype = TK_FUNC },
    {.symbol = "var", .size = 3, .type = TOKEN_KEYWORD, .subtype = TK_VAR },
    {.symbol = "const", .size = 5, .type = TOKEN_KEYWORD, .subtype = TK_CONST },
    {.symbol = "static", .size = 6, .type = TOKEN_KEYWORD, .subtype = TK_STATIC },
    {.symbol = "inline", .size = 6, .type = TOKEN_KEYWORD, .subtype = TK_INLINE },
    {.symbol = "struct", .size = 6, .type = TOKEN_KEYWORD, .subtype = TK_STRUCT },
    {.symbol = "return", .size = 6, .type = TOKEN_KEYWORD, .subtype = TK_RETURN },
    {.symbol = "if", .size = 2, .type = TOKEN_KEYWORD, .subtype = TK_IF },
    {.symbol = "else", .size = 4, .type = TOKEN_KEYWORD, .subtype = TK_ELSE },
    {.symbol = "for", .size = 3, .type = TOKEN_KEYWORD, .subtype = TK_FOR },
    {.symbol = "while", .size = 5, .type = TOKEN_KEYWORD, .subtype = TK_WHILE },
    {.symbol = "do", .size = 2, .type = TOKEN_KEYWORD, .subtype = TK_DO },
};

#define MAKE_TOKEN(TYPE) { \
    tk.source_file = file_path;\
    tk.line = line;\
    tk.type = TYPE;\
    advance();\
}

#define MAKE_TOKEN_NO_ADVANCE(TYPE) { \
    tk.source_file = file_path;\
    tk.line = line;\
    tk.type = TYPE;\
}

#define MAKE_TOKEN_TWO(TYPE) { \
    tk.source_file = file_path;\
    tk.line = line;\
    tk.type = TYPE;\
    advance();\
    advance();\
}


void CLexer::set(StringID source_file, u8* ctn, u32 s)
{
    file_path = source_file;

    content = ctn;
    size = s;
    index = 0;
    line = 1;
    current = content[index];
}

CToken CLexer::get_next()
{
    CToken tk = {};
    tk.type = TOKEN_ERROR;
    tk.source_file = file_path;
    tk.line = line;

    skip_white_space();

lexer_routine:
    switch (current)
    {
    case ':':
        MAKE_TOKEN(TOKEN_COLON);
        break;
    case ';':
        MAKE_TOKEN(TOKEN_SEMICOLON);
        break;
    case '=':
        if (peek(1) == '=')
        {
            MAKE_TOKEN_TWO(TOKEN_EQUALEQUAL);
        }
        else
        {
            MAKE_TOKEN(TOKEN_EQUAL);
        }

        break;
    case '!':
        if (peek(1) == '=')
        {
            MAKE_TOKEN_TWO(TOKEN_NOTEQUAL);
        }
        else
        {
            MAKE_TOKEN(TOKEN_NOT);
        }
        break;
    case '<':
        if (peek(1) == '=')
        {
            MAKE_TOKEN_TWO(TOKEN_LESSEQUAL);
        }
        else if (peek(1) == '<')
        {
            MAKE_TOKEN_TWO(TOKEN_SHL);
        }
        else
        {
            MAKE_TOKEN(TOKEN_LESS);
        }
        break;
    case '>':
        if (peek(1) == '=')
        {
            MAKE_TOKEN_TWO(TOKEN_GREATEREQUAL);
        }
        else if (peek(1) == '>')
        {
            MAKE_TOKEN_TWO(TOKEN_SHR);
        }
        else
        {
            MAKE_TOKEN(TOKEN_GREATER);
        }
        break;
    case ',':
        MAKE_TOKEN(TOKEN_COMMA);
        break;
    case '(':
        MAKE_TOKEN(TOKEN_LEFT_PARENT);
        break;
    case ')':
        MAKE_TOKEN(TOKEN_RIGHT_PARENT);
        break;
    case '{':
        MAKE_TOKEN(TOKEN_LEFT_BRACE);
        break;
    case '}':
        MAKE_TOKEN(TOKEN_RIGHT_BRACE);
        break;
    case '[':
        MAKE_TOKEN(TOKEN_LEFT_KEY);
        break;
    case ']':
        MAKE_TOKEN(TOKEN_RIGHT_KEY);
        break;
    case '+':
        MAKE_TOKEN(TOKEN_PLUS);
        break;
    case '-':
        MAKE_TOKEN(TOKEN_MINUS);
        break;
    case '*':
        MAKE_TOKEN(TOKEN_STAR);
        break;
    case '/':
    {
        if (peek(1) == '/')
        {
            u32 last_line = line;
            while (last_line == line && current != '\0')
            {
                advance();
            }

            goto lexer_routine;
        }
        else
        {
            MAKE_TOKEN(TOKEN_SLASH);
        }
    }
        break;
    case '^':
        MAKE_TOKEN(TOKEN_XOR);
        break;
    case '~':
        MAKE_TOKEN(TOKEN_NOT);
        break;
    case '&':
        MAKE_TOKEN(TOKEN_AND);
        break;
    case '|':
        MAKE_TOKEN(TOKEN_OR);
        break;
    case '"':
        tk = get_string();
        break;
    case '\'':
        tk = get_string();
        break;
    case '\0':
        MAKE_TOKEN(TOKEN_END_OF_FILE);
        break;
    default:
        if (is_alpha(current) || current == '_')
        {
            tk = get_identifier();
        }
        else if (is_num(current))
        {
            tk = get_number();
        }
        else
        {
            ErrorManager::error(get_file_path().data(), line, "invalid token '%c'", current);
            advance();
        }
        break;
    }

    return tk;
}

void CLexer::skip_white_space()
{
    while (current == ' '
        || current == '\r'
        || current == '\t'
        || current == '\n')
    {
        advance();
    }
}

char CLexer::peek(u8 offset)
{
    return (index + offset < size) ? content[index + offset] : '\0';
}

void CLexer::advance()
{
    if (index + 1 < size)
    {
        if (current == '\n')
        {
            line++;
        }
        current = content[++index];
    }
    else
    {
        index = size;
        current = '\0';
    }
}

CToken CLexer::get_identifier()
{
    CToken tk =
    {
        .type = TOKEN_IDENTIFIER,
        .source_file = file_path,
        .line = line,
    };

    u64 start = index;
    while (is_alnum(current) || current == '_')
    {
        advance();
    }

    std::string_view view{ (char*)content + start, index - start };
    tk.str = StringPool::get_or_insert(view);

    for (auto& sym : symbols)
    {
        if (sym.size == view.size())
        {
            for (u8 i = 0; i < sym.size; i++)
            {
                if (sym.symbol[i] != view[i])
                {
                    goto next;
                }
            }

            tk.type = sym.type;
            tk.subtype = sym.subtype;
        next:
            {}
        }
    }

    return tk;
}

CToken CLexer::get_number()
{
    CToken tk =
    {
        .type = TOKEN_NUMBER,
        .source_file = file_path,
        .line = line,
    };

    if (current == '#')
    {
        advance();
    }

    int base = 10;
    if (current == '0')
    {
        if (peek(1) == 'X' || peek(1) == 'x')
        {
            base = 16;
            advance();
            advance();

            if (!is_hex(current))
            {
                ErrorManager::error(get_file_path().data(), line, "invalid constant", current);
                return tk;
            }
        }
        else if (peek(1) == 'b' || peek(1) == 'B')
        {
            base = 2;
            advance();
            advance();

            if (!is_bin(current))
            {
                ErrorManager::error(get_file_path().data(), line, "invalid constant", current);
                return tk;
            }
        }
    }

    u32 start = index;
    u32 i = 0;
    while (is_hex(current) && i < 32)
    {
        advance();
        i++;
    }

    if (i >= 32)
    {
        ErrorManager::error(get_file_path().data(), line, "constant number too long", current);
    }

    if (current == '.')
    {
        tk.subtype = TN_FLOAT;
        advance(); // Skip '.'

        while (is_num(current))
            advance();

        tk.s = std::strtof((char*)content + start, nullptr);
    }
    else
    {
        tk.u = std::stoull((char*)content + start, nullptr, base);
        tk.subtype = TN_UNSIGNED;
    }

    return tk;
}

bool CLexer::is_alpha(u8 c) const
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z');
}

bool CLexer::is_num(u8 c) const
{
    return (c >= '0' && c <= '9');
}

bool CLexer::is_alnum(u8 c) const
{
    return is_alpha(c) || is_num(c);
}

bool CLexer::is_hex(u8 c) const
{
    return
        is_num(c) ||
        (c >= 'A' && c <= 'F') ||
        (c >= 'a' && c <= 'f');
}

bool CLexer::is_bin(u8 c) const
{
    return c == '0' || c == '1';
}

CToken CLexer::get_string()
{
    CToken tk =
    {
        .type = TOKEN_STRING,
        .source_file = file_path,
        .line = line,
    };

    bool cuot = current == '\'';

    advance(); // " || '
    u32 start = index;
    while ((cuot ? current != '\'' : current != '"') && current != '\0')
    {
        if (current == '\'' || current == '"')
        {
            ErrorManager::error(get_file_path().data(), line, "inconsist string");
            return tk;
        }
        advance();
    }
    std::string_view view{ (char*)content + start, index - start };
    tk.str = StringPool::get_or_insert(view);

    if (current == '\0')
    {
        ErrorManager::error(get_file_path().data(), line, "bad string");
    }
    else
    {
        advance(); // " || '
    }

    return tk;
}
