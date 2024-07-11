#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include <fstream>

i32 Assembler::assemble_file(const char* file_path, const char* output_path, u32 origin)
{
    origin_address = origin;
    pre_processor.process(file_path);
    if (ErrorManager::is_panic_mode) {
        return ERROR;
    }

    pre_processor.source_index = 0;

    reserve(sizeof(RomHeader));

    u32 initial_size = (u32)program.size();
    phase1();
    program.resize(size_t(initial_size));
    
    phase2();

    // write
    std::ofstream output{ output_path, std::ios::binary };

    auto it = pre_processor.global_scope().labels.find(
        std::string_view((char*)entry_point.symbol.ptr, entry_point.symbol.len)
    );
    if (it == pre_processor.global_scope().labels.end()) {
        ErrorManager::error(
            entry_point.source_file, entry_point.line,
            "the entry point was not defined"
        );
        return ERROR;
    }
    
    entry_point_address = it->second.address;
    RomHeader* header = (RomHeader*)program.data();
    header->magic = RomSignature;
    header->check_sum = u32(program.size());
    header->address_of_entry_point = entry_point_address / 4;

    output.write((char*)program.data(), program.size());

    output.close();

    return current_status;
}

void Assembler::phase1() {
    pre_processor.current_scope().token_index = 0;

    advance();
    advance();

    while (current->is_not(TOKEN_END_OF_FILE)) {
        if (ErrorManager::must_syncronize) {
            syncronize();
        }

        switch (current->type) {
        case TOKEN_DIRECTIVE:
            advance();
            switch (last->subtype) {
            case TD_INCLUDE:
            {
                Token* last_current = current;
                Token* last_next = next;

                u32 last_scope = pre_processor.current_scope_index;
                pre_processor.current_scope_index = last->include;

                phase1();

                pre_processor.current_scope_index = last_scope;
                current = last_current;
                next = last_next;
            }
                break;
            case TD_STRING:
            {
                Token string = parse_expresion(ParsePrecedence::Assignment);
                program.resize(program.size() + string.str.len);
            }
                break;
            case TD_BYTE:
                new_byte();
                break;
            case TD_HALF:
                new_half();
                break;
            case TD_WORD:
                new_word();
                break;
            case TD_DWORD:
                new_word();
                new_word();
                break;
            case TD_ZERO:
            {
                Token count = parse_expresion(ParsePrecedence::Assignment);
                program.resize(program.size() + count.u);
            }
                break;
            }
            break;
        case TOKEN_LABEL_LOCAL:
            assemble_label();
            break;
        case TOKEN_LABEL:
            assemble_global_label();
            break;
        case TOKEN_INSTRUCTION:
            new_word();
            advance();
            break;
        default:
            advance();
            break;
        }
    }

}

void Assembler::assemble_label()
{
    auto& l = pre_processor.current_scope().labels.emplace(
        std::string_view((char*)current->str.ptr, current->str.len),
        Label()
    ).first->second;

    l.symbol = current->str;
    l.address = origin_address + u32(program.size());
    l.source_file = current->source_file;
    l.line = current->line;

    advance();
}

void Assembler::assemble_global_label() {
    auto& l = pre_processor.global_scope().labels.emplace(
        std::string_view((char*)current->str.ptr, current->str.len),
        Label()
    ).first->second;

    l.symbol = current->str;
    l.address = origin_address + u32(program.size());
    l.source_file = current->source_file;
    l.line = current->line;

    advance();
}

void Assembler::phase2() {
    pre_processor.current_scope().token_index = 0;

    advance();
    advance();

    while (current->is_not(TOKEN_END_OF_FILE)) {
        if (ErrorManager::must_syncronize) {
            syncronize();
        }

        switch (current->type) {
        case TOKEN_DIRECTIVE:
            assemble_directive();
            break;
        case TOKEN_NEW_LINE:
            advance();
            break;
        case TOKEN_LABEL:
        case TOKEN_LABEL_LOCAL:
            advance();
            break;
        case TOKEN_INSTRUCTION:
            assemble_instruction();
            break;
        default:
            ErrorManager::error(current->source_file, current->line, "invalid token");
            current_status = ERROR;
            return;
        }
    }
}

void Assembler::advance()
{
    last = current;
    current = next;
    if (pre_processor.current_scope().token_index < pre_processor.current_scope().tokens.size()) {
        next = &pre_processor.current_scope().tokens[pre_processor.current_scope().token_index++];
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
        current_status = ERROR;
        return false;
    }

    return true;
}

void Assembler::goto_next_line()
{
    u32 line = current->line;
    while (line == current->line && current->is_not(TOKEN_END_OF_FILE)) {
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

u16& Assembler::new_half() {
    program.emplace_back();
    program.emplace_back();
    return *(u16*)(&program[program.size() - 2]);
}

u8& Assembler::new_byte() {
    return program.emplace_back();
}

u8* Assembler::reserve(u32 count)
{
    for (u32 i = 0; i < count; i++)
        program.emplace_back();
    return &program[program.size() - count];
}

std::unordered_map<std::string_view, Label>::iterator Assembler::find_in_scope(SourceScope& scope,
    const TokenView& label, bool& founded) {
    auto it = scope.labels.find(std::string_view((char*)label.ptr, label.len));
    founded = true;
    if (it == scope.labels.end()) {
        founded = false;

        if (scope.parent_scope_index < pre_processor.sources.size()) {
            return find_in_scope(pre_processor.sources[scope.parent_scope_index], label, founded);
        }
    }

    return it;
}
