#include "Backend/Assembler.h"
#include "Backend/AssemblerUtility.h"
#include "ErrorManager.h"


#define EXPECTED_COMMA \
    if (!expected(TOKEN_COMMA, "',' was expected")) {\
        current_status = ERROR;\
    }

void Assembler::assemble_instruction()
{
    if (current.is_not(TOKEN_INSTRUCTION)) {
        ErrorManager::error(current.source_file, current.line, current.column, "invalid token");
        current_status = ERROR;
        return;
    }

    advance();

    u32& inst = new_word();
    switch (last.subtype)
    {
    case TI_MOV:
    {
        if (!expected(TOKEN_REGISTER, "a register was expected!")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(last);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_MOV, dest, get_register(current), 0);
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = movi(dest, current.ushort[0]);
            advance();
        }
    }
    break;
    case TI_ADD:
    {
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_ADD, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_ADD_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
    }
    break;
    case TI_SUB:
    {
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_SUB, dest, src1, get_register(current));
            advance();
        }
        else if (current.is(TOKEN_IMMEDIATE)) {
            inst = binaryi(NGP_SUB_IMMEDIATE, dest, src1, current.ushort[0]);
            advance();
        }
    }
    break;
    case TI_MUL:
    {
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_MUL, dest, src1, get_register(current));
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column, 
                "invalid operand"
            );
            current_status = ERROR;
            return;
        }
    }
    break;
    case TI_UMUL:
    {
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_UMUL, dest, src1, get_register(current));
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            return;
        }
    }
    break;
    case TI_DIV:
    {
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_DIV, dest, src1, get_register(current));
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            return;
        }
    }
    break;
    case TI_UDIV:
    {
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
            inst = binary(NGP_UDIV, dest, src1, get_register(current));
            advance();
        }
        else {
            ErrorManager::error(
                current.source_file, current.line, current.column,
                "invalid operand"
            );
            current_status = ERROR;
            return;
        }
    }
    break;
    case TI_AND:
    {
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
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
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
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
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
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
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

        if (current.is(TOKEN_REGISTER)) {
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
        if (!expected(TOKEN_REGISTER, "expected destination register")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(current);
        EXPECTED_COMMA;

        if (!expected(TOKEN_REGISTER, "expected first source register")) {
            current_status = ERROR;
            return;
        }

        u8 src1 = get_register(current);
        EXPECTED_COMMA;

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
    case TI_SC:
    {
        if (!expected(TOKEN_IMMEDIATE, "a immediate value was expected!")) {
            current_status = ERROR;
            return;
        }

        inst = sc((u32)last.u);
    }
        break;
    case TI_RET:
    {
        inst = TI_RET;
    }
    break;
    case TI_HALT:
    {
        inst = NGP_HALT;
    }
    break;
    case TI_ADR:
    {
        if (!expected(TOKEN_REGISTER, "a register was expected!")) {
            current_status = ERROR;
            return;
        }

        u8 dest = get_register(last);
        EXPECTED_COMMA;

        if (current.is(TOKEN_SYMBOL)) {
            auto it = labels.find(current.str);
            if (it != labels.map.end()) {
                Label& l = labels.get(it->second);
                inst = adr(dest, i32((l.address - program.size())/4));
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
        }

    }
    break;
    default:
        ErrorManager::error(last.source_file, last.line, last.column, "invalid token");
        current_status = ERROR;
        break;
    }

    if (!expected(TOKEN_NEW_LINE, "a new line was expected")) {
        current_status = ERROR;
    }

    skip_whitespaces();
}
