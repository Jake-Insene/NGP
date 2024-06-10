#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <fstream>

i32 Assembler::assemble_file(const char* file_path, const char* output_path)
{
    std::ifstream input{ file_path, std::ios::binary | std::ios::ate };
    if (!input.is_open()) {
        return INVALID_ARGUMENTS;
    }

    u32 size = (u32)input.tellg();
    u8* content = new u8[size+1];
    input.seekg(0);
    input.read((char*)content, size);
    input.close();

    content[size] = '\0';

    lexer.set(file_path, content, size);

    advance();
    advance();

    assemble_program();
    resolve_labels();

    // write
    std::ofstream output{ output_path, std::ios::binary };

    auto it = labels.find(entry_point.symbol);
    if (it == labels.end()) {
        ErrorManager::error(entry_point.source_file, entry_point.line, entry_point.column, "the entry point was not defined");
        current_status = ERROR;
        return current_status;
    }
    
    entry_point_address = it->second.address;
    RomHeader header = {
        .address_of_entry_point = (entry_point_address + sizeof(RomHeader)) / 4,
        .check_sum = u32(sizeof(RomHeader) + program.size()),
    };

    output.write((char*)&header, sizeof(header));

    output.write((char*)program.data(), program.size());

    output.close();

    return current_status;
}

void Assembler::assemble_program()
{
    while (current.is_not(TOKEN_END_OF_FILE)) {
        if (ErrorManager::must_syncronize) {
            syncronize();
        }

        switch (current.type)
        {
        case TOKEN_DIRECTIVE:
            assemble_directive();
            break;
        case TOKEN_NEW_LINE:
            advance();
            break;
        case TOKEN_LABEL:
            assemble_label();
            break;
        case TOKEN_INSTRUCTION:
            assemble_instruction();
            break;
        default:
            ErrorManager::error(current.source_file, current.line, current.column, "invalid token");
            current_status = ERROR;
            return;
        }
    }
}

void Assembler::assemble_label()
{
    auto& l = labels.emplace(current.str, Label()).first->second;

    l.symbol = current.str;
    l.address = u32(program.size());
    l.source_file = current.source_file;
    l.line = current.line;
    l.column = current.column;

    advance();
    skip_whitespaces();
}

void Assembler::encode_string(u8* mem, const std::string_view& str)
{
    u32 i = 0;
    u32 memi = 0;
    while (i < str.size()) {
        switch (str[i])
        {
        case '\\':
        {
            i++;
            if (str[i] == '0') {
                mem[memi] = '\0';
                memi++;
                i++;
            }
            else if (str[i] == 'n') {
                mem[memi] = '\n';
                memi++;
                i++;
            }
        }
        break;
        default:
            mem[memi] = str[i];
            memi++;
            i++;
            break;
        }
    }
}

void Assembler::resolve_labels()
{
    for (auto& tr : to_resolve) {
        u32& inst = *(u32*)&program[tr.address];

        auto it = labels.find(tr.symbol);
        if (it != labels.end()) {
            u32 ra = (it->second.address - (tr.address+4))/4;

            if (tr.type == TI_CALL) {
                inst = call(ra);
            }
            else if (tr.type == TI_B) {
                inst = b(ra);
            }
            else if (tr.type == TI_ADR) {
                inst = adr((inst >> 6) & 0x1F, ra);
            }
            else if (tr.type == TI_LD) {
                inst = ldpc((inst >> 6) & 0x1F, inst & 0x3F, ra);
            }
            else if (tr.type >= TI_BEQ && tr.type <= TI_BGE) {
                inst = bcond((inst >> 6) & 0xF, ra);
            }
        }
        else {
            ErrorManager::error(
                tr.source_file, tr.line, tr.column,
                "undefine reference to %s.*", tr.symbol.size(), tr.symbol.data()
            );
        }
    }
}

void Assembler::advance()
{
    last = current;
    current = next;
    next = lexer.get_next();
}

void Assembler::syncronize()
{
    ErrorManager::must_syncronize = false;

    while (true) {
        advance();

        switch (current.type)
        {
        case TOKEN_DIRECTIVE:
        case TOKEN_INSTRUCTION:
        case TOKEN_LABEL:
            return;
        default:
            break;
        }
    }
}

bool Assembler::expected(TokenType tk, const char* format, ...)
{
    advance();
    if (last.type != tk) {
        va_list args;
        va_start(args, format);
        ErrorManager::errorV(last.source_file, last.line, last.column, format, args);
        va_end(args);
        return false;
    }

    return true;
}

void Assembler::skip_whitespaces()
{
    while (current.is(TOKEN_NEW_LINE)) {
        advance();
    }
}

u8 Assembler::get_register(Token tk)
{
    if (tk.subtype >= TOKEN_R0 && tk.subtype <= TOKEN_R31) {
        return u8(tk.subtype);
    }
    else if (tk.subtype >= TOKEN_S0 && tk.subtype <= TOKEN_S31) {
        return u8(tk.subtype - TOKEN_S0);
    }
    else if (tk.subtype >= TOKEN_D0 && tk.subtype <= TOKEN_D31) {
        return u8(tk.subtype - TOKEN_D0);
    }

    return u8(-1);
}

u32& Assembler::new_word()
{
    program.emplace_back();
    program.emplace_back();
    program.emplace_back();
    program.emplace_back();
    return *(u32*)(&program[program.size() - 4]);
}

u8* Assembler::reserve(u32 count)
{
    for (u32 i = 0; i < count; i++)
        program.emplace_back();
    return &program[program.size() - count];
}
