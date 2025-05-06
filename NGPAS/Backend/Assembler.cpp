/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <fstream>

bool Assembler::assemble_file(const char* file_path, const char* output_path)
{
    origin_address = 0;
    last_size = 0;
    pre_processor.process(file_path);
    if (ErrorManager::is_panic_mode) {
        return false;
    }

    phase1();
    
    if(!ErrorManager::is_panic_mode)
    {
        program_index = 0;
        program.resize(4096);

        phase2();
        if (ErrorManager::is_panic_mode) {
            return false;
        }

        resolve_instructions();
        if (ErrorManager::is_panic_mode) {
            return false;
        }

        // write

        std::string output_file_name = { output_path };
        if (!extension.empty()) {
            output_file_name += '.';
            output_file_name += extension;
        }

        std::ofstream output{ output_file_name, std::ios::binary };

        if (file_format == TD_FORMAT_ROM) {
            // TODO: NGP executable format
        }

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

    while (!current->is(TOKEN_END_OF_FILE)) {
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

                Token value = parse_expresion(ParsePrecedence::Start);
                if (context.unknown_label) {
                    break;
                }

                make_symbol(*name, value.u, name->source_file, name->line);
            }
            else {
                MAKE_ERROR((*name), return, "invalid expresion");
            }
        }
            break;
        case TOKEN_DIRECTIVE:
            goto_next_line();
            break;
        case TOKEN_INSTRUCTION:
        {
            u32 line = current->line;
            while (line == current->line && !current->is(TOKEN_END_OF_FILE)) {
                if (current->is(TOKEN_SYMBOL) && current->str[0] == '.') {
                    std::string composed = std::string(last_label) + std::string(current->str);
                    auto it = find_label(composed);
                    if (it != symbols.end()) {
                        current->str = it->second.symbol;
                    }
                    else {
                        MAKE_ERROR((*current), {}, "undefined reference to '%.*s'", current->str.length(), current->str.data());
                    }
                }

                advance();
            }
        }
            break;
        case TOKEN_LABEL:
        {
            Symbol& symbol = make_label(*current, u64(-1), current->source_file, current->line);
            symbol.is_defined = false;
            current->str = symbol.symbol;
            advance();
        }
            break;
        default:
            advance();
            break;
        }
    }

}

Symbol& Assembler::make_label(const Token& label, u64 address, const char* source_file, u32 line) {
    std::string composed = {};

    if (label.str[0] == '.') {
        composed = std::string(last_label) + std::string(label.str);
    }
    else {
        composed = label.str;
        last_label = label.str;
    }

    auto it = symbols.find(composed);
    if (it != symbols.end()) {
        if (address != u64(-1)) {
            it->second.uvalue = address;
            return it->second;
        }
        else {
            MAKE_ERROR(label, return it->second, "'%.*s' is already defined", label.str.length(), label.str.data());
        }
    }

    auto& l = symbols.emplace(
        composed,
        Symbol()
    ).first->second;

    l.symbol = std::move(composed);
    l.uvalue = address;
    l.source_file = source_file;
    l.line = line;

    return l;
}

Symbol& Assembler::make_symbol(const Token& label, u64 value, const char* source_file, u32 line) {
    std::string str_name = std::string(label.str);
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

    while (!current->is(TOKEN_END_OF_FILE)) {
        context.undefined_label = false;

        if (ErrorManager::must_syncronize) {
            syncronize();
            if (current->is(TOKEN_END_OF_FILE))
            {
                continue;
            }
        }

        switch (current->type) {
        case TOKEN_SYMBOL:
        {
            Token* name = current;
            advance();

            auto it = find_label(name->str);
            if (it != symbols.end()) {
                if (it->second.is_defined) {
                    goto_next_line();
                }
            }

            if (current->is(TOKEN_EQUAL)) {
                advance();

                Token value = parse_expresion(ParsePrecedence::Start);
                if (context.unknown_label) {
                    break;
                }

                make_symbol(*name, value.u, name->source_file, name->line);
            }
            else {
                MAKE_ERROR((*name), return, "invalid expresion");
            }
        }
            break;
        case TOKEN_DIRECTIVE:
            assemble_directive();
            break;
        case TOKEN_NEW_LINE:
            advance();
            break;
        case TOKEN_LABEL:
        {
            u32 address = u64(origin_address + program_index - last_size);
            find_label(current->str)->second.uvalue = address;
            advance();
        }
            break;
        case TOKEN_INSTRUCTION:
            assemble_instruction();
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

        assemble_instruction();
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
}

void Assembler::syncronize()
{
    ErrorManager::must_syncronize = false;

    while (true) {
        switch (current->type)
        {
        case TOKEN_DIRECTIVE:
            if (current->subtype == TD_FORMAT) {
                return;
            }
            break;
        case TOKEN_INSTRUCTION:
            break;
        case TOKEN_SYMBOL:
            if (next->is(TOKEN_EQUAL)) {
                return;
            }
            break;
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

void Assembler::goto_next_line()
{
    u32 line = current->line;
    while (line == current->line && !current->is(TOKEN_END_OF_FILE)) {
        advance();
    }
}

void Assembler::advance_to_next_line() {
    while (!current->is(TOKEN_NEW_LINE)) {
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

u32& Assembler::new_word() {
    check_capacity(4);
    program_index += 4;
    return *(u32*)(&program[(u64)program_index - 4]);
}

u16& Assembler::new_half() {
    check_capacity(2);
    program_index += 2;
    return *(u16*)(&program[(u64)program_index - 2]);
}

u8& Assembler::new_byte() {
    check_capacity(1);
    program_index += 1;
    return program[(u64)program_index-1];
}

u8* Assembler::reserve(u32 count)
{
    check_capacity(count);

    u8& address = new_byte();
    for (u32 i = 1; i < count; i++) {
        new_byte();
    }

    return &address;
}

void Assembler::check_capacity(u32 count) {
    if (program_index + count >= (u32)program.size()) {
        program.resize(program.size() + (program.size()) / 2);
    }
}

std::unordered_map<std::string, Symbol>::iterator Assembler::find_label(const std::string_view label) {
    auto it = symbols.find(std::string(label));
    if (it == symbols.end()) {
        return symbols.end();
    }

    return it;
}
