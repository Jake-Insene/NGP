/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <fstream>

void Assembler::assemble_directive() {
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
            if (next->is_not(TOKEN_STRING)) {
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
        Token result = parse_expresion(ParsePrecedence::Start);
        if (result.is_not(TOKEN_IMMEDIATE)) {
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
        Token string = parse_expresion(ParsePrecedence::Start);

        if (string.is_not(TOKEN_STRING)) {
            MAKE_ERROR(string, break, "a string was expected");
        }

        u32 size = align_up(u32(string.str.size()), 4);
        u8* mem = reserve(size);
        encode_string(mem, string.str);

    }
    break;
    case TD_BYTE:
    {
        bool is_first = true;
        while (current->is(TOKEN_COMMA) || is_first) {
            if (is_first) {
                is_first = false;
            }
            else {
                advance(); // ,
            }
            
            Token byte = parse_expresion(ParsePrecedence::Start);
            if (byte.is_not(TOKEN_IMMEDIATE)) {
                MAKE_ERROR(byte, break, "a immediate value was expected");
            }

            if (byte.u > 0xFF) {
                MAKE_ERROR(byte, break, "immediate value too long");
                return;
            }
            
            new_byte() = byte.byte[0];
        }
    }
    break;
    case TD_HALF:
    {
        bool is_first = true;
        while (current->is(TOKEN_COMMA) || is_first) {
            if (is_first) {
                is_first = false;
            }
            else {
                advance(); // ,
            }

            Token half = parse_expresion(ParsePrecedence::Start);
            if (half.is_not(TOKEN_IMMEDIATE)) {
                MAKE_ERROR(half, break, "a immediate value was expected");
            }

            if (half.u > 0xFFFF) {
                MAKE_ERROR(half, break, "immediate value too long");
                return;
            }

            new_half() = half.ushort[0];
        }
    }
    break;
    case TD_WORD:
    {
        bool is_first = true;
        while (current->is(TOKEN_COMMA) || is_first) {
            if (is_first) {
                is_first = false;
            }
            else {
                advance(); // ,
            }

            Token word = parse_expresion(ParsePrecedence::Start);
            if (word.is_not(TOKEN_IMMEDIATE)) {
                MAKE_ERROR(word, break, "a immediate value was expected");
            }

            if (word.u > 0xFFFF'FFFF) {
                MAKE_ERROR(word, break, "immediate value too long");
                return;
            }

            new_word() = word.uword;
        }
    }
    break;
    case TD_DWORD:
    {
        bool is_first = true;
        while (current->is(TOKEN_COMMA) || is_first) {
            if (is_first) {
                is_first = false;
            }
            else {
                advance(); // ,
            }

            Token dword = parse_expresion(ParsePrecedence::Start);
            if (dword.is_not(TOKEN_IMMEDIATE)) {
                MAKE_ERROR(dword, break, "a immediate value was expected");
            }

            new_word() = dword.u >> 32;
            new_word() = dword.u & 0xFFFF'FFFF;
        }
    }
    break;
    case TD_ZERO:
    {
        Token count = parse_expresion(ParsePrecedence::Start);
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

