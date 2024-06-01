#include "Backend/Assembler.h"
#include "ErrorManager.h"

void Assembler::assemble_directive()
{
    advance();
    switch (last.subtype)
    {
    case TD_ENTRY_POINT:
        if (!expected(TOKEN_SYMBOL, "a symbol was expected")) {
            current_status = ERROR;
            return;
        }
        entry_point.symbol = last.str;
        entry_point.source_file = last.source_file;
        entry_point.line = last.line;
        entry_point.column = last.column;
        break;
    case TD_STRING:
    {
        if (!expected(TOKEN_STRING, "a string was expected")) {
            current_status = ERROR;
            return;
        }

        u32 size = u32(align_up(u32(last.str.size()), 4));
        u8* mem = reserve(size);
        encode_string(mem, last.str);

        break;
    }
    case TD_BYTE:
        break;
    case TD_HALF:
        break;
    case TD_WORD:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected")) {
            current_status = ERROR;
            return;
        }

        new_word() = (u32)last.u;
    }
        break;
    case TD_DWORD:
        break;
    case TD_ZERO:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected")) {
            current_status = ERROR;
            return;
        }

        reserve(align_up(u32(last.u), 4));
    }
        break;
    default:
        break;
    }

    skip_whitespaces();
}
