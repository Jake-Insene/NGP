#include "Backend/Assembler.h"
#include "ErrorManager.h"

#define EXPECTED_COMMA(BREAKER) \
    if (!expected(TOKEN_COMMA, "',' was expected")) {\
        BREAKER;\
    }

#define EXPECTED_KEY_LEFT(BREAKER) \
    if (!expected(TOKEN_LEFT_KEY, "'[' was expected")) {\
        BREAKER;\
    }

#define EXPECTED_KEY_RIGHT(BREAKER) \
    if (!expected(TOKEN_RIGHT_KEY, "']' was expected")) {\
        BREAKER;\
    }

#define GET_REG(var, message, BREAKER) \
    if(!expected(TOKEN_REGISTER, message)) {\
        BREAKER;\
    }\
    u8 var = getRegister(*last);

#define INVALIDATE_FP(TOKEN, BREAKER) \
    if (TOKEN.isFPReg()) {\
        MAKE_ERROR(TOKEN, BREAKER, "expected a gp register but a fp register was given");\
    }

#define BCOND(type_inst, ti)\
    if (expected(TOKEN_SYMBOL, "a label was expected")) {\
        bool founded = false;\
        auto it = findLabel(last->str, founded);\
        if (founded) {\
            inst = bcond(type_inst, i32((it->second.address - program.size()) / 4));\
        }\
        else {\
            ErrorManager::error(\
                last->source_file, last->line,\
                "undefined reference to %.*s", last->str.size(), last->str.data()\
            );\
        }\
    }\

#define RELATIVE(tk, type_inst, ti) \
        bool founded = false;\
        auto it = findLabel(tk->str, founded);\
        if (founded) {\
            inst = type_inst(i32((it->second.address - program.size()) / 4));\
        }\
        else {\
            ErrorManager::error(\
                tk->source_file, tk->line,\
                "undefined reference to %.*s", tk->str.size(), tk->str.data()\
            );\
        }\

void Assembler::assembleInstruction() {
    advance(); // inst

    if (align_up(u32(program.size()), 4) != u32(program.size()) && !ErrorManager::is_panic_mode) {
        MAKE_ERROR((*last), {}, "rom isn't aligned");
    }
    u32& inst = newWord();

    switch (last->subtype) {
    case TI_MOV:
    {
        GET_REG(dest, "expected destination register", break);
        INVALIDATE_FP((*last), break);
        EXPECTED_COMMA(break);

        Token operand = parseExpresion(ParsePrecedence::Start);
        if (operand.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(operand, break);
            inst = logicalAddSub(NGP_OR_SHL, dest, 31, getRegister(operand), 0);
        }
        else if (operand.is(TOKEN_IMMEDIATE)) {
            if (operand.u > 0xFFFF) {
                MAKE_ERROR(operand, break, "immediate value too long");
            }
            inst = binaryi(NGP_OR_IMMEDIATE, dest, 31, operand.ushort[0]);
        }
        else if (operand.is(TOKEN_NEW_LINE)) {
            MAKE_ERROR(operand, {}, "expected a source operand");
        }
        else {
            MAKE_ERROR(operand, {}, "invalid operand");
        }
    }
    break;
    case TI_MOVT:
    {
        GET_REG(dest, "expected destination register", break);
        INVALIDATE_FP((*last), break);
        EXPECTED_COMMA(break);

        Token operand = parseExpresion(ParsePrecedence::Start);
        if (operand.is(TOKEN_IMMEDIATE)) {
            if (operand.u > 0xFFFF) {
                MAKE_ERROR(operand, break, "immediate value too long");
            }
            inst = iimmediate(NGP_MOVT_IMMEDIATE, dest, operand.ushort[0]);
        }
        else if (operand.is(TOKEN_NEW_LINE)) {
            MAKE_ERROR(operand, {}, "expected a source operand");
        }
        else {
            MAKE_ERROR(operand, {}, "invalid operand");
        }
    }
    break;
    case TI_MVN:
    {
        GET_REG(dest, "expected destination register", break);
        INVALIDATE_FP((*last), break);
        EXPECTED_COMMA(break);

        Token operand = parseExpresion(ParsePrecedence::Start);
        if (operand.is(TOKEN_IMMEDIATE)) {
            if (operand.u > 0xFFFF) {
                MAKE_ERROR(operand, break, "immediate value too long");
            }
            inst = iimmediate(NGP_MVN_IMMEDIATE, dest, operand.ushort[0]);
        }
        else if (operand.is(TOKEN_NEW_LINE)) {
            MAKE_ERROR(operand, {}, "expected a source operand");
        }
        else {
            MAKE_ERROR(operand, {}, "invalid operand");
        }
    }
    break;
    case TI_ADD:
        assembleBinary(inst, NGP_ADD_SHL, NGP_ADD_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_ADDS:
        assembleBinary(inst, NGP_ADDS_SHL, NGP_ADDS_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_SUB:
        assembleBinary(inst, NGP_SUB_SHL, NGP_SUB_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_SUBS:
        assembleBinary(inst, NGP_SUBS_SHL, NGP_SUBS_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_MUL:
        assembleTwoOperands(inst, [](u8 dest, u8 src1, u8 src2) {
            return additional(NGP_MADD, dest, src1, src2, 31);
            }
        );
        break;
    case TI_UMUL:
        assembleTwoOperands(inst, [](u8 dest, u8 src1, u8 src2) {
            return additional(NGP_UMADD, dest, src1, src2, 31);
            }
        );
        break;
    case TI_DIV:
        assembleTwoOperands(inst, [](u8 dest, u8 src1, u8 src2) {
            return additional(NGP_DIV, dest, src1, src2, 0);
            }
        );
        break;
    case TI_UDIV:
        assembleTwoOperands(inst, [](u8 dest, u8 src1, u8 src2) {
            return additional(NGP_UDIV, dest, src1, src2, 0);
            }
        );
        break;
    case TI_ADC:
        assembleTwoOperands(inst, [](u8 dest, u8 src1, u8 src2) {
            return logicalAddSub(NGP_ADC, dest, src1, src2, 0);
            }
        );
        break;
    case TI_ADCS:
        assembleTwoOperands(inst, [](u8 dest, u8 src1, u8 src2) {
            return logicalAddSub(NGP_ADCS, dest, src1, src2, 0);
            }
        );
        break;
    case TI_SBC:
        assembleTwoOperands(inst, [](u8 dest, u8 src1, u8 src2) {
            return logicalAddSub(NGP_SBC, dest, src1, src2, 0);
            }
        );
        break;
    case TI_SBCS:
        assembleTwoOperands(inst, [](u8 dest, u8 src1, u8 src2) {
            return logicalAddSub(NGP_SBCS, dest, src1, src2, 0);
            }
        );
        break;
    case TI_AND:
        assembleBinary(inst, NGP_AND_SHL, NGP_AND_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_ANDS:
        assembleBinary(inst, NGP_ANDS_SHL, NGP_ANDS_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_OR:
        assembleBinary(inst, NGP_OR_SHL, NGP_OR_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_ORN:
        assembleBinary(inst, NGP_ORN_SHL, NGP_ORN_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_EOR:
        assembleBinary(inst, NGP_EOR_SHL, NGP_EOR_IMMEDIATE, 0xFFFF, false, true);
        break;
    case TI_SHL:
        assembleShift(inst, NGP_SHL);
        break;
    case TI_SHR:
        assembleShift(inst, NGP_SHR);
        break;
    case TI_ASR:
        assembleShift(inst, NGP_ASR);
        break;
    case TI_ROR:
        assembleShift(inst, NGP_ROR);
        break;
    case TI_BIC:
        assembleBinary(inst, NGP_BIC_SHL, 0, u16(-1), 0, true);
        break;
    case TI_BICS:
        assembleBinary(inst, NGP_BICS_SHL, 0, u16(-1), 0, true);
        break;
    case TI_LD:
        assembleLoadStore(inst, NGP_LD_IMMEDIATE, NGP_LD, 4, true);
        break;
    case TI_LDSH:
        assembleLoadStore(inst, NGP_LDSH_IMMEDIATE, NGP_LDSH, 2, false);
        break;
    case TI_LDH:
        assembleLoadStore(inst, NGP_LDH_IMMEDIATE, NGP_LDH, 2, false);
        break;
    case TI_LDSB:
        assembleLoadStore(inst, NGP_LDSB_IMMEDIATE, NGP_LDSB, 1, false);
        break;
    case TI_LDB:
        assembleLoadStore(inst, NGP_LDB_IMMEDIATE, NGP_LDB, 1, false);
        break;
    case TI_ST:
        assembleLoadStore(inst, NGP_ST_IMMEDIATE, NGP_ST, 4, false);
        break;
    case TI_STH:
        assembleLoadStore(inst, NGP_STH_IMMEDIATE, NGP_STH, 2, false);
        break;
    case TI_STB:
        assembleLoadStore(inst, NGP_STB_IMMEDIATE, NGP_STB, 1, false);
        break;
    case TI_CMP:
        assembleComparision(inst, NGP_SUBS_SHL, NGP_SUBS_IMMEDIATE, 0xFFFF);
        break;
    case TI_CMN:
        assembleComparision(inst, NGP_ADDS_SHL, NGP_ADDS_IMMEDIATE, 0xFFFF);
        break;
    case TI_TST:
        assembleComparision(inst, NGP_ANDS_SHL, NGP_ANDS_IMMEDIATE, 0xFFFF);
        break;
    case TI_NOT:
        assembleOneOperand(inst, [](u8 dest, u8 src) {
            return logicalAddSub(NGP_ORN_SHL, dest, 31, src, 0);
            }
        );
        break;
    case TI_NEG:
        assembleOneOperand(inst, [](u8 dest, u8 src) {
            return logicalAddSub(NGP_SUB_SHL, dest, 31, src, 0);
            }
        );
        break;
    case TI_ABS:
        assembleOneOperand(inst, [](u8 dest, u8 src) {
            return additional(NGP_ABS, dest, src, 0, 0);
            }
        );
        break;
    case TI_BEQ:
        BCOND(NGP_BEQ, TI_BEQ);
        break;
    case TI_BNE:
        BCOND(NGP_BEQ, TI_BNE);
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
    case TI_BC:
        BCOND(NGP_BGE, TI_BC);
        break;
    case TI_BNC:
        BCOND(NGP_BGE, TI_BNC);
        break;
    case TI_BN:
        BCOND(NGP_BGE, TI_BN);
        break;
    case TI_BP:
        BCOND(NGP_BGE, TI_BP);
        break;
    case TI_BO:
        BCOND(NGP_BGE, TI_BO);
        break;
    case TI_BNO:
        BCOND(NGP_BGE, TI_BNO);
        break;
    case TI_BHI:
        BCOND(NGP_BGE, TI_BHI);
        break;
    case TI_BLS:
        BCOND(NGP_BGE, TI_BLS);
        break;
    case TI_BL:
        if (expected(TOKEN_SYMBOL, "a label was expected")) {
            RELATIVE(last, bl, TI_BL);
        }
        break;
    case TI_B:
        if (expected(TOKEN_SYMBOL, "a label was expected")) {
            RELATIVE(last, b, TI_B);
        }
        break;
    case TI_SWI:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected!")) {
            break;
        }

        inst = swi((u32)last->uword);
    }
    break;
    case TI_RET:
        inst = nonBinary(NGP_RET, 30, 0, 0, 0);
        break;
    case TI_HLT:
        inst = nonBinary(NGP_HLT, 0, 0, 0, 0);
        break;
    case TI_ADR:
    {
        GET_REG(dest, "a register was expected!", break);
        INVALIDATE_FP((*last), break);
        EXPECTED_COMMA(break);

        Token symbol = parseExpresion(ParsePrecedence::Start);
        if (symbol.is(TOKEN_SYMBOL)) {
            bool founded = false;
            auto it = findLabel(last->str, founded);
            if (founded) {
                inst = pcrel(NGP_ADR_PC, dest, i32((it->second.address - program.size()) / 4));
            }
            else {
                MAKE_ERROR((*last), {}, "undefined reference to %.*s", last->str.size(), last->str.data());
            }
        }
        else {
            MAKE_ERROR((*last), {}, "a label was expected");
        }
    }
    break;
    default:
        MAKE_ERROR((*last), {}, "invalid token");
        break;
    }

    if (ErrorManager::is_panic_mode) {
        while (current->isNot(TOKEN_END_OF_FILE) && current->isNot(TOKEN_NEW_LINE)) {
            advance();
        }
    }

    if (current->isNot(TOKEN_END_OF_FILE)) {
        expected(TOKEN_NEW_LINE, "a new line was expected");
    }
}

void Assembler::assembleLoadStore(u32& inst, u8 imm_opcode,
    u8 index_opc, u8 alignment, bool handle_symbol) {
    GET_REG(dest, "expected source register", return);

    u8 fp_type = 0;
    if (last->isFPReg()) {
        if (last->isSingleReg()) {
            fp_type = 1;
        }
        else if (last->isDoubleReg()) {
            fp_type = 2;
        }
        else if (last->isQwordReg()) {
            fp_type = 3;
        }
    }
    EXPECTED_COMMA(return);

    if (handle_symbol) {
        Token symbol = parseExpresion(ParsePrecedence::Start);
        if (symbol.is(TOKEN_SYMBOL)) {
            bool founded = false;
            auto it = findLabel(last->str, founded);
            if (founded) {
                if (fp_type == 1) {
                    inst = pcrel(NGP_LD_PC, dest, i32((it->second.address - program.size()) / 4));
                }
                else if (fp_type == 2) {
                    inst = pcrel(NGP_LD_PC, dest, i32((it->second.address - program.size()) / 4));
                }
                else if (fp_type == 3) {
                    inst = pcrel(NGP_LD_PC, dest, i32((it->second.address - program.size()) / 4));
                }
                else {
                    inst = pcrel(NGP_LD_PC, dest, i32((it->second.address - program.size()) / 4));
                }
            }
            else {
                ErrorManager::error(
                    last->source_file, last->line,
                    "undefined reference to %.*s", last->str.size(), last->str.data()
                );
            }

            return;
        }
    }

    EXPECTED_KEY_LEFT(return);
    if (!expected(TOKEN_REGISTER, "expected base register")) {
        return;
    }
    u8 base = getRegister(*last);
    INVALIDATE_FP((*last), return);

    if (current->is(TOKEN_RIGHT_KEY)) {
        advance();
        inst = memoryi(imm_opcode, dest, base, 0, 0);
    }
    else if (current->isNot(TOKEN_NEW_LINE)) {
        expected(TOKEN_COMMA, "',' was expected");

        Token indice = parseExpresion(ParsePrecedence::Start);
        if (indice.is(TOKEN_IMMEDIATE)) {
            if (indice.iword >= 0) {
                u32 aligned = align_up(indice.uword, alignment);
                if (aligned != indice.u) {
                    MAKE_ERROR(indice, return, "immediate value must be a multiple of %d", alignment);
                }

                if (indice.u > 0xFFF) {
                    MAKE_ERROR(indice, return, "immediate offset too long");
                }
            }
            else if (indice.iword < 0) {
                u32 aligned = align_down(indice.uword, alignment);
                if (aligned != indice.u) {
                    MAKE_ERROR(indice, return, "immediate value must be a multiple of %d", alignment);
                }

                if (indice.iword < -0xFFF) {
                    advance();
                    MAKE_ERROR(indice, return, "immediate offset too long");
                }
            }

            // check if we need to subtract or add
            bool sub = current->i < 0 ? 1 : 0;
            u16 offset = (u16)abs(current->i);
            if (fp_type == 1) {
                inst = fmemoryi(NGP_LD_S_IMMEDIATE, dest, base, offset / alignment, sub);
            }
            else if (fp_type == 2) {
                inst = fmemoryi(NGP_LD_D_IMMEDIATE, dest, base, offset / alignment, sub);
            }
            else if (fp_type == 3) {
                inst = fmemoryi(NGP_LD_Q_IMMEDIATE, dest, base, offset / alignment, sub);
            }
            else {
                inst = memoryi(imm_opcode, dest, base, offset / alignment, sub);
            }
            advance(); // imm
            EXPECTED_KEY_RIGHT(return);
        }
        else if (indice.is(TOKEN_REGISTER)) {
            u8 index = getRegister(indice);
            INVALIDATE_FP(indice, return);

            if (fp_type == 1) {
                inst = fbinary(NGP_LD_S, dest, base, index, 0);
            }
            else if (fp_type == 2) {
                inst = fbinary(NGP_LD_D, dest, base, index, 0);
            }
            else if (fp_type == 3) {
                inst = fbinary(NGP_LD_Q, dest, base, index, 0);
            }
            else {
                inst = additional(index_opc, dest, base, index, 0);
            }
            EXPECTED_KEY_RIGHT(return);
        }
        else if (indice.is(TOKEN_NEW_LINE)) {
            MAKE_ERROR(indice, {}, "expected a second source operand");
        }
        else {
            MAKE_ERROR(indice, {}, "invalid operand");
        }
    }
    else {
        MAKE_ERROR((*last), return, "']' was expected");
    }
}

void Assembler::assembleBinary(u32& inst, u8 opc,
    u8 opc_imm, u16 immediate_limit, bool is_additional_opc, bool use_amount) {
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);
    GET_REG(src1, "expected first source register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token second_operand = parseExpresion(ParsePrecedence::Start);
    if (second_operand.is(TOKEN_REGISTER)) {
        INVALIDATE_FP(second_operand, return);

        u8 adder = 0;
        u8 amount = 0;
        if (use_amount) {
            checkForAmount(adder, amount);
        }

        if (is_additional_opc) {// SHL/SHR/AST/ROR
            inst = additional(opc, dest, src1, getRegister(second_operand), 0);
        }
        else {
            inst = logicalAddSub(opc, dest, src1, getRegister(second_operand), amount);
        }
    }
    else if (second_operand.is(TOKEN_IMMEDIATE) && immediate_limit != -1) {
        if (second_operand.u > immediate_limit) {
            MAKE_ERROR(second_operand, return, "immediate value too long");
        }

        if (is_additional_opc) { // SHL/SHR/ASR/ROR
            inst = additional(opc, dest, src1, (u8)last->ushort[0], 0);
        }
        else {
            inst = binaryi(opc_imm, dest, src1, last->ushort[0]);
        }
    }
    else if (second_operand.is(TOKEN_NEW_LINE)) {
        MAKE_ERROR(second_operand, {}, "expected a second source operand");
    }
    else {
        MAKE_ERROR(second_operand, {}, "invalid operand");
    }
}

void Assembler::assembleComparision(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit) {
    GET_REG(src1, "expected first source register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token second_source = parseExpresion(ParsePrecedence::Start);
    if (second_source.is(TOKEN_REGISTER)) {
        INVALIDATE_FP(second_source, return);
        u8 src2 = getRegister(second_source);

        u8 adder = 0;
        u8 amount = 0;
        checkForAmount(adder, amount);

        inst = logicalAddSub(opc + adder, 31, src1, src2, amount);
    }
    else if (second_source.is(TOKEN_IMMEDIATE)) {
        if (second_source.u > immediate_limit) {
            MAKE_ERROR(second_source, return, "immediate value too long");
        }
        inst = binaryi(opc_imm, 0, src1, second_source.ushort[0]);
    }
    else if (second_source.is(TOKEN_NEW_LINE)) {
        MAKE_ERROR(second_source, {}, "expected a second source operand");
    }
    else {
        MAKE_ERROR(second_source, {}, "invalid operand");
    }
}

void Assembler::assembleTwoOperands(u32& inst, u32(*fn)(u8, u8, u8)) {
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    GET_REG(src1, "expected first source register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token second_operand = parseExpresion(ParsePrecedence::Start);
    if (second_operand.is(TOKEN_REGISTER)) {
        INVALIDATE_FP(second_operand, return);
        inst = fn(dest, src1, getRegister(second_operand));
    }
    else if (second_operand.is(TOKEN_NEW_LINE)) {
        MAKE_ERROR(second_operand, {}, "expected a second source operand");
    }
    else {
        MAKE_ERROR(second_operand, {}, "invalid operand");
    }
}

void Assembler::assembleOneOperand(u32& inst, u32(*fn)(u8, u8)) {
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token operand = parseExpresion(ParsePrecedence::Start);
    if (operand.is(TOKEN_REGISTER)) {
        INVALIDATE_FP(operand, return);
        inst = fn(dest, getRegister(operand));
    }
    else if (operand.is(TOKEN_NEW_LINE)) {
        MAKE_ERROR(operand, {}, "expected a source operand");
    }
    else {
        MAKE_ERROR(operand, {}, "invalid operand");
    }
}

void Assembler::assembleShift(u32& inst, u8 opcode) {
    GET_REG(dest, "expected destination register", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    GET_REG(src1, "expected first source operand", return);
    INVALIDATE_FP((*last), return);
    EXPECTED_COMMA(return);

    Token third_operand = parseExpresion(ParsePrecedence::Start);
    if (third_operand.is(TOKEN_REGISTER)) {
        INVALIDATE_FP(third_operand, return);
        inst = additional(opcode, dest, src1, getRegister(third_operand), 0);
    }
    else if (third_operand.is(TOKEN_IMMEDIATE)) {
        if (third_operand.u > 0x1F) {
            MAKE_ERROR(third_operand, return, "shift amount too long");
        }

        u8 logopc = opcode - NGP_SHL;
        inst = logicalAddSub(NGP_OR_SHL + logopc, dest, 31, src1, third_operand.byte[0]);
    }
    else if (third_operand.is(TOKEN_NEW_LINE)) {
        MAKE_ERROR(third_operand, {}, "expected a source operand");
    }
    else {
        MAKE_ERROR(third_operand, {}, "invalid operand");
    }
}

void Assembler::checkForAmount(u8& adder, u8& amount) {
    if (!current->is(TOKEN_COMMA)) {
        return;
    }

    advance(); // skip comma

    if (current->is(TOKEN_INSTRUCTION)) {
        advance();
        switch (last->subtype) {
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
                current->source_file, current->line,
                "invalid shift type, try shl/shr/asr/ror"
            );
            return;
        }

        if (!expected(TOKEN_IMMEDIATE, "a amount was expected")) {
            return;
        }

        if (last->u > 31) {
            ErrorManager::error(
                current->source_file, current->line,
                "shift amount too long, must to be #0-31"
            );
            return;
        }

        amount = (u8)last->u;
    }
}

