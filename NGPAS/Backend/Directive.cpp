// --------------------
// Directive.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <fstream>

void Assembler::assembleDirective() {
    advance();
    switch (last->subtype) {
    case TD_FORMAT:
    {
        if (current->is(TOKEN_DIRECTIVE)) {
            if (current->subtype != TD_FORMAT_RAW || current->subtype != TD_FORMAT_ROM) {
                file_format = TokenDirective(current->subtype);

                advance(); // file_format
            }
            else {
                MAKE_ERROR((*current), return, "invalid format specifier");
            }
        }else{
            MAKE_ERROR((*current), return, "a format directive was expected");
        }

        if (current->is(TOKEN_DIRECTIVE) && current->subtype == TD_AS) {
            if (next->isNot(TOKEN_STRING)) {
                MAKE_ERROR((*current), return, "a file extension was expected");
            }

            extension = next->str;
            advance(); // as
            advance(); // str
        }

    }
    break;
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
        last_size = program_index;
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

