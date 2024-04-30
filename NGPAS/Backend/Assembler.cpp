#include "Assembler.h"
#include "ErrorManager.h"
#include <fstream>

i32 Assembler::assemble_file(const char* file_path, const char* /*output_path*/)
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
    case TD_FUNCTION:
        assemble_function();
        break;
    default:
        break;
    }
}

void Assembler::assemble_function()
{
    Context.is_in_function = true;
    
    Function& fn = functions.create(current.str);
    advance();

    assemble_function_directives(fn);

    if(!expected(TOKEN_LEFT_BRACE, "'{' was expected")) {
        current_status = ERROR;
        return;
    }

    while (current.is_not(TOKEN_RIGHT_BRACE)) {
        assemble_instruction(fn);
    }
    advance(); // }

    Context.is_in_function = false;
}

void Assembler::assemble_function_directives(Function& /*fn*/)
{
}

#define EXPECTED_COMMA \
    if (!expected(TOKEN_COMMA, "',' was expected")) {\
        current_status = ERROR;\
        return;\
    }

void Assembler::assemble_instruction(Function& fn)
{
    advance();

    if (last.is_not(TOKEN_INSTRUCTION)) {
        ErrorManager::error(current.source_file, current.line, current.column, "invalid token");
        current_status = ERROR;
        return;
    }

    switch (last.subtype)
    {
    case TI_MOV:
    {
        Register dest = get_register(current);
        advance();
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
            fn.body.emplace_back(Instruction::mov(dest, get_register(current)));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            fn.body.emplace_back(Instruction::movi(dest, (u16)current.u));
            advance();
        }
    }
        break;
    case TI_RET:
        if (current.is(TOKEN_REGISTER)) {
            fn.body.emplace_back(Instruction::ret(get_register(current)));
        }
        else if(current.is_one_of(TOKEN_NEW_LINE, TOKEN_END_OF_FILE)) {
            fn.body.emplace_back(Instruction::retv());
        }
        else {
            ErrorManager::error(last.source_file, last.line, last.column, "invalid operand");
            return;
        }
        break;
    default:
        ErrorManager::error(last.source_file, last.line, last.column, "invalid token");
        current_status = ERROR;
        return;
    }

    if (!expected(TOKEN_NEW_LINE, "a new line was expected")) {
        current_status = ERROR;
        return;
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

Register Assembler::get_register(Token tk)
{
    Register reg = {};
    if (tk.subtype >= TOKEN_X0 && tk.subtype <= TOKEN_X15) {
        reg.size = REG_DWORD;
        reg.index = tk.subtype;
    }
    else if (tk.subtype >= TOKEN_W0 && tk.subtype <= TOKEN_W15) {
        reg.size = REG_WORD;
        reg.index = tk.subtype - TOKEN_W0;
    }
    else if (tk.subtype >= TOKEN_S0 && tk.subtype <= TOKEN_S15) {
        reg.is_float = true;
        reg.size = REG_WORD;
        reg.index = tk.subtype - TOKEN_S0;
    }
    else if (tk.subtype >= TOKEN_D0 && tk.subtype <= TOKEN_D15) {
        reg.is_float = true;
        reg.size = REG_DWORD;
        reg.index = tk.subtype - TOKEN_D0;
    }
    else if (tk.subtype >= TOKEN_Q0 && tk.subtype <= TOKEN_Q15) {
        reg.is_float = true;
        reg.size = REG_QWORD;
        reg.index = tk.subtype - TOKEN_Q0;
    }

    return reg;
}
