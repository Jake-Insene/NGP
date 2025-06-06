/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Backend/Assembler.h"
#include "ErrorManager.h"

#define EXPECTED_COMMA(BREAKER) \
    if (!expected(TOKEN_COMMA, "',' was expected"))\
    {\
        BREAKER;\
    }

#define EXPECTED_KEY_LEFT(BREAKER) \
    if (!expected(TOKEN_LEFT_KEY, "'[' was expected"))\
    {\
        BREAKER;\
    }

#define EXPECTED_KEY_RIGHT(BREAKER) \
    if (!expected(TOKEN_RIGHT_KEY, "']' was expected"))\
    {\
        BREAKER;\
    }

#define GET_REG(var, message, BREAKER) \
    if(!expected(TOKEN_REGISTER, message))\
    {\
        BREAKER;\
    }\
    u8 var = get_register(*last)

#define INVALIDATE_FP(TOKEN, BREAKER) \
    if (TOKEN.is_fpreg())\
    {\
        MAKE_ERROR(TOKEN, BREAKER, "expected a gp register but a fp register was given");\
    }

#define BCOND(type_inst, ti)\
    {\
    u32 index = token_index - 3;\
    auto target = parse_expression(ParsePrecedence::Start);\
    if (context.undefined_label == false)\
    {\
        inst = bcond(type_inst, i32(target.u - program_index) / 4);\
    }\
    else if(context.is_in_resolve == false)\
    {\
        auto& tr = to_resolve.emplace_back();\
        tr.address = u32(program_index - 4);\
        tr.index = index;\
        advance_to_next_line();\
    }\
    }


#define RELATIVE(type_inst, ti) \
    {\
    u32 index = token_index - 3;\
    auto target = parse_expression(ParsePrecedence::Start);\
    if (context.undefined_label == false)\
    {\
        inst = type_inst(i32(target.u - program_index) / 4);\
    }\
    else if(context.is_in_resolve == false)\
    {\
        auto& tr = to_resolve.emplace_back();\
        tr.address = u32(program_index - 4);\
        tr.index = index;\
        advance_to_next_line();\
    }\
    }



void Assembler::assemble_instruction()
{
    advance(); // inst

    if (align_up(u32(program_index), 4) != program_index && !ErrorManager::is_panic_mode)
    {
        MAKE_ERROR((*last), {}, "bad file alignment");
    }
    u32& inst = new_word();

    switch (last->subtype)
    {
    case TI_NOP:
        inst = non_binary(NGP_NOP, 0, 0, 0);
        break;
    case TI_BL:
        RELATIVE(bl, TI_BL);
        break;
    case TI_B:
    {
        RELATIVE(b, TI_B);
    };
        break;
    case TI_ADR:
    {
        u32 index = token_index - 3;
        GET_REG(dest, "a register was expected!", break);
        INVALIDATE_FP((*last), break);
        EXPECTED_COMMA(break);

        Token symbol = parse_expression(ParsePrecedence::Start);
        if (context.undefined_label == false)
        {
            inst = pcrel(NGP_ADR_PC, dest, i32(symbol.u - program_index) / 4);
        }
        else if (context.is_in_resolve == false)
        {
            auto& tr = to_resolve.emplace_back();
            tr.address = u32(program_index - 4);
            tr.index = index;
            advance_to_next_line();
        }
    }
    break;
    case TI_BEQ:
        BCOND(NGP_BEQ, TI_BEQ);
        break;
    case TI_BNE:
        BCOND(NGP_BNE, TI_BNE);
        break;
    case TI_BLT:
        BCOND(NGP_BLT, TI_BLT);
        break;
    case TI_BLE:
        BCOND(NGP_BLE, TI_BLE);
        break;
    case TI_BGT:
        BCOND(NGP_BGT, TI_BGT);
        break;
    case TI_BGE:
        BCOND(NGP_BGE, TI_BGE);
        break;
    case TI_BCS:
        BCOND(NGP_BCS, TI_BCS);
        break;
    case TI_BCC:
        BCOND(NGP_BCC, TI_BCC);
        break;
    case TI_BMI:
        BCOND(NGP_BMI, TI_BMI);
        break;
    case TI_BPL:
        BCOND(NGP_BPL, TI_BPL);
        break;
    case TI_BVS:
        BCOND(NGP_BVS, TI_BVS);
        break;
    case TI_BVC:
        BCOND(NGP_BVC, TI_BVC);
        break;
    case TI_BHI:
        BCOND(NGP_BHI, TI_BHI);
        break;
    case TI_BLS:
        BCOND(NGP_BLS, TI_BLS);
        break;
    case TI_BAL:
        BCOND(NGP_BAL, TI_BAL);
        break;
    case TI_ADD:
        assemble_binary(inst, NGP_ADD_SHL, NGP_ADD_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_ADDS:
        assemble_binary(inst, NGP_ADDS_SHL, NGP_ADDS_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_SUB:
        assemble_binary(inst, NGP_SUB_SHL, NGP_SUB_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_SUBS:
        assemble_binary(inst, NGP_SUBS_SHL, NGP_SUBS_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_ADC:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return alu(NGP_ADC, dest, src1, src2, 0);
            }
        );
        break;
    case TI_ADCS:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return alu(NGP_ADCS, dest, src1, src2, 0);
            }
        );
        break;
    case TI_SBC:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return alu(NGP_SBC, dest, src1, src2, 0);
            }
        );
        break;
    case TI_SBCS:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return alu(NGP_SBCS, dest, src1, src2, 0);
            }
        );
        break;
    case TI_AND:
        assemble_binary(inst, NGP_AND_SHL, NGP_AND_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_ANDS:
        assemble_binary(inst, NGP_ANDS_SHL, NGP_ANDS_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_OR:
        assemble_binary(inst, NGP_OR_SHL, NGP_OR_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_ORN:
        assemble_binary(inst, NGP_ORN_SHL, NGP_ORN_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_EOR:
        assemble_binary(inst, NGP_EOR_SHL, NGP_EOR_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_SHL:
        assemble_shift(inst, NGP_SHL);
        break;
    case TI_SHR:
        assemble_shift(inst, NGP_SHR);
        break;
    case TI_ASR:
        assemble_shift(inst, NGP_ASR);
        break;
    case TI_ROR:
        assemble_shift(inst, NGP_ROR);
        break;
    case TI_BIC:
        assemble_binary(inst, NGP_BIC_SHL, 0, u16(-1), 0, true);
        break;
    case TI_BICS:
        assemble_binary(inst, NGP_BICS_SHL, 0, u16(-1), 0, true);
        break;
    case TI_CMP:
        assemble_comparision(inst, NGP_SUBS_SHL, NGP_SUBS_IMMEDIATE, 0xFFFF);
        break;
    case TI_CMN:
        assemble_comparision(inst, NGP_ADDS_SHL, NGP_ADDS_IMMEDIATE, 0xFFFF);
        break;
    case TI_TST:
        assemble_comparision(inst, NGP_ANDS_SHL, NGP_ANDS_IMMEDIATE, 0xFFFF);
        break;
    case TI_NOT:
        assemble_one_operand(inst, [](u8 dest, u8 src)
            {
                return alu(NGP_ORN_SHL, dest, ZeroRegister, src, 0);
            }
        );
        break;
    case TI_NEG:
        assemble_one_operand(inst, [](u8 dest, u8 src)
            {
                return alu(NGP_SUB_SHL, dest, ZeroRegister, src, 0);
            }
        );
        break;
    case TI_ABS:
        assemble_one_operand(inst, [](u8 dest, u8 src)
            {
                return extendedalu(NGP_ABS, dest, src, 0, 0);
            }
        );
        break;
    case TI_LD:
        assemble_load_store(inst, NGP_LD_IMMEDIATE, NGP_LD, 4, true);
        break;
    case TI_LDSH:
        assemble_load_store(inst, NGP_LDSH_IMMEDIATE, NGP_LDSH, 2, false);
        break;
    case TI_LDH:
        assemble_load_store(inst, NGP_LDH_IMMEDIATE, NGP_LDH, 2, false);
        break;
    case TI_LDSB:
        assemble_load_store(inst, NGP_LDSB_IMMEDIATE, NGP_LDSB, 1, false);
        break;
    case TI_LDB:
        assemble_load_store(inst, NGP_LDB_IMMEDIATE, NGP_LDB, 1, false);
        break;
    case TI_ST:
        assemble_load_store(inst, NGP_ST_IMMEDIATE, NGP_ST, 4, false);
        break;
    case TI_STH:
        assemble_load_store(inst, NGP_STH_IMMEDIATE, NGP_STH, 2, false);
        break;
    case TI_STB:
        assemble_load_store(inst, NGP_STB_IMMEDIATE, NGP_STB, 1, false);
        break;
    case TI_MADD:
        assemble_three_operands(inst, [](u8 dest, u8 src1, u8 src2, u8 src3)
            {
                return extendedalu(NGP_MADD, dest, src1, src2, src3);
            }
        );
        break;
    case TI_MSUB:
        assemble_three_operands(inst, [](u8 dest, u8 src1, u8 src2, u8 src3)
            {
                return extendedalu(NGP_MSUB, dest, src1, src2, src3);
            }
        );
        break;
    case TI_MUL:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return extendedalu(NGP_MADD, dest, src1, src2, ZeroRegister);
            }
        );
        break;
    case TI_DIV:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return extendedalu(NGP_DIV, dest, src1, src2, 0);
            }
        );
        break;
    case TI_UDIV:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return extendedalu(NGP_UDIV, dest, src1, src2, 0);
            }
        );
        break;
    case TI_MOV:
    {
        GET_REG(dest, "expected destination register", break);
        INVALIDATE_FP((*last), break);
        EXPECTED_COMMA(break);

        Token operand = parse_expression(ParsePrecedence::Start);
        if (operand.is(TOKEN_REGISTER))
        {
            INVALIDATE_FP(operand, break);
            inst = alu(NGP_OR_SHL, dest, ZeroRegister, get_register(operand), 0);
        }
        else if (operand.is(TOKEN_IMMEDIATE))
        {
            if (operand.u > 0xFFFF)
            {
                MAKE_ERROR(operand, break, "immediate value too long");
            }
            inst = binaryi(NGP_OR_IMMEDIATE, dest, ZeroRegister, operand.ushort[0]);
        }
        else if (operand.is(TOKEN_NEW_LINE))
        {
            MAKE_ERROR(operand, {}, "expected a source operand");
        }
        else
        {
            MAKE_ERROR(operand, {}, "invalid operand");
        }
    }
    break;
    case TI_MOVT:
    {
        GET_REG(dest, "expected destination register", break);
        INVALIDATE_FP((*last), break);
        EXPECTED_COMMA(break);

        Token operand = parse_expression(ParsePrecedence::Start);
        if (operand.is(TOKEN_IMMEDIATE))
        {
            if (operand.u > 0xFFFF)
            {
                MAKE_ERROR(operand, break, "immediate value too long");
            }
            inst = iimmediate(NGP_MOVT_IMMEDIATE, dest, operand.ushort[0]);
        }
        else if (operand.is(TOKEN_NEW_LINE))
        {
            MAKE_ERROR(operand, {}, "expected a source operand");
        }
        else
        {
            MAKE_ERROR(operand, {}, "invalid operand");
        }
    }
    break;
    case TI_MVN:
    {
        GET_REG(dest, "expected destination register", break);
        INVALIDATE_FP((*last), break);
        EXPECTED_COMMA(break);

        Token operand = parse_expression(ParsePrecedence::Start);
        if (operand.is(TOKEN_IMMEDIATE))
        {
            if (operand.u > 0xFFFF)
            {
                MAKE_ERROR(operand, break, "immediate value too long");
            }
            inst = iimmediate(NGP_MVN_IMMEDIATE, dest, operand.ushort[0]);
        }
        else if (operand.is(TOKEN_NEW_LINE))
        {
            MAKE_ERROR(operand, {}, "expected a source operand");
        }
        else
        {
            MAKE_ERROR(operand, {}, "invalid operand");
        }
    }
    break;
    case TI_RET:
    {
        inst = non_binary(NGP_RET, 0, 30, 0);
    }
        break;
    case TI_BLR:
    {
        GET_REG(reg, "a target branch register was expected", break);
        inst = non_binary(NGP_BLR, 0, reg, 0);
    }
        break;
    case TI_BR:
    {
        GET_REG(reg, "a target branch register was expected", break);
        inst = non_binary(NGP_BLR, 0, reg, 0);
    }
        break;
    case TI_ERET:
    {
        inst = non_binary(NGP_ERET, 0, 0, 0);
    }
        break;
    case TI_BRK:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected!"))
        {
            break;
        }

        inst = exception(NGP_BRK, (u16)last->uword);
    }
    break;
    case TI_SVC:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected!"))
        {
            break;
        }

        inst = exception(NGP_SVC, (u16)last->uword);
    }
    break;
    case TI_EVC:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected!"))
        {
            break;
        }

        inst = exception(NGP_EVC, (u16)last->uword);
    }
    break;
    case TI_SMC:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected!"))
        {
            break;
        }

        inst = exception(NGP_SMC, (u16)last->uword);
    }
    break;
    case TI_HALT:
        inst = exception(NGP_HALT, 0);
        break;
    default:
        MAKE_ERROR((*last), {}, "invalid token");
        break;
    }

    if (ErrorManager::is_panic_mode)
    {
        while (!current->is(TOKEN_END_OF_FILE) && !current->is(TOKEN_NEW_LINE))
        {
            advance();
        }
    }

    if (!current->is(TOKEN_END_OF_FILE))
    {
        expected(TOKEN_NEW_LINE, "a new line was expected");
    }
}

void Assembler::assemble_load_store(u32& inst, u8 imm_opcode,
    u8 index_opc, u8 alignment, bool handle_symbol)
{
    u32 index = token_index - 3;

    GET_REG(dest, "expected source register", return);

    u8 fp_type = 0;
    if (last->is_fpreg())
    {
        if (last->is_single_reg())
        {
            fp_type = 1;
        }
        else if (last->is_double_reg())
        {
            fp_type = 2;
        }
        else if (last->is_qword_reg())
        {
            fp_type = 3;
        }
    }
    EXPECTED_COMMA(return);

    if (handle_symbol && !current->is(TOKEN_LEFT_KEY))
    {
        Token symbol = parse_expression(ParsePrecedence::Start);
        if (context.undefined_label == false)
        {
            if (fp_type == 1)
            {
                inst = pcrel(NGP_LD_PC, dest, i32((symbol.u - program_index) / 4));
            }
            else if (fp_type == 2)
            {
                inst = pcrel(NGP_LD_PC, dest, i32((symbol.u - program_index) / 4));
            }
            else if (fp_type == 3)
            {
                inst = pcrel(NGP_LD_PC, dest, i32((symbol.u - program_index) / 4));
            }
            else
            {
                inst = pcrel(NGP_LD_PC, dest, i32((symbol.u - program_index) / 4));
            }
            return;
        }
        else if (context.is_in_resolve == false)
        {
            auto& tr = to_resolve.emplace_back();
            tr.address = u32(program_index - 4);
            tr.index = index;
            advance_to_next_line();
            return;
        }
    }

    EXPECTED_KEY_LEFT(return);
    if (!expected(TOKEN_REGISTER, "expected base register"))
    {
        return;
    }
    u8 base = get_register(*last);
    INVALIDATE_FP((*last), return);

    if (current->is(TOKEN_RIGHT_KEY))
    {
        advance();
        inst = memoryi(imm_opcode, dest, base, 0, 0);
    }
    else if (!current->is(TOKEN_NEW_LINE))
    {
        expected(TOKEN_COMMA, "',' was expected");

        Token indice = parse_expression(ParsePrecedence::Start);
        if (indice.is(TOKEN_IMMEDIATE))
        {
            if (indice.iword >= 0)
            {
                u32 aligned = align_up(indice.uword, alignment);
                if (aligned != indice.u)
                {
                    MAKE_ERROR(indice, return, "immediate value must be a multiple of %d", alignment);
                }

                if (indice.u > 0xFFF)
                {
                    MAKE_ERROR(indice, return, "immediate offset too long");
                }
            }
            else if (indice.iword < 0)
            {
                u32 aligned = align_down(indice.uword, alignment);
                if (aligned != indice.uword)
                {
                    MAKE_ERROR(indice, return, "immediate value must be a multiple of %d", alignment);
                }

                if (indice.iword < -0xFFF)
                {
                    advance();
                    MAKE_ERROR(indice, return, "immediate offset too long");
                }
            }

            // check if we need to subtract or add
            bool sub = indice.i < 0 ? 1 : 0;
            i16 offset = (i16)std::abs(indice.i);
            if (fp_type == 1)
            {
                inst = fmemoryi(NGP_LD_S_IMMEDIATE, dest, base, offset / alignment, sub);
            }
            else if (fp_type == 2)
            {
                inst = fmemoryi(NGP_LD_D_IMMEDIATE, dest, base, offset / alignment, sub);
            }
            else if (fp_type == 3)
            {
                inst = fmemoryi(NGP_LD_Q_IMMEDIATE, dest, base, offset / alignment, sub);
            }
            else
            {
                inst = memoryi(imm_opcode, dest, base, offset / alignment, sub);
            }
            EXPECTED_KEY_RIGHT(return);
        }
        else if (indice.is(TOKEN_REGISTER))
        {
            u8 reg_index = get_register(indice);
            INVALIDATE_FP(indice, return);

            if (fp_type == 1)
            {
                inst = fbinary(NGP_LD_S, dest, base, reg_index, 0);
            }
            else if (fp_type == 2)
            {
                inst = fbinary(NGP_LD_D, dest, base, reg_index, 0);
            }
            else if (fp_type == 3)
            {
                inst = fbinary(NGP_LD_Q, dest, base, reg_index, 0);
            }
            else
            {
                inst = memoryr(index_opc, dest, base, reg_index);
            }
            EXPECTED_KEY_RIGHT(return);
        }
        else if (indice.is(TOKEN_NEW_LINE))
        {
            MAKE_ERROR(indice, {}, "expected a second source operand");
        }
        else
        {
            MAKE_ERROR(indice, {}, "invalid operand");
        }
    }
    else
    {
        MAKE_ERROR((*last), return, "']' was expected");
    }
}

void Assembler::assemble_binary(u32& inst, u8 opc,
    u8 opc_imm, u16 immediate_limit, bool is_additional_opc, bool use_amount)
{
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);
    GET_REG(src1, "expected first source register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token second_operand = parse_expression(ParsePrecedence::Start);
    if (second_operand.is(TOKEN_REGISTER))
    {
        INVALIDATE_FP(second_operand, return);

        u8 adder = 0;
        u8 amount = 0;
        if (use_amount)
        {
            check_for_amount(adder, amount);
        }

        if (is_additional_opc)
        {// SHL/SHR/AST/ROR
            inst = extendedalu(opc, dest, src1, get_register(second_operand), 0);
        }
        else
        {
            inst = alu(opc, dest, src1, get_register(second_operand), amount);
        }
    }
    else if (second_operand.is(TOKEN_IMMEDIATE) && immediate_limit != 0)
    {
        if (second_operand.u > immediate_limit)
        {
            MAKE_ERROR(second_operand, return, "immediate value too long");
        }

        if (is_additional_opc)
        { // SHL/SHR/ASR/ROR
            inst = extendedalu(opc, dest, src1, (u8)last->ushort[0], 0);
        }
        else
        {
            inst = binaryi(opc_imm, dest, src1, last->ushort[0]);
        }
    }
    else if (second_operand.is(TOKEN_NEW_LINE))
    {
        MAKE_ERROR(second_operand, {}, "expected a second source operand");
    }
    else
    {
        MAKE_ERROR(second_operand, {}, "invalid operand");
    }
}

void Assembler::assemble_comparision(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit)
{
    GET_REG(src1, "expected first source register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token second_source = parse_expression(ParsePrecedence::Start);
    if (second_source.is(TOKEN_REGISTER))
    {
        INVALIDATE_FP(second_source, return);
        u8 src2 = get_register(second_source);

        u8 adder = 0;
        u8 amount = 0;
        check_for_amount(adder, amount);

        inst = alu(opc + adder, ZeroRegister, src1, src2, amount);
    }
    else if (second_source.is(TOKEN_IMMEDIATE))
    {
        if (second_source.u > immediate_limit)
        {
            MAKE_ERROR(second_source, return, "immediate value too long");
        }
        inst = binaryi(opc_imm, ZeroRegister, src1, second_source.ushort[0]);
    }
    else if (second_source.is(TOKEN_NEW_LINE))
    {
        MAKE_ERROR(second_source, {}, "expected a second source operand");
    }
    else
    {
        MAKE_ERROR(second_source, {}, "invalid operand");
    }
}

void Assembler::assemble_three_operands(u32& inst, u32(*fn)(u8, u8, u8, u8))
{
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    GET_REG(src1, "expected first source register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    GET_REG(src2, "expected second source register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token third_operand = parse_expression(ParsePrecedence::Start);
    if (third_operand.is(TOKEN_REGISTER))
    {
        INVALIDATE_FP(third_operand, return);
        inst = fn(dest, src1, src2, get_register(third_operand));
    }
    else if (third_operand.is(TOKEN_NEW_LINE))
    {
        MAKE_ERROR(third_operand, {}, "expected a third source operand");
    }
    else
    {
        MAKE_ERROR(third_operand, {}, "invalid operand");
    }
}

void Assembler::assemble_two_operands(u32& inst, u32(*fn)(u8, u8, u8))
{
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    GET_REG(src1, "expected first source register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token second_operand = parse_expression(ParsePrecedence::Start);
    if (second_operand.is(TOKEN_REGISTER))
    {
        INVALIDATE_FP(second_operand, return);
        inst = fn(dest, src1, get_register(second_operand));
    }
    else if (second_operand.is(TOKEN_NEW_LINE))
    {
        MAKE_ERROR(second_operand, {}, "expected a second source operand");
    }
    else
    {
        MAKE_ERROR(second_operand, {}, "invalid operand");
    }
}

void Assembler::assemble_one_operand(u32& inst, u32(*fn)(u8, u8))
{
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token operand = parse_expression(ParsePrecedence::Start);
    if (operand.is(TOKEN_REGISTER))
    {
        INVALIDATE_FP(operand, return);
        inst = fn(dest, get_register(operand));
    }
    else if (operand.is(TOKEN_NEW_LINE))
    {
        MAKE_ERROR(operand, {}, "expected a source operand");
    }
    else
    {
        MAKE_ERROR(operand, {}, "invalid operand");
    }
}

void Assembler::assemble_shift(u32& inst, u8 opcode)
{
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    GET_REG(src1, "expected first source operand", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token third_operand = parse_expression(ParsePrecedence::Start);
    if (third_operand.is(TOKEN_REGISTER))
    {
        INVALIDATE_FP(third_operand, return);
        inst = extendedalu(opcode, dest, src1, get_register(third_operand), 0);
    }
    else if (third_operand.is(TOKEN_IMMEDIATE))
    {
        if (third_operand.u > 0x1F)
        {
            MAKE_ERROR(third_operand, return, "shift amount too long");
        }

        u8 logopc = opcode - NGP_SHL;
        inst = alu(NGP_OR_SHL + logopc, dest, ZeroRegister, src1, third_operand.byte[0]);
    }
    else if (third_operand.is(TOKEN_NEW_LINE))
    {
        MAKE_ERROR(third_operand, {}, "expected a source operand");
    }
    else
    {
        MAKE_ERROR(third_operand, {}, "invalid operand");
    }
}

void Assembler::check_for_amount(u8& adder, u8& amount)
{
    if (!current->is(TOKEN_COMMA))
    {
        return;
    }

    advance(); // skip comma

    if (current->is(TOKEN_INSTRUCTION))
    {
        advance();
        switch (last->subtype)
        {
        case TI_SHL:
            adder = 0;
            break;
        case TI_SHR:
            adder = 1;
            break;
        case TI_ASR:
            adder = 2;
            break;
        case TI_ROR:
            adder = 3;
            break;
        default:
            ErrorManager::error(
                current->source_file.c_str(), current->line,
                "invalid shift type, try shl/shr/asr/ror"
            );
            return;
        }

        if (!expected(TOKEN_IMMEDIATE, "a amount was expected"))
        {
            return;
        }

        if (last->u > ZeroRegister)
        {
            ErrorManager::error(
                current->source_file.c_str(), current->line,
                "shift amount too long, must to be #0-31"
            );
            return;
        }

        amount = (u8)last->u;
    }
}

