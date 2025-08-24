/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Backend/Assembler.h"
#include "ErrorManager.h"
#include "FileFormat/ISA.h"
#include "Frontend/AsmToken.h"

#define GET_VINDEX_REG(VAR, VAR_TK, VAR_IDX, MESSAGE, BREAKER) \
    if(!expected(TOKEN_REGISTER, MESSAGE))\
    {\
        BREAKER;\
    }\
    AsmToken* VAR_TK = last;\
    u16 VAR;\
    if(!try_get_register_tk(*last, VAR, RegisterVector)) { BREAKER ; }\
    if(!last->is_vector_reg())\
    {\
        MAKE_ERROR((*last), BREAKER, "expected a vector register");\
    }\
    if(!expected_left_key()) { BREAKER; };\
    AsmToken VAR_IDX = parse_expression(ParsePrecedence::Start);\
    if(!VAR_IDX.is(TOKEN_IMMEDIATE))\
    {\
        MAKE_ERROR(VAR_IDX, BREAKER, "expected a constant index");\
    }\
    if(!expected_right_key()) { BREAKER; };
    

#define BCOND(TYPE_INST, TI)\
    {\
    auto target = parse_expression(ParsePrecedence::Start);\
    if (context.undefined_label == false)\
    {\
        inst = bcond(TYPE_INST, i32(target.u - program_index) / 4);\
    }\
    else if(context.is_in_resolve == false)\
    {\
        auto& tr = to_resolve.emplace_back();\
        tr.address = u32(program_index - 4);\
        tr.index = index;\
        advance_to_next_line();\
    }\
    }


#define RELATIVE(TYPE_INST, TI) \
    {\
    auto target = parse_expression(ParsePrecedence::Start);\
    if (context.undefined_label == false)\
    {\
        inst = TYPE_INST(i32(target.u - program_index) / 4);\
    }\
    else if(context.is_in_resolve == false)\
    {\
        auto& tr = to_resolve.emplace_back();\
        tr.type = ResolveInstruction;\
        tr.address = u32(program_index - 4);\
        tr.index = index;\
        advance_to_next_line();\
    }\
    }


void Assembler::assemble_instruction()
{
    advance(); // inst

    if (align_up(u32(program_index), 4) != program_index && !ErrorManager::is_panic_mode)
        MAKE_ERROR((*last), {}, "bad file alignment");

    u32 prog_index = program_index;
    u32& inst = new_word();
    u32 index = token_index - 3;

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
        u16 dest;
        if (!try_get_register(dest, RegisterGP, "expected destination register"))
            break;
        if(!expected_comma())
            break;

        AsmToken symbol = parse_expression(ParsePrecedence::Start);
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
        assemble_binary(inst, NGP_ADD, NGP_ADD_IMMEDIATE, 0xFFFF);
        break;
    case TI_ADDS:
        assemble_binary(inst, NGP_ADDS, NGP_ADDS_IMMEDIATE, 0xFFFF);
        break;
    case TI_SUB:
        assemble_binary(inst, NGP_SUB, NGP_SUB_IMMEDIATE, 0xFFFF);
        break;
    case TI_SUBS:
        assemble_binary(inst, NGP_SUBS, NGP_SUBS_IMMEDIATE, 0xFFFF);
        break;
    case TI_ADC:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return inst_3op(NGP_ADC, dest, src1, src2);
            }
        );
        break;
    case TI_ADCS:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return inst_3op(NGP_ADCS, dest, src1, src2);
            }
        );
        break;
    case TI_SBC:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return inst_3op(NGP_SBC, dest, src1, src2);
            }
        );
        break;
    case TI_SBCS:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return inst_3op(NGP_SBCS, dest, src1, src2);
            }
        );
        break;
    case TI_AND:
        assemble_binary(inst, NGP_AND, NGP_AND_IMMEDIATE, 0xFFFF);
        break;
    case TI_ANDS:
        assemble_binary(inst, NGP_ANDS, NGP_ANDS_IMMEDIATE, 0xFFFF);
        break;
    case TI_OR:
        assemble_binary(inst, NGP_OR, NGP_OR_IMMEDIATE, 0xFFFF);
        break;
    case TI_ORN:
        assemble_binary(inst, NGP_ORN, NGP_ORN_IMMEDIATE, 0xFFFF);
        break;
    case TI_EOR:
        assemble_binary(inst, NGP_EOR, NGP_EOR_IMMEDIATE, 0xFFFF);
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
        assemble_binary(inst, NGP_BIC, 0, u16(-1));
        break;
    case TI_BICS:
        assemble_binary(inst, NGP_BICS, 0, u16(-1));
        break;
    case TI_CMP:
        assemble_comparison(inst, NGP_SUBS, NGP_SUBS_IMMEDIATE, 0xFFFF);
        break;
    case TI_CMN:
        assemble_comparison(inst, NGP_ADDS, NGP_ADDS_IMMEDIATE, 0xFFFF);
        break;
    case TI_TST:
        assemble_comparison(inst, NGP_ANDS, NGP_ANDS_IMMEDIATE, 0xFFFF);
        break;
    case TI_NOT:
        assemble_one_operand(inst, [](u8 dest, u8 src)
            {
                return inst_3op(NGP_ORN, dest, ZeroRegister, src);
            }
        );
        break;
    case TI_NEG:
        assemble_one_operand(inst, [](u8 dest, u8 src)
            {
                return inst_3op(NGP_SUB, dest, ZeroRegister, src);
            }
        );
        break;
    case TI_ABS:
        assemble_one_operand(inst, [](u8 dest, u8 src)
            {
                return inst_3op(NGP_ABS, dest, src, 0);
            }
        );
        break;
    case TI_FMOV:
    {
        u16 dest;
        if (!try_get_register(dest, RegisterAny, "expected destination register"))
            break;
        AsmToken* dest_tk = last;
        AsmToken dest_index;
        if (dest_tk->is_vector_reg())
        {
            if(!expected_left_key())
                break;
            dest_index = parse_expression(ParsePrecedence::Start);
            if (!dest_index.is(TOKEN_IMMEDIATE))
            {
                MAKE_ERROR(dest_index, break, "expected a constant index");
            }
            if (!expected_right_key())
                break;
        }
        
        if (!expected_comma())
            break;

        AsmToken operand = parse_expression(ParsePrecedence::Start);

        if (operand.is(TOKEN_IMMEDIATE))
        {
            MAKE_ERROR(operand, break, "fmov immediate, not implemented yet");
        }
        else if (operand.is(TOKEN_REGISTER))
        {
            if (dest_tk->is_gp_reg() && operand.is_gp_reg())
            {
                MAKE_ERROR((*dest_tk), break, "invalid register operands");
            }
            else if ((!dest_tk->is_single_reg() && operand.is_gp_reg())
                || (dest_tk->is_gp_reg() && !operand.is_single_reg()))
            {
                MAKE_ERROR((*dest_tk), break, "register width mismatch");
            }
            else if (dest_tk->is_fp_reg() && operand.is_fp_reg() && 
                (!dest_tk->is_vector_reg() && !operand.is_vector_reg() && dest_tk->get_fp_type() != operand.get_fp_type()))
            {
                MAKE_ERROR((*dest_tk), break, "register type mismatch");
            }
            else if(dest_tk->is_vector_reg() && dest_tk->get_fp_type() == FPVector
                && operand.is_fp_reg())
            {
                MAKE_ERROR((*dest_tk), break, "register type mismatch");
            }

            if (!dest_tk->is_vector_reg() && operand.is_vector_reg())
            {
                if(!expected_left_key())
                    break;
                AsmToken src_index = parse_expression(ParsePrecedence::Start);
                if (!src_index.is(TOKEN_IMMEDIATE))
                {
                    MAKE_ERROR(src_index, break, "expected a constant index");
                }
                if (!expected_right_key())
                    break;

                if (dest_tk->is_single_reg())
                {
                    inst = fbinary(NGP_FDUP_S_V, dest, get_register(operand), src_index.u & 0x3);
                }
                else if (dest_tk->is_gp_reg())
                {
                    inst = fbinary(NGP_FUMOV_W_V, dest, get_register(operand), src_index.u & 0x3);
                }
            }
            else if (dest_tk->is_vector_reg() && operand.is_vector_reg())
            {
                if (!expected_left_key())
                    break;
                AsmToken src_index = parse_expression(ParsePrecedence::Start);
                if (!src_index.is(TOKEN_IMMEDIATE))
                {
                    MAKE_ERROR(src_index, break, "expected a constant index");
                }
                if (!expected_right_key())
                    break;

                if (operand.is_vector_reg())
                {
                    inst = fbinary(NGP_FDUP_V_V, dest, get_register(operand), src_index.u & 0x3);
                }
            }
            else if (dest_tk->is_vector_reg() && operand.is_gp_reg())
            {
                inst = fbinary(NGP_FINS_V_W, dest, dest_index.u & 0x3, get_register(operand));
            }
            else if (dest_tk->is_single_reg() && operand.is_gp_reg())
            {
                inst = fbinary(NGP_FMOV_S_W, dest, get_register(operand), 0);
            }
            else if (dest_tk->is_gp_reg() && operand.is_single_reg())
            {
                inst = fbinary(NGP_FMOV_W_S, dest, get_register(operand), 0);
            }
            else if (dest_tk->is_single_reg())
            {
                inst = fbinary(NGP_FMOV_S_S, dest, get_register(operand), 0);
            }
            else if (dest_tk->is_vector_reg())
            {
                inst = fbinary(NGP_FMOV_V_V, dest, get_register(operand), 0);
            }
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
    case TI_FSMOV:
    {
        u16 dest;
        if(!try_get_register(dest, RegisterGP, "expected destination register"))
            break;
        if(!expected_comma())
            break;

        GET_VINDEX_REG(src, src_tk, src_index, "expected source vector register", break);
        if (src_tk->is_vector_reg())
        {
            inst = fbinary(NGP_FSMOV_W_V, dest, src, src_index.u & 0x3);
        }
        else
        {
            MAKE_ERROR((*src_tk), break, "register width mismatch");
        }
    }
        break;
    case TI_FUMOV:
    {
        u16 dest;
        if (!try_get_register(dest, RegisterGP, "expected destination register"))
            break;
        if(!expected_comma())
            break;

        GET_VINDEX_REG(src, src_tk, src_index, "expected source vector register", break);
        if (src_tk->is_vector_reg())
        {
            inst = fbinary(NGP_FUMOV_W_V, dest, src, src_index.u & 0x3);
        }
        else
        {
            MAKE_ERROR((*src_tk), break, "register width mismatch");
        }
    }
        break;
    case TI_SCVTF:
    {
        u16 dest;
        if (!try_get_register(dest, RegisterFP, "expected destination register"))
            break;

        AsmToken* dest_tk = last;
        if (!expected_comma())
            break;

        u16 src;
        if (!try_get_register(src, RegisterGP, "expected source register"))
            break;
        AsmToken* src_tk = last;

        if (dest_tk->is_single_reg())
        {
            inst = fbinary(NGP_SCVTF_S_W, dest, src, 0);
        }
    }
        break;
    case TI_UCVTF:
    {
        u16 dest;
        if (!try_get_register(dest, RegisterFP, "expected destination register"))
            break;
        AsmToken* dest_tk = last;
        if (!expected_comma())
            break;

        u16 src;
        if (!try_get_register(src, RegisterGP, "expected source register"))
            break;
        AsmToken* src_tk = last;

        if (dest_tk->is_single_reg())
        {
            inst = fbinary(NGP_UCVTF_S_W, dest, src, 0);
        }
    }
        break;
    case TI_FADD:
        assemble_fbinary(inst, NGP_FADD_S, NGP_FADD_V);
        break;
    case TI_FSUB:
        assemble_fbinary(inst, NGP_FSUB_S, NGP_FSUB_V);
        break;
    case TI_FMUL:
        assemble_fbinary(inst, NGP_FMUL_S, NGP_FMUL_V);
        break;
    case TI_FDIV:
        assemble_fbinary(inst, NGP_FDIV_S, NGP_FDIV_V);
        break;
    case TI_FNEG:
    {
        u16 dest;
        if (!try_get_register(dest, RegisterFPOrVector, "expected destination register"))
            break;

        AsmToken* dest_tk = last;
        if (!expected_comma())
            break;

        u16 src;
        if (!try_get_register(src, RegisterFPOrVector, "expected source register"))
            break;
        AsmToken* src_tk = last;

        if (dest_tk->get_fp_type() != src_tk->get_fp_type())
        {
            MAKE_ERROR((*dest_tk), break, "register width mismatch");
        }

        if (dest_tk->get_fp_type() == FPSingle)
        {
            inst = fbinary(NGP_FNEG_S, dest, src, 0);
        }
        else if (dest_tk->is_vector_reg())
        {
            inst = fbinary(NGP_FNEG_V, dest, src, 0);
        }
    }
        break;
    case TI_FABS:
    {
        u16 dest;
        if (!try_get_register(dest, RegisterFP, "expected destination register"))
            break;
        AsmToken* dest_tk = last;
        if (!expected_comma())
            break;

        u16 src;
        if (!try_get_register(src, RegisterFP, "expected source register"))
            break;
        AsmToken* src_tk = last;

        if (dest_tk->get_fp_type() != src_tk->get_fp_type())
        {
            MAKE_ERROR((*dest_tk), break, "register width mismatch");
        }

        if (dest_tk->get_fp_type() == FPSingle)
        {
            inst = fbinary(NGP_FABS_S, dest, src, 0);
        }
    }
    break;
    case TI_FINS:
    {
        GET_VINDEX_REG(dest, dest_tk, dest_index, "expected destination register", break);
        if(!expected_comma())
            break;

        AsmToken operand = parse_expression(ParsePrecedence::Start);
        if (!operand.is(TOKEN_REGISTER))
        {
            MAKE_ERROR(operand, break, "expected source register");
        }

        if(operand.is_vector_reg() && !dest_tk->is_vector_reg())
        {
            MAKE_ERROR((*dest_tk), break, "register subfix mismatch");
        }

        if (operand.is_gp_reg())
        {
            inst = fbinary(NGP_FINS_V_W, dest, dest_index.u & 0x3, get_register(operand));
        }
        else
        {
            if(!expected_left_key())
                break;
            AsmToken src_index = parse_expression(ParsePrecedence::Start);
            if (!src_index.is(TOKEN_IMMEDIATE))
            {
                MAKE_ERROR(src_index, break, "expected a constant index");
            }
            if (!expected_right_key())
                break;

            if (dest_tk->is_vector_reg())
            {
                inst = fp_4op(NGP_FINS_V, dest, dest_index.u & 0x3, src_index.u, operand.u & 0x3);
            }
        }
    }
        break;
    case TI_FDUP:
    {
        u16 dest;
        if(!try_get_register(dest, RegisterFP, "expected destination register"))
            break;
        AsmToken* dest_tk = last;
        if(!expected_comma())
            break;

        GET_VINDEX_REG(src, src_tk, src_index, "expected source register", break);

        if (dest_tk->get_fp_type() == FPSingle && !src_tk->is_vector_reg())
        {
            MAKE_ERROR((*dest_tk), break, "register type mismatch")
        }

        if (dest_tk->get_fp_type() == FPSingle)
        {
            inst = fbinary(NGP_FDUP_S_V, dest, src, src_index.u & 0x3);
        }
    }
        break;
    case TI_FMADD:
        assemble_fp_three_operands(inst, [](u8 dest, u8 src1, u8 src2, u8 src3, FPType fp_type)
            {
                if (fp_type == FPSingle)
                {
                    return fp_4op(NGP_FMADD_S, dest, src1, src2, src3);
                }
                return 0U;
            }
        );
        break;
    case TI_FMSUB:
        assemble_fp_three_operands(inst, [](u8 dest, u8 src1, u8 src2, u8 src3, FPType fp_type)
            {
                if (fp_type == FPSingle)
                {
                    return fp_4op(NGP_FMSUB_S, dest, src1, src2, src3);
                }
                return 0U;
            }
        );
        break;
    case TI_LDP:
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
    case TI_MNEG:
        assemble_two_operands(inst, [](u8 dest, u8 src1, u8 src2)
            {
                return extendedalu(NGP_MSUB, dest, src1, src2, ZeroRegister);
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
        u16 dest;
        if(!try_get_register(dest, RegisterGP, "expected destination register"))
            break;
        if(!expected_comma())
            break;

        AsmToken operand = parse_expression(ParsePrecedence::Start);
        HANDLE_NOT_DEFINED_VALUE(ResolveInstruction, break, index, prog_index);

        if (operand.is(TOKEN_REGISTER))
        {
            u16 op_reg;
            if (!try_get_register_tk(operand, op_reg, RegisterGP))
                return;
            inst = inst_3op(NGP_OR, dest, ZeroRegister, op_reg);
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
        u16 dest;
        if (!try_get_register(dest, RegisterGP, "expected destination register"))
            break;
        if (!expected_comma())
            break;

        AsmToken operand = parse_expression(ParsePrecedence::Start);
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
        u16 dest;
        if (!try_get_register(dest, RegisterGP, "expected destination register"))
            break;
        if(!expected_comma())
            break;
        
        AsmToken operand = parse_expression(ParsePrecedence::Start);
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
        inst = non_binary(NGP_RET, 30, 0, 0);
    }
        break;
    case TI_BR:
    {
        u16 reg;
        if (!try_get_register(reg, RegisterGP, "a target branch register was expected"))
        inst = non_binary(NGP_BLR, reg, 0, 0);
    }
        break;
    case TI_BLR:
    {
        u16 reg;
        if (!try_get_register(reg, RegisterGP, "a target branch register was expected"))
        inst = non_binary(NGP_BLR, reg, 0, 0);
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
    case TI_ERET:
    {
        inst = non_binary(NGP_ERET, 0, 0, 0);
    }
        break;
    case TI_WFI:
    {
        inst = non_binary(NGP_WFI, 0, 0, 0);
    }
        break;
    case TI_MSR:
    {
        u16 dest;
        if (!try_get_register(dest, RegisterSysReg, "expected a destination system register"))
            break;
        if (!expected_comma())
            break;

        u16 src;
        if (!try_get_register(src, RegisterGP, "expected a source register"))
            break;

        inst = non_binary_msr(src, dest);
    }
        break;
    case TI_MRS:
    {
        u16 dest;
        if (!try_get_register(dest, RegisterGP, "expected a destination register"))
            break;
        if (!expected_comma())
            break;

        u16 src;
        if (!try_get_register(src, RegisterSysReg, "expected a source system register"))
            break;

        inst = non_binary_msr(src, dest);
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

    u16 dest;

    if (!try_get_register(dest, RegisterAny, "expected source/destination register"))
        return;

    FPType fp_type = FPNone;
    if (last->is_fp_reg())
    {
        fp_type = last->get_fp_type();
    }
    if(!expected_comma())
        return;

    if (handle_symbol && !current->is(TOKEN_LEFT_KEY))
    {
        AsmToken symbol = parse_expression(ParsePrecedence::Start);
        if (context.undefined_label == false)
        {
            if (fp_type == FPSingle)
            {
                inst = pcrel(NGP_LD_S_PC, dest, i32((symbol.u - program_index) / 4));
            }
            else if (fp_type == FPVector)
            {
                inst = pcrel(NGP_LD_V_PC, dest, i32((symbol.u - program_index) / 4));
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

    if(!expected_left_key())
        return;
    if (!expected(TOKEN_REGISTER, "expected base register"))
    {
        return;
    }

    u16 base;
    if (!try_get_register_tk(*last, base, RegisterGP))
        return;

    if (current->is(TOKEN_RIGHT_KEY))
    {
        advance();
        if (fp_type != FPNone)
        {
            if (fp_type == FPSingle)
            {
                inst = fmemoryi(imm_opcode == NGP_ST_IMMEDIATE ? NGP_ST_S_IMMEDIATE : NGP_LD_S_IMMEDIATE, dest, base, 0, 0);
            }
            else if (fp_type == FPVector)
            {
                inst = fmemoryi(imm_opcode == NGP_ST_IMMEDIATE ? NGP_ST_V_IMMEDIATE : NGP_LD_V_IMMEDIATE, dest, base, 0, 0);
            }
        }
        else
        {
            inst = memoryi(imm_opcode, dest, base, 0, 0);
        }
    }
    else if (!current->is(TOKEN_NEW_LINE))
    {
        expected(TOKEN_COMMA, "',' was expected");
        
        AsmToken index_reg = parse_expression(ParsePrecedence::Start);
        if (index_reg.is(TOKEN_IMMEDIATE))
        {
            if (index_reg.iword >= 0)
            {
                u32 aligned = align_up(index_reg.uword, alignment);
                if (aligned != index_reg.u)
                {
                    MAKE_ERROR(index_reg, return, "immediate value must be a multiple of %d", alignment);
                }

                if (index_reg.u > 0xFFF)
                {
                    MAKE_ERROR(index_reg, return, "immediate offset too long");
                }
            }
            else if (index_reg.iword < 0)
            {
                u32 aligned = align_down(index_reg.uword, alignment);
                if (aligned != index_reg.uword)
                {
                    MAKE_ERROR(index_reg, return, "immediate value must be a multiple of %d", alignment);
                }

                if (index_reg.iword < -0xFFF)
                {
                    advance();
                    MAKE_ERROR(index_reg, return, "immediate offset too long");
                }
            }

            // check if we need to subtract or add
            bool sub = index_reg.i < 0 ? 1 : 0;
            i16 offset = (i16)std::abs(index_reg.i);
            if (fp_type == FPSingle)
            {
                inst = fmemoryi(imm_opcode == NGP_ST_IMMEDIATE ? NGP_ST_S_IMMEDIATE : NGP_LD_S_IMMEDIATE, dest, base, offset / alignment, sub);
            }
            else if (fp_type == FPVector)
            {
                inst = fmemoryi(imm_opcode == NGP_ST_IMMEDIATE ? NGP_ST_V_IMMEDIATE : NGP_LD_V_IMMEDIATE, dest, base, offset / alignment, sub);
            }
            else
            {
                inst = memoryi(imm_opcode, dest, base, offset / alignment, sub);
            }
            if (!expected_right_key())
                return;
        }
        else if (index_reg.is(TOKEN_REGISTER))
        {
            u16 reg_index;
            if (!try_get_register_tk(index_reg, reg_index, RegisterGP))
                return;

            if (fp_type == FPSingle)
            {
                inst = inst_3op(NGP_LD_S, dest, base, reg_index);
            }
            else if (fp_type == FPVector)
            {
                inst = inst_3op(NGP_LD_V, dest, base, reg_index);
            }
            else
            {
                inst = inst_3op(index_opc, dest, base, reg_index);
            }
            if(!expected_right_key())
                return;
        }
        else if (index_reg.is(TOKEN_NEW_LINE))
        {
            MAKE_ERROR(index_reg, {}, "expected a second source operand");
        }
        else
        {
            MAKE_ERROR(index_reg, {}, "invalid operand");
        }
    }
    else
    {
        MAKE_ERROR((*last), return, "']' was expected");
    }
}

void Assembler::assemble_binary(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit)
{
    u16 dest;
    if (!try_get_register(dest, RegisterGP, "expected destination register"))
        return;
    if(!expected_comma())
        return;

    u16 src1;
    if (!try_get_register(src1, RegisterGP, "expected first source register"))
        return;
    if (!expected_comma())
        return;

    AsmToken second_operand = parse_expression(ParsePrecedence::Start);
    if (second_operand.is(TOKEN_REGISTER))
    {
        u16 src2 = 0;
        if (!try_get_register_tk(second_operand, src2, RegisterGP))
            return;

        inst = inst_3op(opc, dest, src1, src2);
    }
    else if (second_operand.is(TOKEN_IMMEDIATE) && immediate_limit != 0)
    {
        if (second_operand.u > immediate_limit)
        {
            MAKE_ERROR(second_operand, return, "immediate value too long");
        }

        inst = binaryi(opc_imm, dest, src1, second_operand.ushort[0]);
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

void Assembler::assemble_fbinary(u32& inst, u8 s_opc, u8 v_s4_opc)
{
    u16 dest;
    if (!try_get_register(dest, RegisterFPOrVector, "expected destination register"))
        return;

    AsmToken* dest_tk = last;
    if (!expected_comma())
        return;

    u16 src1;
    if (!try_get_register(src1, RegisterFPOrVector, "expected first source register"))
        return;

    AsmToken* src1_tk = last;
    if (!expected_comma())
        return;

    u16 src2;
    if (!try_get_register(src2, RegisterFPOrVector, "expected second source register"))
        return;

    AsmToken* src2_tk = last;
    if (!expected_comma())
        return;
    
    if (dest_tk->get_fp_type() != src1_tk->get_fp_type() 
        || dest_tk->get_fp_type() != src2_tk->get_fp_type())
    {
        MAKE_ERROR((*dest_tk), return, "register type mismatch");
    }

    u8 final_opc = dest_tk->get_fp_type() == FPSingle ? s_opc : v_s4_opc;
    inst = fbinary(final_opc, dest, src1, src2);
}

void Assembler::assemble_comparison(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit)
{
    u16 src1;
    if (!try_get_register(src1, RegisterGP, "expected first source register"))
        return;
    if (!expected_comma())
        return;

    AsmToken second_source = parse_expression(ParsePrecedence::Start);
    if (second_source.is(TOKEN_REGISTER))
    {
        u16 src2;
        if (!try_get_register_tk(second_source, src2, RegisterGP))
            return;
        
        inst = inst_3op(opc, ZeroRegister, src1, src2);
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
    u16 dest;
    if (!try_get_register(dest, RegisterGP, "expected destination register"))
        return;
    if (!expected_comma())
        return;

    u16 src1;
    if (!try_get_register(src1, RegisterGP, "expected first source register"))
        return;
    if (!expected_comma())
        return;

    u16 src2;
    if (!try_get_register(src2, RegisterGP, "expected second source register"))
        return;
    if (!expected_comma())
        return;

    u16 src3;
    if (!try_get_register(src3, RegisterGP, "expected third source register"))
        return;

    inst = fn(dest, src1, src2, src3);
}

void Assembler::assemble_fp_three_operands(u32& inst, u32(*fn)(u8, u8, u8, u8, FPType))
{
    u16 dest;
    if (!try_get_register(dest, RegisterFP, "expected destination register"))
        return;

    AsmToken* dest_tk = last;
    if (!expected_comma())
        return;

    u16 src1;
    if (!try_get_register(src1, RegisterFP, "expected first source register"))
        return;

    AsmToken* src1_tk = last;
    if (!expected_comma())
        return;

    u16 src2;
    if (!try_get_register(src2, RegisterFP, "expected second source register"))
        return;
    
    AsmToken* src2_tk = last;
    if (!expected_comma())
        return;

    u16 src3;
    if (!try_get_register(src3, RegisterFP, "expected second source register"))
        return;
    
    AsmToken* src3_tk = last;

    if (dest_tk->get_fp_type() != src1_tk->get_fp_type()
        || dest_tk->get_fp_type() != src2_tk->get_fp_type()
        || dest_tk->get_fp_type() != src3_tk->get_fp_type())
    {
        MAKE_ERROR((*dest_tk), return, "register type mismatch");
    }

    inst = fn(dest, src1, src2, src3, dest_tk->get_fp_type());

}

void Assembler::assemble_two_operands(u32& inst, u32(*fn)(u8, u8, u8))
{
    u16 dest;
    if (!try_get_register(dest, RegisterGP, "expected destination register"))
        return;
    if (!expected_comma())
        return;

    u16 src1;
    if (!try_get_register(src1, RegisterGP, "expected first source register"))
        return;
    if (!expected_comma())
        return;

    AsmToken second_operand = parse_expression(ParsePrecedence::Start);
    if (second_operand.is(TOKEN_REGISTER))
    {
        u16 src2;
        if (!try_get_register_tk(second_operand, src2, RegisterGP))
            return;
        inst = fn(dest, src1, src2);
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
    u16 dest;
    if (!try_get_register(dest, RegisterGP, "expected destination register"))
        return;
    if (!expected_comma())
        return;

    AsmToken operand = parse_expression(ParsePrecedence::Start);
    if (operand.is(TOKEN_REGISTER))
    {
        u16 src;
        if (!try_get_register_tk(operand, src, RegisterGP))
            return;
        inst = fn(dest, src);
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
    u16 dest;
    if (!try_get_register(dest, RegisterGP, "expected destination register"))
        return;
    if (!expected_comma())
        return;

    u16 src1;
    if (!try_get_register(src1, RegisterGP, "expected first source register"))
        return;
    if (!expected_comma())
        return;

    AsmToken second_operand = parse_expression(ParsePrecedence::Start);
    if (second_operand.is(TOKEN_REGISTER))
    {
        u16 src2;
        if (!try_get_register_tk(second_operand, src2, RegisterGP))
            return;
        inst = inst_3op(opcode, dest, src1, src2);
    }
    else if (second_operand.is(TOKEN_IMMEDIATE))
    {
        if (second_operand.u > 0x1F)
        {
            MAKE_ERROR(second_operand, return, "shift amount too long");
        }

        const u8 alu_opc = NGP_SHL_IMM + (opcode - NGP_SHL);
        inst = inst_3op(alu_opc, dest, src1, second_operand.byte[0]);
    }
    else if (second_operand.is(TOKEN_NEW_LINE))
    {
        MAKE_ERROR(second_operand, {}, "expected a source operand");
    }
    else
    {
        MAKE_ERROR(second_operand, {}, "invalid operand");
    }
}


