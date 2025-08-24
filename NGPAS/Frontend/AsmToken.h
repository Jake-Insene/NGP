/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "StringPool.h"


enum AsmTokenType : u8 {
    TOKEN_ERROR,
    TOKEN_END_OF_FILE,

    TOKEN_IMMEDIATE,
    TOKEN_STRING,

    TOKEN_DIRECTIVE,
    TOKEN_INSTRUCTION,
    TOKEN_LABEL,
    TOKEN_SYMBOL,

    TOKEN_NEW_LINE,

    TOKEN_LEFT_PARENT,
    TOKEN_RIGHT_PARENT,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_EQUAL,
    TOKEN_EQUALEQUAL,
    TOKEN_NOTEQUAL,
    TOKEN_LESS,
    TOKEN_LESSEQUAL,
    TOKEN_GREATER,
    TOKEN_GREATEREQUAL,
    TOKEN_DOLLAR,
    TOKEN_COMMA,
    TOKEN_LEFT_KEY,
    TOKEN_RIGHT_KEY,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_XOR,
    TOKEN_NOT,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_SHL,
    TOKEN_SHR,
    TOKEN_ROR,
    TOKEN_ASR,

    TOKEN_REGISTER,

    TOKEN_COUNT,
};

enum AsmTokenRegister : u8
{
    TOKEN_R0 = 0,
    TOKEN_R1,
    TOKEN_R2,
    TOKEN_R3,
    TOKEN_R4,
    TOKEN_R5,
    TOKEN_R6,
    TOKEN_R7,
    TOKEN_R8,
    TOKEN_R9,
    TOKEN_R10,
    TOKEN_R11,
    TOKEN_R12,
    TOKEN_R13,
    TOKEN_R14,
    TOKEN_R15,
    TOKEN_R16,
    TOKEN_R17,
    TOKEN_R18,
    TOKEN_R19,
    TOKEN_R20,
    TOKEN_R21,
    TOKEN_R22,
    TOKEN_R23,
    TOKEN_R24,
    TOKEN_R25,
    TOKEN_R26,
    TOKEN_R27,
    TOKEN_R28,
    TOKEN_R29,
    TOKEN_R30,
    TOKEN_R31,

    TOKEN_S0,
    TOKEN_S1,
    TOKEN_S2,
    TOKEN_S3,
    TOKEN_S4,
    TOKEN_S5,
    TOKEN_S6,
    TOKEN_S7,
    TOKEN_S8,
    TOKEN_S9,
    TOKEN_S10,
    TOKEN_S11,
    TOKEN_S12,
    TOKEN_S13,
    TOKEN_S14,
    TOKEN_S15,
    TOKEN_S16,
    TOKEN_S17,
    TOKEN_S18,
    TOKEN_S19,
    TOKEN_S20,
    TOKEN_S21,
    TOKEN_S22,
    TOKEN_S23,
    TOKEN_S24,
    TOKEN_S25,
    TOKEN_S26,
    TOKEN_S27,
    TOKEN_S28,
    TOKEN_S29,
    TOKEN_S30,
    TOKEN_S31,

    TOKEN_V0,
    TOKEN_V1,
    TOKEN_V2,
    TOKEN_V3,
    TOKEN_V4,
    TOKEN_V5,
    TOKEN_V6,
    TOKEN_V7,
    TOKEN_V8,
    TOKEN_V9,
    TOKEN_V10,
    TOKEN_V11,
    TOKEN_V12,
    TOKEN_V13,
    TOKEN_V14,
    TOKEN_V15,
    TOKEN_V16,
    TOKEN_V17,
    TOKEN_V18,
    TOKEN_V19,
    TOKEN_V20,
    TOKEN_V21,
    TOKEN_V22,
    TOKEN_V23,
    TOKEN_V24,
    TOKEN_V25,
    TOKEN_V26,
    TOKEN_V27,
    TOKEN_V28,
    TOKEN_V29,
    TOKEN_V30,
    TOKEN_V31,

    TOKEN_PSR,
    TOKEN_CURRENT_EL,
    TOKEN_SPSR_EL1,
    TOKEN_SPSR_EL2,
    TOKEN_SPSR_EL3,
    TOKEN_SPSR_IRQ,

    TOKEN_EDR_EL1,
    TOKEN_EDR_EL2,
    TOKEN_EDR_EL3,

    TOKEN_ELR_EL1,
    TOKEN_ELR_EL2,
    TOKEN_ELR_EL3,

    TOKEN_VBAR_EL1,
    TOKEN_VBAR_EL2,
    TOKEN_VBAR_EL3,
    
    TOKEN_FAR_EL1,
    TOKEN_FAR_EL2,
    TOKEN_FAR_EL3,

    TOKEN_END_SYSTEM_REGS,
};

enum AsmTokenDirective : u8
{
    TD_FORMAT,
    TD_FORMAT_RAW,
    TD_FORMAT_ROM,
    TD_AS,
    TD_ORG,
    TD_INCLUDE,
    TD_INCBIN,
    TD_MACRO,
    TD_STRING,
    TD_BYTE,
    TD_HALF,
    TD_WORD,
    TD_DWORD,
    TD_FLOAT32,
    TD_SINGLE,
    TD_ZERO,
    TD_SPACE,
    TD_ALIGN,
};

enum AsmTokenInstruction : u8
{
    TI_NOP,

    TI_BL,
    TI_B,
    TI_ADR,

    TI_BEQ,
    TI_BEZ = TI_BEQ,
    TI_BNE,
    TI_BNZ = TI_BNE,
    TI_BLT,
    TI_BLE,
    TI_BGT,
    TI_BGE,
    TI_BCS,
    TI_BCC,
    TI_BMI,
    TI_BPL,
    TI_BVS,
    TI_BVC,
    TI_BHI,
    TI_BLS,
    TI_BAL,

    TI_ADD,
    TI_ADDS,
    TI_SUB,
    TI_SUBS,

    TI_ADC,
    TI_ADCS,
    TI_SBC,
    TI_SBCS,

    TI_AND,
    TI_ANDS,
    TI_OR,
    TI_ORN,
    TI_EOR,
    TI_SHL,
    TI_SHR,
    TI_ASR,
    TI_ROR,

    TI_BIC,
    TI_BICS,

    TI_CMP,
    TI_CMN,
    TI_TST,
    TI_NOT,
    TI_NEG,
    TI_ABS,

    TI_FMOV,
    TI_FSMOV,
    TI_FUMOV,
    TI_SCVTF,
    TI_UCVTF,

    TI_FADD,
    TI_FSUB,
    TI_FMUL,
    TI_FDIV,

    TI_FNEG,
    TI_FABS,

    TI_FINS,
    TI_FDUP,

    TI_FMADD,
    TI_FMSUB,

    TI_LDP,
    TI_LD,
    TI_LDH,
    TI_LDSH,
    TI_LDB,
    TI_LDSB,

    TI_STP,
    TI_ST,
    TI_STH,
    TI_STB,

    TI_TBZ,
    TI_TBNZ,

    TI_CBZ,
    TI_CBNZ,

    TI_MADD,
    TI_MSUB,
    TI_MNEG,
    TI_MUL,
    TI_DIV,
    TI_UDIV,

    TI_MOV,
    TI_MOVT,
    TI_MVN,

    TI_RET,
    TI_BR,
    TI_BLR,

    TI_BRK,
    TI_SVC,
    TI_EVC,
    TI_SMC,

    TI_ERET,
    TI_WFI,
    TI_HALT,
    TI_MSR,
    TI_MRS,
};

enum FPType
{
    FPNone = 0,
    FPSingle = 1,
    FPVector = 2,
};

struct AsmToken
{
    AsmTokenType type;
    StringID source_file;
    u32 line;
    u8 subtype;

    StringID str;

    union
    {
        u8 byte[8];
        u16 ishort[4];
        u16 ushort[4];
        i32 iword;
        u32 uword;
        i64 i;
        u64 u;
        f32 s;
        f64 d;
    };

    [[nodiscard]] std::string_view get_source_file() const { return StringPool::get(source_file); }
    [[nodiscard]] std::string_view get_str() const { return StringPool::get(str); }

    [[nodiscard]] constexpr bool is(AsmTokenType tk) const
    {
        return type == tk;
    }

    [[nodiscard]] constexpr bool is_one_of(AsmTokenType tk1, AsmTokenType tk2) const
    {
        return type == tk1 || type == tk2;
    }

    [[nodiscard]] constexpr bool is_gp_reg() const
    {
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_R0 && subtype <= TOKEN_R31);
    }

    [[nodiscard]] constexpr bool is_fp_reg() const
    {
        return is_single_reg() || is_vector_reg();
    }

    [[nodiscard]] constexpr bool is_single_reg() const
    {
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_S0 && subtype <= TOKEN_S31);
    }

    [[nodiscard]] constexpr bool is_vector_reg() const
    {
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_V0 && subtype <= TOKEN_V0);
    }

    [[nodiscard]] constexpr FPType get_fp_type() const
    {
        return is_single_reg() ? FPSingle
            : is_vector_reg() ? FPVector
            : FPNone;
    }

    [[nodiscard]] constexpr bool is_system_reg() const
    {
        return is(TOKEN_REGISTER) && (subtype >= TOKEN_PSR && subtype < TOKEN_END_SYSTEM_REGS);
    }
};

