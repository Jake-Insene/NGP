#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <fstream>

void Assembler::assemble_directive() {
    advance();
    switch (last->subtype) {
    case TD_ORG:
    {
        Token result = parse_expresion(ParsePrecedence::Assignment);
        if (result.is_not(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(result, break, "a immediate value was expected");
        }

        if (result.u > 0xFFFF'FFFF) {
            MAKE_ERROR(result, break, "invalid origin");
        }

        origin_address = result.uword;
    }
    break;
    case TD_INCLUDE:
    {
        Token* last_current = current;
        Token* last_next = next;

        u32 last_scope = pre_processor.current_scope_index;
        pre_processor.current_scope_index = last->include;

        phase2();

        pre_processor.current_scope_index = last_scope;
        current = last_current;
        next = last_next;
    }
    break;
    case TD_ENTRY:
    {
        Token entry = parse_expresion(ParsePrecedence::Assignment);

        if (entry.is_not(TOKEN_SYMBOL)) {
            MAKE_ERROR(entry, break, "a symbol was expected")
        }

        entry_point.symbol = entry.str;
        entry_point.source_file = entry.source_file;
        entry_point.line = entry.line;
    }
    break;
    case TD_STRING:
    {
        Token string = parse_expresion(ParsePrecedence::Assignment);

        if (string.is_not(TOKEN_STRING)) {
            MAKE_ERROR(string, break, "a string was expected");
        }

        u32 size = u32(align_up(u32(string.str.len), 4));
        u8* mem = reserve(size);
        encode_string(mem, string.str);

    }
    break;
    case TD_BYTE:
    {
        Token byte = parse_expresion(ParsePrecedence::Assignment);
        if (byte.is_not(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(byte, break, "a immediate value was expected");
        }

        if (byte.u > 0xFF) {
            MAKE_ERROR(byte, break, "immediate value too long");
            return;
        }

        new_byte() = byte.byte[0];
    }
    break;
    case TD_HALF:
    {
        Token half = parse_expresion(ParsePrecedence::Assignment);
        if (half.is_not(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(half, break, "a immediate value was expected");
        }

        if (half.u > 0xFFFF) {
            MAKE_ERROR(half, break, "immediate value too long");
            return;
        }

        new_half() = half.ushort[0];
    }
    break;
    case TD_WORD:
    {
        Token word = parse_expresion(ParsePrecedence::Assignment);
        if (word.is_not(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(word, break, "a immediate value was expected");
        }

        if (word.u > 0xFFFF'FFFF) {
            MAKE_ERROR(word, break, "immediate value too long");
            return;
        }

        new_word() = word.uword;
    }
    break;
    case TD_DWORD:
    {
        Token dword = parse_expresion(ParsePrecedence::Assignment);
        if (dword.is_not(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(dword, break, "a immediate value was expected");
        }

        new_word() = dword.u >> 32;
        new_word() = dword.u & 0xFFFF'FFFF;
    }
    break;
    case TD_ZERO:
    {
        Token count = parse_expresion(ParsePrecedence::Assignment);
        if (count.is_not(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(count, break, "a immediate value was expected");
        }

        while (count.u--) {
            program.emplace_back(u8(0));
        }
    }
    break;
    default:
        break;
    }
}

