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

        resolve_pending();
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

void Assembler::phase1()
{
    token_index = 0;

    advance();
    advance();

    while (!current->is(TOKEN_END_OF_FILE))
    {
        if (ErrorManager::must_synchronize)
        {
            synchronize();
        }

        switch (current->type)
        {
        case TOKEN_SYMBOL:
        {
            AsmToken* name = current;
            advance();

            if (current->is(TOKEN_EQUAL))
            {
                advance();

                AsmToken value = parse_expression(ParsePrecedence::Start);
                if (context.unknown_label)
                    break;

                make_symbol(*name, value.u, name->source_file, name->line);
            }
            else
            {
                MAKE_ERROR((*name), return, "invalid expression");
            }
        }
        break;
        case TOKEN_DIRECTIVE:
            goto_next_line();
            break;
        case TOKEN_INSTRUCTION:
        {
            u32 line = current->line;
            while (line == current->line && !current->is(TOKEN_END_OF_FILE))
            {
                if (current->is(TOKEN_SYMBOL) && current->get_str()[0] == '.')
                {
                    std::string composed = std::string(StringPool::get(last_label)) + std::string(current->get_str());
                    StringID composed_id = StringPool::get_or_insert(composed);
                    current->str = composed_id;
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

Symbol& Assembler::make_label(const AsmToken& label, u64 address, StringID source_file, u32 line)
{
    std::string composed = {};

    if (label.get_str()[0] == '.')
    {
        composed = std::string(StringPool::get(last_label)) + std::string(label.get_str());
    }
    else
    {
        composed = label.get_str();
        last_label = label.str;
    }

    StringID composed_id = StringPool::get_or_insert(composed);
    auto it = symbols.find(composed_id);
    if (it != symbols.end())
    {
        if (address != u64(-1))
        {
            it->second.uvalue = address;
            return it->second;
        }
        else
        {
            MAKE_ERROR(label, return it->second, "'%.*s' is already defined", label.get_str().length(), label.get_str().data());
        }
    }

    auto& l = symbols.emplace(
        composed_id,
        Symbol()
    ).first->second;

    l.symbol = composed_id;
    l.uvalue = address;
    l.source_file = source_file;
    l.line = line;

    return l;
}

Symbol& Assembler::make_symbol(const AsmToken& label, u64 value, StringID source_file, u32 line)
{
    std::string composed = {};

    if (label.get_str()[0] == '.')
    {
        composed = std::string(StringPool::get(last_label)) + std::string(label.get_str());
    }
    else
    {
        composed = label.get_str();
    }

    StringID composed_id = StringPool::get_or_insert(composed);
    auto it = symbols.find(composed_id);
    if (it != symbols.end())
    {
        auto& symbol = it->second;
        return symbol;
    }

    auto& symbol = symbols.emplace(
        composed_id, Symbol()
    ).first->second;

    symbol.uvalue = value;
    symbol.symbol = composed_id;
    symbol.source_file = source_file;
    symbol.line = line;
    return symbol;
}

void Assembler::phase2()
{
    token_index = 0;

    advance();
    advance();

    while (!current->is(TOKEN_END_OF_FILE))
    {
        context.undefined_label = false;
        context.unknown_label = false;

        if (ErrorManager::must_synchronize)
        {
            synchronize();
            if (current->is(TOKEN_END_OF_FILE))
            {
                continue;
            }
        }

        switch (current->type)
        {
        case TOKEN_SYMBOL:
        {
            AsmToken* name = current;
            advance();

            auto it = find_label_or_symbol(name->str);
            if (it != symbols.end())
            {
                if (it->second.is_defined)
                {
                    goto_next_line();
                }
            }

            if (current->is(TOKEN_EQUAL))
            {
                advance();

                AsmToken value = parse_expression(ParsePrecedence::Start);
                if (context.unknown_label)
                {
                    break;
                }

                make_symbol(*name, value.u, name->source_file, name->line);
            }
            else
            {
                MAKE_ERROR((*name), return, "invalid expression");
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
            find_label_or_symbol(current->str)->second.uvalue = address;
            advance();
        }
        break;
        case TOKEN_INSTRUCTION:
            assemble_instruction();
            break;
        default:
            ErrorManager::error(current->get_source_file().data(), current->line, "invalid token");
            return;
        }
    }
}

void Assembler::resolve_pending()
{
    context.is_in_resolve = true;
    u32 index = program_index;

    for (auto& tr : to_resolve)
    {
        context.undefined_label = false;
        context.unknown_label = false;

        program_index = tr.address;

        token_index = tr.index;
        advance();
        advance();

        if (tr.type == ResolveInstruction)
            assemble_instruction();
        else if(tr.type == ResolveDirective)
            assemble_directive();
    }

    program_index = index;
}

void Assembler::advance()
{
    last = current;
    current = next;
    if (token_index < pre_processor.tokens.size())
    {
        next = &pre_processor.tokens[token_index++];
    }
}

void Assembler::synchronize()
{
    ErrorManager::must_synchronize = false;

    while (true)
    {
        switch (current->type)
        {
        case TOKEN_DIRECTIVE:
            if (current->subtype == TD_FORMAT)
            {
                return;
            }
            break;
        case TOKEN_INSTRUCTION:
            return;
        case TOKEN_SYMBOL:
            if (next->is(TOKEN_EQUAL))
            {
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

bool Assembler::expected(AsmTokenType tk, const char* format, ...)
{
    advance();
    if (last->type != tk)
    {
        va_list args;
        va_start(args, format);
        ErrorManager::errorV(last->get_source_file().data(), last->line, format, args);
        va_end(args);
        return false;
    }

    return true;
}

bool Assembler::expectedv(AsmTokenType tk, const char* format, va_list va)
{
    advance();
    if (last->type != tk)
    {
        ErrorManager::errorV(last->get_source_file().data(), last->line, format, va);
        return false;
    }

    return true;
}

bool Assembler::expected_comma()
{
    return expected(TOKEN_COMMA, "',' was expected");
}

bool Assembler::expected_left_key()
{
    return expected(TOKEN_LEFT_KEY, "'[' was expected");
}

bool Assembler::expected_right_key()
{
    return expected(TOKEN_RIGHT_KEY, "']' was expected");
}

void Assembler::goto_next_line()
{
    u32 line = current->line;
    while (line == current->line && !current->is(TOKEN_END_OF_FILE)) {
        advance();
    }
}

void Assembler::advance_to_next_line()
{
    while (!current->is(TOKEN_NEW_LINE))
    {
        advance();
    }
}

u16 Assembler::get_register(AsmToken tk)
{
    if (tk.subtype >= TOKEN_R0 && tk.subtype <= TOKEN_R31)
        return u8(tk.subtype);
    else if (tk.subtype >= TOKEN_S0 && tk.subtype <= TOKEN_S31)
        return u8(tk.subtype - TOKEN_S0);
    else if (tk.subtype >= TOKEN_D0 && tk.subtype <= TOKEN_D31)
        return u8(tk.subtype - TOKEN_D0);
    else if (tk.subtype >= TOKEN_V0 && tk.subtype <= TOKEN_V31)
        return u8(tk.subtype - TOKEN_V0);
    else if (tk.subtype >= TOKEN_V0_S4 && tk.subtype <= TOKEN_V31_S4)
        return u8(tk.subtype - TOKEN_V0_S4);
    else if (tk.subtype >= TOKEN_V0_D2 && tk.subtype <= TOKEN_V31_D2)
        return u8(tk.subtype - TOKEN_V0_D2);
    else if (tk.subtype >= TOKEN_PSR && tk.subtype <= TOKEN_END_SYSTEM_REGS)
        return u8(tk.subtype - TOKEN_PSR);

    MAKE_ERROR(tk, return u16(-1), "invalid register name");
}

bool Assembler::try_get_register(u16& reg, RegisterType reg_type, const char* format, ...)
{
    va_list va;
    va_start(va, format);
    if (!expectedv(TOKEN_REGISTER, format, va))
    {
        va_end(va);
        return false;
    }
    va_end(va);

    if (reg_type == RegisterGP && !last->is_gp_reg())
    {
        if (last->is_fp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a gp register but a fp register was given");
        }
        else if (last->is_vector_reg())
        {
            MAKE_ERROR((*last), return false, "expected a gp register but a vector register was given");
        }
        else if (last->is_system_reg())
        {
            MAKE_ERROR((*last), return false, "expected a gp register but a system register was given");
        }
    }
    
    if (reg_type == RegisterFP && !last->is_fp_reg())
    {
        if (last->is_gp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a fp register but a gp register was given");
        }
        else if (last->is_vector_reg())
        {
            MAKE_ERROR((*last), return false, "expected a fp register but a vector register was given");
        }
        else if (last->is_system_reg())
        {
            MAKE_ERROR((*last), return false, "expected a gp register but a system register was given");
        }
    }
    
    if (reg_type == RegisterVector && !last->is_vector_reg())
    {
        if (last->is_gp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a vector register but a gp register was given");
        }
        else if (last->is_fp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a vector register but a fp register was given");
        }
        else if (last->is_system_reg())
        {
            MAKE_ERROR((*last), return false, "expected a gp register but a system register was given");
        }
    }

    if (reg_type == RegisterFPOrVector)
    {
        if (last->is_gp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a fp or vector register but a gp register was given");
        }
        else if (last->is_system_reg())
        {
            MAKE_ERROR((*last), return false, "expected a fp or vector register but a system register was given");
        }
    }

    if (reg_type == RegisterSysReg)
    {
        if (last->is_gp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a system register but a gp register was given");
        }
        else if (last->is_fp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a system register but a fp register was given");
        }
        else if (last->is_vector_reg())
        {
            MAKE_ERROR((*last), return false, "expected a system register but a vector register was given");
        }
    }

    reg = get_register(*last);
    return true;
}

bool Assembler::try_get_register_tk(AsmToken tk, u16& reg, RegisterType reg_type)
{
    if (reg_type == RegisterGP && !tk.is_gp_reg())
    {
        if (tk.is_fp_reg())
        {
            MAKE_ERROR(tk, return false, "expected a gp register but a fp register was given");
        }
        else if (tk.is_vector_reg())
        {
            MAKE_ERROR(tk, return false, "expected a gp register but a vector register was given");
        }
    }

    if (reg_type == RegisterFP && !tk.is_fp_reg())
    {
        if (tk.is_gp_reg())
        {
            MAKE_ERROR(tk, return false, "expected a fp register but a gp register was given");
        }
        else if (tk.is_vector_reg())
        {
            MAKE_ERROR(tk, return false, "expected a fp register but a vector register was given");
        }
    }

    if (reg_type == RegisterVector && !tk.is_vector_reg())
    {
        if (tk.is_gp_reg())
        {
            MAKE_ERROR(tk, return false, "expected a vector register but a gp register was given");
        }
        else if (tk.is_fp_reg())
        {
            MAKE_ERROR(tk, return false, "expected a vector register but a fp register was given");
        }
    }

    if (reg_type == RegisterFPOrVector)
    {
        if (last->is_gp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a fp or vector register but a gp register was given");
        }
        else if (last->is_system_reg())
        {
            MAKE_ERROR((*last), return false, "expected a fp or vector register but a system register was given");
        }
    }

    if (reg_type == RegisterSysReg)
    {
        if (last->is_gp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a system register but a gp register was given");
        }
        else if (last->is_fp_reg())
        {
            MAKE_ERROR((*last), return false, "expected a system register but a fp register was given");
        }
        else if (last->is_vector_reg())
        {
            MAKE_ERROR((*last), return false, "expected a system register but a vector register was given");
        }
    }

    reg = get_register(tk);
    return true;
}

u32& Assembler::new_word()
{
    check_capacity(4);
    program_index += 4;
    return *(u32*)(&program[(u64)program_index - 4]);
}

u16& Assembler::new_half()
{
    check_capacity(2);
    program_index += 2;
    return *(u16*)(&program[(u64)program_index - 2]);
}

u8& Assembler::new_byte()
{
    check_capacity(1);
    program_index += 1;
    return program[(u64)program_index - 1];
}

u8* Assembler::reserve(u32 count)
{
    check_capacity(count);

    u8& address = new_byte();
    for (u32 i = 1; i < count; i++)
    {
        (void)new_byte();
    }

    return &address;
}

void Assembler::check_capacity(u32 count)
{
    if (program_index + count >= (u32)program.size())
    {
        program.resize(program.size() + (program.size()) / 2);
    }
}

std::unordered_map<StringID, Symbol>::iterator Assembler::find_label_or_symbol(StringID label)
{
    auto it = symbols.find(label);
    if (it != symbols.end())
    {
        return it;
    }

    return symbols.end();
}
