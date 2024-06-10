#include "Backend/Assembler.h"
#include "ErrorManager.h"

void Assembler::assemble_directive()
{
    advance();
    switch (last.subtype)
    {
    case TD_ENTRY:
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
            break;
        }

        u32 size = u32(align_up(u32(last.str.size()), 4));
        u8* mem = reserve(size);
        encode_string(mem, last.str);

        break;
    }
    case TD_BYTE:
        ErrorManager::error(last.source_file, last.line, last.column, "not implemented yet");
        break;
    case TD_HALF:
        ErrorManager::error(last.source_file, last.line, last.column, "not implemented yet");
        break;
    case TD_WORD:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected")) {
            current_status = ERROR;
            break;
        }

        new_word() = (u32)last.u;
    }
        break;
    case TD_DWORD:
        ErrorManager::error(last.source_file, last.line, last.column, "not implemented yet");
        break;
    case TD_ZERO:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected")) {
            current_status = ERROR;
            break;
        }

        reserve(align_up(u32(last.u), 4));
    }
        break;
    default:
        break;
    }

    skip_whitespaces();
}
