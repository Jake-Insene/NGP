/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Backend/Assembler.h"

#include "ErrorManager.h"
#include "StringUtility.h"
#include <fstream>


#define HANDLE_NON_VALID_IMM(TK, IMM_LIMIT, IDX, PIDX) \
    if (context.undefined_label == true && context.is_in_resolve == false)\
        ADD_RESOLVE(ResolveDirective, IDX, PIDX)\
    else if (context.undefined_label == true && context.is_in_resolve == true)\
        return;\
    else if (TK.u > IMM_LIMIT)\
        MAKE_ERROR(TK, return, "immediate value too long")

#define HANDLE_NON_VALID_IMM_FP(TK, IDX, PIDX) \
    if (context.undefined_label == true && context.is_in_resolve == false)\
        ADD_RESOLVE(ResolveDirective, IDX, PIDX)\
    else if (context.undefined_label == true && context.is_in_resolve == true)\
        return;\


void Assembler::assemble_directive()
{
    advance();
    switch (last->subtype)
    {
    case TD_FORMAT:
    {
        if (current->is(TOKEN_DIRECTIVE))
        {
            if (current->subtype == TD_FORMAT_RAW || current->subtype == TD_FORMAT_ROM)
            {
                file_format = AsmTokenDirective(current->subtype);

                advance(); // file_format
            }
            else
            {
                MAKE_ERROR((*current), return, "invalid format specifier");
            }
        }
        else
        {
            MAKE_ERROR((*current), return, "a format directive was expected");
        }

        if (current->is(TOKEN_DIRECTIVE) && current->subtype == TD_AS)
        {
            if (!next->is(TOKEN_STRING))
            {
                MAKE_ERROR((*current), return, "a file extension was expected");
            }

            extension = next->get_str();
            advance(); // as
            advance(); // str
        }

    }
    break;
    case TD_ORG:
    {
        AsmToken result = parse_expression(ParsePrecedence::Start);
        if (!result.is(TOKEN_IMMEDIATE))
        {
            MAKE_ERROR(result, return, "a immediate value was expected");
        }

        if (result.u > 0xFFFF'FFFF)
        {
            MAKE_ERROR(result, return, "invalid origin");
        }

        origin_address = result.uword;
        last_size = program_index;
    }
    break;
    case TD_INCBIN:
    {
        if (!expected(TOKEN_STRING, "a file path was expected"))
            return;

        std::string file_path_tmp{};
        file_path_tmp.resize(last->get_str().size());
        encode_string((u8*)file_path_tmp.data(), last->str);
        StringID file_path_tmp_id = StringPool::get_or_insert(file_path_tmp);

        std::string file_path = AsmUtility::path_relative_to(last->source_file, file_path_tmp_id);

        std::ifstream input{ file_path, std::ios::binary | std::ios::ate };
        if (!input.is_open())
        {
            MAKE_ERROR((*last), return, "the file '%s' was not founded", file_path.c_str());
        }

        std::streamoff file_size = input.tellg();
        input.seekg(0);

        u8* mem = reserve(file_size);
        input.read((char*)mem, file_size);
        input.close();
    }
        break;
    case TD_STRING:
    {
        AsmToken string = parse_expression(ParsePrecedence::Start);

        if (!string.is(TOKEN_STRING))
        {
            MAKE_ERROR(string, return, "a string was expected");
        }

        u32 len = get_real_string_len(string.str);
        u32 size = align_up(len, 4);
        u8* mem = reserve(size);
        encode_string(mem, string.str);

    }
    break;
    case TD_BYTE:
    {
        bool is_first = true;
        u32 index = token_index - 3;
        u32 prog_idx = program_index;
        while (current->is(TOKEN_COMMA) || is_first)
        {
            if (is_first)
            {
                is_first = false;
            }
            else
            {
                advance(); // ,
            }

            AsmToken byte = parse_expression(ParsePrecedence::Start);
            if (!byte.is(TOKEN_IMMEDIATE))
            {
                MAKE_ERROR(byte, return, "a immediate value was expected");
            }

            HANDLE_NON_VALID_IMM(byte, 0xFF, index, prog_idx);
            new_byte() = byte.byte[0];
        }
    }
    break;
    case TD_HALF:
    {
        bool is_first = true;
        u32 index = token_index - 3;
        u32 prog_idx = program_index;
        while (current->is(TOKEN_COMMA) || is_first)
        {
            if (is_first)
            {
                is_first = false;
            }
            else
            {
                advance(); // ,
            }

            AsmToken half = parse_expression(ParsePrecedence::Start);
            if (!half.is(TOKEN_IMMEDIATE))
            {
                MAKE_ERROR(half, break, "a immediate value was expected");
            }

            HANDLE_NON_VALID_IMM(half, 0xFFFF, index, prog_idx);
            new_half() = half.ushort[0];
        }
    }
    break;
    case TD_WORD:
    {
        bool is_first = true;
        u32 index = token_index - 3;
        u32 prog_idx = program_index;
        while (current->is(TOKEN_COMMA) || is_first)
        {
            if (is_first)
            {
                is_first = false;
            }
            else
            {
                advance(); // ,
            }
            
            AsmToken word = parse_expression(ParsePrecedence::Start);
            if (!word.is(TOKEN_IMMEDIATE))
            {
                MAKE_ERROR(word, return, "a immediate value was expected");
            }

            HANDLE_NON_VALID_IMM(word, 0xFFFF'FFFF, index, prog_idx);
            new_word() = word.uword;
        }
    }
    break;
    case TD_DWORD:
    {
        bool is_first = true;
        u32 index = token_index - 3;
        u32 prog_idx = program_index;
        while (current->is(TOKEN_COMMA) || is_first)
        {
            if (is_first)
            {
                is_first = false;
            }
            else
            {
                advance(); // ,
            }

            AsmToken dword = parse_expression(ParsePrecedence::Start);
            if (!dword.is(TOKEN_IMMEDIATE))
            {
                MAKE_ERROR(dword, return, "a immediate value was expected");
            }

            HANDLE_NON_VALID_IMM(dword, 0xFFFF'FFFF'FFFF'FFFF, index, prog_idx);
            new_word() = dword.u & 0xFFFF'FFFF;
            new_word() = dword.u >> 32;
        }
    }
    break;
    case TD_FLOAT32:
    {
        bool is_first = true;
        u32 index = token_index - 3;
        u32 prog_idx = program_index;
        while (current->is(TOKEN_COMMA) || is_first)
        {
            if (is_first)
            {
                is_first = false;
            }
            else
            {
                advance(); // ,
            }

            AsmToken f = parse_expression(ParsePrecedence::Start);
            if (!f.is(TOKEN_IMMEDIATE))
            {
                MAKE_ERROR(f, return, "a immediate value was expected");
            }

            HANDLE_NON_VALID_IMM_FP(f, index, prog_idx);
            f.s = f32(f.d);
            new_word() = f.uword;
        }
    }
    break;
    case TD_SINGLE:
    {
        bool is_first = true;
        u32 index = token_index - 3;
        u32 prog_idx = program_index;
        while (current->is(TOKEN_COMMA) || is_first)
        {
            if (is_first)
            {
                is_first = false;
            }
            else
            {
                advance(); // ,
            }

            AsmToken f = parse_expression(ParsePrecedence::Start);
            if (!f.is(TOKEN_IMMEDIATE))
            {
                MAKE_ERROR(f, return, "a immediate value was expected");
            }

            HANDLE_NON_VALID_IMM_FP(f, index, prog_idx);
            f.s = f32(f.d);
            new_word() = f.u;
        }
    }
    break;
    case TD_ZERO:
    {
        u32 index = token_index - 3;
        u32 prog_idx = program_index;
        
        AsmToken count = parse_expression(ParsePrecedence::Start);
        if (!count.is(TOKEN_IMMEDIATE))
        {
            MAKE_ERROR(count, return, "a immediate value was expected");
        }

        if (count.i < 0)
        {
            MAKE_ERROR(count, return, "a negative value is not allowed %lli", count.i);
        }

        while (count.u--)
        {
            new_byte() = 0;
        }
    }
    break;
    case TD_SPACE:
    {

    }
    break;
    case TD_ALIGN:
    {
        AsmToken alignment = parse_expression(ParsePrecedence::Start);
        if (!alignment.is(TOKEN_IMMEDIATE))
        {
            MAKE_ERROR(alignment, return, "a immediate value was expected");
        }

        if (alignment.i < 0)
        {
            MAKE_ERROR(alignment, return, "a negative value is not allowed: '%lli'", alignment.i);
        }

        u32 aligned = align_up(this->program_index, (u16)alignment.u);

        u32 required_bytes = aligned - program_index;
        while (required_bytes--)
            new_byte() = 0;
    }
    break;
    default:
        break;
    }
}

