#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <fstream>

bool Assembler::assembleFile(const char* file_path, const char* output_path, u32 origin)
{
    origin_address = origin;
    pre_processor.process(file_path);
    if (ErrorManager::is_panic_mode) {
        return false;
    }

    reserve(sizeof(RomHeader));

    phase1();
    
    if(!ErrorManager::is_panic_mode)
    {
        phase2();
        if (!ErrorManager::is_panic_mode) {
            return false;
        }

        // write
        std::ofstream output{ output_path, std::ios::binary };

        auto it = symbols.find(
            std::string(entry_point.symbol)
        );
        if (it == symbols.end()) {
            ErrorManager::error(
                entry_point.source_file, entry_point.line,
                "the entry point was not defined"
            );
        }

        entry_point_address = it->second.address;
        RomHeader* header = (RomHeader*)program.data();
        header->magic = RomSignature;
        header->check_sum = u32(program.size());
        header->address_of_entry_point = entry_point_address / 4;

        output.write((char*)program.data(), program.size());

        output.close();

        return true;
    }
     
    return false;
}

void Assembler::phase1() {
    token_index = 0;

    advance();
    advance();

    while (current->isNot(TOKEN_END_OF_FILE)) {
        if (ErrorManager::must_syncronize) {
            syncronize();
        }

        switch (current->type) {
        case TOKEN_SYMBOL:
        {
            Token* name = current;
            advance();

            if (current->is(TOKEN_EQUAL)) {
                advance();

                Token value = parseExpresion(ParsePrecedence::Start);

                std::string str_name = std::string(name->str);
                auto it = symbols.find(str_name);
                if (it != symbols.end()) {
                    MAKE_ERROR((*name), return, "%.*s", name->str.size(), name->str.data());
                }
                else {
                    auto& symbol = symbols.emplace(
                        str_name, Symbol()
                    ).first->second;

                    symbol.uvalue = value.u;
                    symbol.symbol = str_name;
                }
            }
            else {
                MAKE_ERROR((*name), return, "invalid expresion");
            }
        }
            break;
        case TOKEN_LABEL:
            makeLabel(current->str, u32(-1), current->source_file, current->line);
            advance();
            break;
        default:
            advance();
            break;
        }
    }

}

void Assembler::makeLabel(std::string_view label, u32 address, const char* source_file, u32 line) {
    std::string composed = {};

    if (label[0] == '.') {
        composed = std::string(last_label) + std::string(label);
    }
    else {
        composed = label;
        last_label = label;
    }

    auto it = symbols.find(composed);
    if (it != symbols.end()) {
        it->second.address = address;
    }
    else {
        auto& l = symbols.emplace(
            label,
            Symbol()
        ).first->second;

        l.symbol = std::move(composed);
        l.address = address;
        l.source_file = source_file;
        l.line = line;
    }

}

void Assembler::phase2() {
    token_index = 0;

    advance();
    advance();

    while (current->isNot(TOKEN_END_OF_FILE)) {
        if (ErrorManager::must_syncronize) {
            syncronize();
        }

        switch (current->type) {
        case TOKEN_DIRECTIVE:
            assembleDirective();
            break;
        case TOKEN_NEW_LINE:
            advance();
            break;
        case TOKEN_LABEL:
        {
            u32 address = u32(origin_address + program.size()) - last_size;
            symbols.find(std::string(current->str))->second.address = address;
            advance();
        }
            break;
        case TOKEN_INSTRUCTION:
            assembleInstruction();
            break;
        default:
            ErrorManager::error(current->source_file, current->line, "invalid token");
            return;
        }
    }
}

void Assembler::advance()
{
    last = current;
    current = next;
    if (token_index < pre_processor.tokens.size()) {
        next = &pre_processor.tokens[token_index++];
    }
    else {
        static Token end_of_file = Token{
            .type = TOKEN_END_OF_FILE
        };

        next = &end_of_file;
    }
}

void Assembler::syncronize()
{
    ErrorManager::must_syncronize = false;

    while (true) {
        switch (current->type)
        {
        case TOKEN_DIRECTIVE:
        case TOKEN_INSTRUCTION:
        case TOKEN_LABEL:
        case TOKEN_END_OF_FILE:
            return;
        default:
            break;
        }

        advance();
    }
}

bool Assembler::expected(TokenType tk, const char* format, ...) {
    advance();
    if (last->type != tk) {
        va_list args;
        va_start(args, format);
        ErrorManager::errorV(last->source_file, last->line, format, args);
        va_end(args);
        return false;
    }

    return true;
}

void Assembler::gotoNextLine()
{
    u32 line = current->line;
    while (line == current->line && current->isNot(TOKEN_END_OF_FILE)) {
        advance();
    }
}

u8 Assembler::getRegister(Token tk)
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

u32& Assembler::newWord()
{
    program.emplace_back();
    program.emplace_back();
    program.emplace_back();
    program.emplace_back();
    return *(u32*)(&program[program.size() - 4]);
}

u16& Assembler::newHalf() {
    program.emplace_back();
    program.emplace_back();
    return *(u16*)(&program[program.size() - 2]);
}

u8& Assembler::newByte() {
    return program.emplace_back();
}

u8* Assembler::reserve(u32 count)
{
    for (u32 i = 0; i < count; i++)
        program.emplace_back();
    return &program[program.size() - count];
}

std::unordered_map<std::string, Symbol>::iterator Assembler::findLabel(std::string_view label, bool& founded) {
    auto it = symbols.find(std::string(label));
    founded = true;
    if (it == symbols.end()) {
        founded = false;
        return symbols.end();
    }

    return it;
}
