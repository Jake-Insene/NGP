#include "Backend/Assembler.h"
#include "Backend/AssemblerUtility.h"
#include "ErrorManager.h"


#define EXPECTED_COMMA() \
    if (!expected(TOKEN_COMMA, "',' was expected")) {\
        current_status = ERROR;\
        break;\
    }

#define EXPECTED_KEY_LEFT() \
    if (!expected(TOKEN_LEFT_KEY, "'[' was expected")) {\
        current_status = ERROR;\
        break;\
    }

#define EXPECTED_KEY_RIGHT() \
    if (!expected(TOKEN_RIGHT_KEY, "']' was expected")) {\
        current_status = ERROR;\
        break;\
    }

#define GET_REG(var, message) \
    if (!expected(TOKEN_REGISTER, message)) {\
        current_status = ERROR;\
        break;\
    }\
    u8 var = get_register(last);

#define CHECK_FP(tk) \
    if(tk.is_single() || tk.is_double()) {\
        is_fp = true;\
        if(tk.is_single()) is_single = true;\
    }

#define INVALIDATE_FP(tk) \
    if (tk.is_single() || tk.is_double()) {\
        ErrorManager::error(\
            tk.source_file, tk.line, tk.column,\
            "expected a gp register but a fp register was given"\
        );\
        current_status = ERROR;\
        break;\
    }

#define BCOND(type_inst, ti)\
    if (current.is(TOKEN_SYMBOL)) {\
        auto it = labels.find(current.str);\
        if (it != labels.end()) {\
            inst = bcond(type_inst, i32((it->second.address - program.size()) / 4));\
        }\
        else {\
            auto& resolve = to_resolve.emplace_back();\
            resolve.source_file = last.source_file;\
            resolve.line = last.line;\
            resolve.column = last.column;\
            resolve.address = u32(program.size() - 4);\
            resolve.symbol = current.str;\
            resolve.type = ti;\
        }\
        advance();\
    }\
    else {\
        ErrorManager::error(current.source_file, current.line, current.column, "a label was expected");\
        current_status = ERROR;\
        break;\
    }

void Assembler::assemble_instruction()
{
    advance(); // inst

    u32& inst = new_word();
    bool is_fp = false;
    bool is_single = false;

    switch (last.subtype)
    {
    case TI_MOV:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_MOV, dest, get_register(current), 0);
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = movi(dest, current.ushort[0]);
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_MOVT:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_IMMEDIATE)) {
            inst = movt(dest, current.ushort[0]);
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
        break;
    case TI_ADD:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_ADD, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_ADD_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_SUB:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_SUB, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_SUB_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_MUL:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_MUL, dest, src1, get_register(current));
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_UMUL:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_UMUL, dest, src1, get_register(current));
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_DIV:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_DIV, dest, src1, get_register(current));
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_UDIV:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_UDIV, dest, src1, get_register(current));
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_AND:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_AND, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_AND_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
    }
    break;
    case TI_OR:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_OR, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_OR_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
    }
    break;
    case TI_XOR:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_XOR, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_XOR_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
    }
    break;
    case TI_SHL:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_SHL, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_SHL_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
    }
    break;
    case TI_SHR:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_SHR, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_SHR_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
    }
    break;
    case TI_LD:
    {
        GET_REG(dest, "expected destination register");
        CHECK_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_SYMBOL)) {
            auto it = labels.find(current.str);
            if (it != labels.end()) {
                inst = ldpc(dest, NGP_LD_PC, i32((it->second.address - program.size()) / 4));
            }
            else {
                auto& resolve = to_resolve.emplace_back();
                resolve.source_file = last.source_file;
                resolve.line = last.line;
                resolve.column = last.column;
                resolve.address = u32(program.size() - 4);
                resolve.symbol = current.str;
                resolve.type = TI_LD;
            }

            advance();
        }
        else {
            EXPECTED_KEY_LEFT();
            if(is_fp){
            }
            if (!assemble_load_store(inst, dest, NGP_LD_IMMEDIATE, NGP_LD, 4, is_fp, is_single)) {
                break;
            }
            EXPECTED_KEY_RIGHT();
        }
    }
    break;
    case TI_LDSH:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        EXPECTED_KEY_LEFT();
        if (!assemble_load_store(inst, dest, NGP_LDSH_IMMEDIATE, NGP_LDSH, 2, false, false)) {
            break;
        }
        EXPECTED_KEY_RIGHT();
    }
    break;
    case TI_LDH:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        EXPECTED_KEY_LEFT();
        if (!assemble_load_store(inst, dest, NGP_LDH_IMMEDIATE, NGP_LDH, 2, false, false)) {
            break;
        }
        EXPECTED_KEY_RIGHT();
    }
    break;
    case TI_LDSB:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        EXPECTED_KEY_LEFT();
        if (!assemble_load_store(inst, dest, NGP_LDSB_IMMEDIATE, NGP_LDSB, 1, false, false)) {
            break;
        }
        EXPECTED_KEY_RIGHT();
    }
    break;
    case TI_LDB:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        EXPECTED_KEY_LEFT();
        if (!assemble_load_store(inst, dest, NGP_LDB_IMMEDIATE, NGP_LDB, 1, false, false)) {
            break;
        }
        EXPECTED_KEY_RIGHT();
    }
    break;
    case TI_ST:
    {
        GET_REG(dest, "expected source register");
        CHECK_FP(last);
        EXPECTED_COMMA();

        EXPECTED_KEY_LEFT();
        if (!assemble_load_store(inst, dest, NGP_ST_IMMEDIATE, NGP_ST, 4, is_fp, is_single)) {
            break;
        }
        EXPECTED_KEY_RIGHT();
    }
    break;
    case TI_STH:
    {
        GET_REG(dest, "expected source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        EXPECTED_KEY_LEFT();
        if (!assemble_load_store(inst, dest, NGP_STH_IMMEDIATE, NGP_STH, 2, false, false)) {
            break;
        }
        EXPECTED_KEY_RIGHT();
    }
    break;
    case TI_STB:
    {
        GET_REG(dest, "expected source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        EXPECTED_KEY_LEFT();
        if (!assemble_load_store(inst, dest, NGP_STB_IMMEDIATE, NGP_STB, 1, false, false)) {
            break;
        }
        EXPECTED_KEY_RIGHT();
    }
        break;
    case TI_CMP:
    {
        GET_REG(src1, "expected first source register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            u8 src2 = get_register(current);
            inst = binary(NGP_CMP, src1, src2, 0);
            advance();
        }
        else if(current.is(TOKEN_IMMEDIATE)) {
            inst = cmpi(src1, current.ushort[0]);
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column, 
                "expected a register or an immediate value"
            );
            current_status = ERROR;
            break;
        }

    }
        break;
    case TI_NOT:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_NOT, dest, get_register(current), 0);
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
        break;
    case TI_NEG:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_NEG, dest, get_register(current), 0);
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_ABS:
    {
        GET_REG(dest, "expected destination register");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_REGISTER)) {
            INVALIDATE_FP(current);
            inst = binary(NGP_ABS, dest, get_register(current), 0);
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            break;
        }
    }
    break;
    case TI_BEQ:
    {
        BCOND(NGP_BEQ, TI_BEQ);
    }
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
    case TI_CALL:
        if (current.is(TOKEN_SYMBOL)) {
            auto it = labels.find(current.str);
            if (it != labels.end()) {
                inst = call(i32((it->second.address - program.size()) / 4));
            }
            else {
                auto& resolve = to_resolve.emplace_back();
                resolve.source_file = last.source_file;
                resolve.line = last.line;
                resolve.column = last.column;
                resolve.address = u32(program.size() - 4);
                resolve.symbol = current.str;
                resolve.type = TI_CALL;
            }

            advance();
        }
        else {
            ErrorManager::error(current.source_file, current.line, current.column, "a label was expected");
            current_status = ERROR;
            break;
        }
        break;
    case TI_B:
        if (current.is(TOKEN_SYMBOL)) {
            auto it = labels.find(current.str);
            if (it != labels.end()) {
                inst = b(i32((it->second.address - program.size()) / 4));
            }
            else {
                auto& resolve = to_resolve.emplace_back();
                resolve.source_file = last.source_file;
                resolve.line = last.line;
                resolve.column = last.column;
                resolve.address = u32(program.size() - 4);
                resolve.symbol = current.str;
                resolve.type = TI_B;
            }

            advance();
        }
        else {
            ErrorManager::error(current.source_file, current.line, current.column, "a label was expected");
            current_status = ERROR;
            break;
        }
        break;
    case TI_SC:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected!")) {
            current_status = ERROR;
            break;
        }

        inst = sc((u32)last.u);
    }
        break;
    case TI_RET:
    {
        inst = NGP_RET;
    }
    break;
    case TI_HALT:
    {
        inst = NGP_HALT;
    }
    break;
    case TI_ADR:
    {
        GET_REG(dest, "a register was expected!");
        INVALIDATE_FP(last);
        EXPECTED_COMMA();

        if (current.is(TOKEN_SYMBOL)) {
            auto it = labels.find(current.str);
            if (it != labels.end()) {
                inst = adr(dest, i32((it->second.address - program.size())/4));
            }
            else {
                auto& resolve = to_resolve.emplace_back();
                resolve.source_file = last.source_file;
                resolve.line = last.line;
                resolve.column = last.column;
                resolve.address = u32(program.size() - 4);
                resolve.symbol = current.str;
                resolve.type = TI_ADR;
            }

            advance();
        }
        else {
            ErrorManager::error(current.source_file, current.line, current.column, "a label was expected");
            current_status = ERROR;
            break;
        }

    }
    break;
    default:
        ErrorManager::error(last.source_file, last.line, last.column, "invalid token");
        current_status = ERROR;
        break;
    }

    if (current.is_not(TOKEN_END_OF_FILE) && !expected(TOKEN_NEW_LINE, "a new line was expected")) {
        current_status = ERROR;
    }

    skip_whitespaces();
}

bool Assembler::assemble_load_store(u32& inst, u8 dest, u8 imm_opcode, 
    u16 index_opc, u8 alignment, bool is_fp, bool is_single) 
{
    if (!expected(TOKEN_REGISTER, "expected base register")) {
            current_status = ERROR;
            return false;
    }
    u8 base = get_register(last);
    if(last.is_single() || last.is_double()) {
        ErrorManager::error(
            last.source_file, last.line, last.column,
            "expected a gp register but a fp register was given"
        );
        current_status = ERROR;
        return false;
    }

    if (current.is(TOKEN_COMMA) && current.is_not(TOKEN_RIGHT_KEY)) {
        advance(); // ,

        if (current.is(TOKEN_IMMEDIATE)) {
            u32 aligned = align_up(current.u, alignment);
            if (aligned != current.u) {
                ErrorManager::error(
                    current.source_file, current.line, current.column,
                    "immediate value must be a multiple of %d", alignment
                );
                return false;
            }

            if (current.u > 0xFFF) {
                ErrorManager::error(
                    current.source_file, current.line, current.column,
                    "immediate offset to long"
                );
                return false;
            }
            else {
                // check if we need to subtract or add
                bool sub = current.i < 0 ? 1 : 0;
                if (is_fp) {
                    if (is_single) {
                        inst = memoryi(NGP_LD_S_IMMEDIATE, dest, base, current.ushort[0] / alignment, sub);
                    }
                    else {
                        inst = memoryi(NGP_LD_D_IMMEDIATE, dest, base, current.ushort[0] / alignment, sub);
                    }
                }
                else {
                    inst = memoryi(imm_opcode, dest, base, current.ushort[0]/alignment, sub);
                }
                advance(); // imm
            }
        }
        else if (current.is(TOKEN_REGISTER)) {
            u8 index = get_register(last);
            if (last.is_single() || last.is_double()) {
                ErrorManager::error(
                    last.source_file, last.line, last.column,
                    "expected a gp register but a fp register was given"
                );
                current_status = ERROR;
                return false;
            }

            if(is_fp) {
                if (is_single) {
                    inst = fbinary(NGP_LD_S, dest, base, index);
                }
                else {
                    inst = fbinary(NGP_LD_D, dest, base, index);
                }
            }
            else {
                inst = binary(index_opc, dest, base, index);
            }
            advance(); // index
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            advance();
            return false;
        }
    }
    else if (current.is(TOKEN_RIGHT_KEY)) {
        inst = memoryi(imm_opcode, dest, base, 0, 0);
    }
    else {
        ErrorManager::error(
            current.source_file, current.line, current.column,
            "invalid operand"
        );
        advance();
        return false;
    }

    return true;
}
