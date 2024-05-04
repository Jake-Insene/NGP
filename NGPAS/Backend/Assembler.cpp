#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <FileFormat/Room.h>
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
    encode();

    // write
    std::ofstream output{ output_path, std::ios::binary };

    RoomHeader header = {
        .size_of_raw_data = instructions.size() * sizeof(word),
        .address_of_entry_point = entry_point_address,
    };

    output.write((char*)&header, sizeof(header));

    for (auto& inst : instructions) {
        output.write((char*)&inst.encoded, 4);
    }

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
        entry_point = last.str;
        break;
    default:
        break;
    }

    skip_whitespaces();
}

void Assembler::assemble_label()
{
    auto& l = labels.create(current.str);

    l.symbol = current.str;
    l.address = instructions.size() - 1;
    l.source_file = current.source_file;
    l.line = current.line;
    l.column = current.column;

    advance();
    skip_whitespaces();
}

#define EXPECTED_COMMA \
    if (!expected(TOKEN_COMMA, "',' was expected")) {\
        current_status = ERROR;\
    }

void Assembler::assemble_instruction()
{
    if (last.is_not(TOKEN_INSTRUCTION)) {
        ErrorManager::error(current.source_file, current.line, current.column, "invalid token");
        current_status = ERROR;
        return;
    }

    u32 address = instructions.size();
    switch (last.subtype)
    {
    case TI_MOV:
    {
        Register dest = get_register(current);
        advance();
        EXPECTED_COMMA;

        auto& inst = instructions.emplace_back();
        inst.source_file = last.source_file;
        inst.line = last.line;
        inst.column = last.column;
        inst.address = address;

        if (current.is(TOKEN_REGISTER)) {
            inst = Instruction::mov(dest, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = Instruction::movi(dest, (u16)current.u);
            advance();
        }
    }
        break;
    case TI_RET:
    {
        auto& inst = instructions.emplace_back(Instruction::ret());
        inst.source_file = last.source_file;
        inst.line = last.line;
        inst.column = last.column;
        inst.address = address;
    }
        break;
    default:
        ErrorManager::error(last.source_file, last.line, last.column, "invalid token");
        current_status = ERROR;
        break;
    }

    if (!expected(TOKEN_NEW_LINE, "a new line was expected")) {
        current_status = ERROR;
    }

    skip_whitespaces();
}

void Assembler::resolve_labels()
{
    for (auto& inst : instructions) {
        if (inst.type == TI_CALL || inst.is_branch()) {
            auto it = labels.find(inst.symbol);
            if (it != labels.map.end()) {
                inst.imm = labels.get(it->second).address - (inst.address+1);
            }
            else {
                ErrorManager::error(
                    inst.source_file, inst.line, inst.column,
                    "undefine reference to %s.*", inst.symbol.size(), inst.symbol.data()
                );
            }
        }
    }
}

void Assembler::encode()
{
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
    }

    return true;
}

void Assembler::skip_whitespaces()
{
    while (current.is(TOKEN_NEW_LINE)) {
        advance();
    }
}

Register Assembler::get_register(Token tk)
{
    Register reg = {};
    if (tk.subtype >= TOKEN_R0 && tk.subtype <= TOKEN_R15) {
        reg.is_float = false;
        reg.index = tk.subtype;
    }
    else if (tk.subtype >= TOKEN_S0 && tk.subtype <= TOKEN_S15) {
        reg.is_float = true;
        reg.index = tk.subtype - TOKEN_S0;
    }

    return reg;
}
