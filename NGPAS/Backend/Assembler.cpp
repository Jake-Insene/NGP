// --------------------
// Assembler.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "Backend/Assembler.h"
#include "Core/Constants.h"
#include "ErrorManager.h"
#include <fstream>

bool Assembler::assembleFile(const char* file_path, const char* output_path, u32 origin)
{
    origin_address = origin;
    pre_processor.process(file_path);
    if (ErrorManager::is_panic_mode) {
        return false;
    }

    phase1();
    
    if(!ErrorManager::is_panic_mode)
    {
        program_index = 0;
        program.resize(2046);

        phase2();
        if (ErrorManager::is_panic_mode) {
            return false;
        }

        resolve_instructions();
        if (ErrorManager::is_panic_mode) {
            return false;
        }

        // write
        std::ofstream output{ output_path, std::ios::binary };

        RomHeader header = {
            .magic = RomSignature,
            .check_sum = u32(program_index),
            .target_address = ROMStart,
        };

        output.write((char*)&header, sizeof(RomHeader));
        output.write((char*)program.data(), program_index);

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
                if (context.unknown_label) {
                    break;
                }

                makeSymbol(name->str, value.u, name->source_file, name->line);
            }
            else {
                MAKE_ERROR((*name), return, "invalid expresion");
            }
        }
            break;
        case TOKEN_DIRECTIVE:
            gotoNextLine();
            break;
        case TOKEN_INSTRUCTION:
            gotoNextLine();
            break;
        case TOKEN_LABEL:
            makeLabel(current->str, u64(-1), current->source_file, current->line).isDefined = false;
            advance();
            break;
        default:
            advance();
            break;
        }
    }

}

Symbol& Assembler::makeLabel(std::string_view label, u64 address, const char* source_file, u32 line) {
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
        it->second.uvalue = address;
        return it->second;
    }

    auto& l = symbols.emplace(
        label,
        Symbol()
    ).first->second;

    l.symbol = std::move(composed);
    l.uvalue = address;
    l.source_file = source_file;
    l.line = line;

    return l;
}

Symbol& Assembler::makeSymbol(std::string_view label, u64 value, const char* source_file, u32 line) {
    std::string str_name = std::string(label);
    auto it = symbols.find(str_name);
    if (it != symbols.end()) {
        auto& symbol = it->second;
        return symbol;
    }
 
    auto& symbol = symbols.emplace(
        str_name, Symbol()
    ).first->second;

    symbol.uvalue = value;
    symbol.symbol = str_name;
    symbol.source_file = source_file;
    symbol.line = line;
    return symbol;
}

void Assembler::phase2() {
    token_index = 0;

    advance();
    advance();

    while (current->isNot(TOKEN_END_OF_FILE)) {
        context.undefined_label = false;

        if (ErrorManager::must_syncronize) {
            syncronize();
        }

        switch (current->type) {
        case TOKEN_SYMBOL:
        {
            Token* name = current;
            advance();

            auto it = findLabel(name->str);
            if (it != symbols.end()) {
                if (it->second.isDefined) {
                    gotoNextLine();
                }
            }

            if (current->is(TOKEN_EQUAL)) {
                advance();

                Token value = parseExpresion(ParsePrecedence::Start);
                if (context.unknown_label) {
                    break;
                }

                makeSymbol(name->str, value.u, name->source_file, name->line);
            }
            else {
                MAKE_ERROR((*name), return, "invalid expresion");
            }
        }
            break;
        case TOKEN_DIRECTIVE:
            assembleDirective();
            break;
        case TOKEN_NEW_LINE:
            advance();
            break;
        case TOKEN_LABEL:
        {
            u32 address = u64(origin_address + program_index - last_size);
            symbols.find(std::string(current->str))->second.uvalue = address;
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

void Assembler::resolve_instructions() {
    u32 index = program_index;

    for (auto& tr : to_resolve) {
        program_index = tr.address;
        
        token_index = tr.index;
        advance();
        advance();

        assembleInstruction();
    }

    program_index = index;
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

void Assembler::advanceToNextLine() {
    while (current->isNot(TOKEN_NEW_LINE)) {
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

u32& Assembler::newWord() {
    check_capacity(4);
    program_index += 4;
    return *(u32*)(&program[program_index - 4]);
}

u16& Assembler::newHalf() {
    check_capacity(2);
    program_index += 2;
    return *(u16*)(&program[program_index - 2]);
}

u8& Assembler::newByte() {
    check_capacity(1);
    program_index += 1;
    return program[program_index-1];
}

u8* Assembler::reserve(u32 count)
{
    check_capacity(count);

    u8& address = newByte();
    for (u32 i = 1; i < count; i++) {
        newByte();
    }

    return &address;
}

void Assembler::check_capacity(u32 count) {
    if (program_index + count >= program.size()) {
        program.resize(program.size() + (program.size()) / 2);
    }
}

std::unordered_map<std::string, Symbol>::iterator Assembler::findLabel(std::string_view label) {
    auto it = symbols.find(std::string(label));
    if (it == symbols.end()) {
        return symbols.end();
    }

    return it;
}
