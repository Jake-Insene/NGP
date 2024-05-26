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

        u32 size = u32(alignUp(u32(last.str.size()), 4));
        u8* mem = reserve(size);
        encode_string(mem, last.str);

        break;
    }
    default:
        break;
    }

    skip_whitespaces();
}
