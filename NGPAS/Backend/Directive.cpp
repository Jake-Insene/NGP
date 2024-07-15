#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <fstream>

void Assembler::assembleDirective() {
    advance();
    switch (last->subtype) {
    case TD_ORG:
    {
        Token result = parseExpresion(ParsePrecedence::Start);
        if (result.isNot(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(result, break, "a immediate value was expected");
        }

        if (result.u > 0xFFFF'FFFF) {
            MAKE_ERROR(result, break, "invalid origin");
        }

        origin_address = result.uword;
        last_size = (u32)program.size();
    }
    break;
    case TD_ENTRY:
    {
        Token entry = parseExpresion(ParsePrecedence::Start);

        if (entry.isNot(TOKEN_SYMBOL)) {
            MAKE_ERROR(entry, break, "a symbol was expected")
        }

        entry_point.symbol = entry.str;
        entry_point.source_file = entry.source_file;
        entry_point.line = entry.line;
    }
    break;
    case TD_STRING:
    {
        Token string = parseExpresion(ParsePrecedence::Start);

        if (string.isNot(TOKEN_STRING)) {
            MAKE_ERROR(string, break, "a string was expected");
        }

        u32 size = align_up(u32(string.str.size()), 4);
        u8* mem = reserve(size);
        encodeString(mem, string.str);

    }
    break;
    case TD_BYTE:
    {
        Token byte = parseExpresion(ParsePrecedence::Start);
        if (byte.isNot(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(byte, break, "a immediate value was expected");
        }

        if (byte.u > 0xFF) {
            MAKE_ERROR(byte, break, "immediate value too long");
            return;
        }

        newByte() = byte.byte[0];
    }
    break;
    case TD_HALF:
    {
        Token half = parseExpresion(ParsePrecedence::Start);
        if (half.isNot(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(half, break, "a immediate value was expected");
        }

        if (half.u > 0xFFFF) {
            MAKE_ERROR(half, break, "immediate value too long");
            return;
        }

        newHalf() = half.ushort[0];
    }
    break;
    case TD_WORD:
    {
        Token word = parseExpresion(ParsePrecedence::Start);
        if (word.isNot(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(word, break, "a immediate value was expected");
        }

        if (word.u > 0xFFFF'FFFF) {
            MAKE_ERROR(word, break, "immediate value too long");
            return;
        }

        newWord() = word.uword;
    }
    break;
    case TD_DWORD:
    {
        Token dword = parseExpresion(ParsePrecedence::Start);
        if (dword.isNot(TOKEN_IMMEDIATE)) {
            MAKE_ERROR(dword, break, "a immediate value was expected");
        }

        newWord() = dword.u >> 32;
        newWord() = dword.u & 0xFFFF'FFFF;
    }
    break;
    case TD_ZERO:
    {
        Token count = parseExpresion(ParsePrecedence::Start);
        if (count.isNot(TOKEN_IMMEDIATE)) {
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

