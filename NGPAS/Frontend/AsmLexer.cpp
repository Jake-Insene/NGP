/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Frontend/AsmLexer.h"

#include "ErrorManager.h"
#include <string>


struct SymbolInfo
{
    const char* symbol;
    u8 size;
    TokenType type;
    u8 subtype;
};

SymbolInfo symbols[] =
{
    // directives
    {.symbol = "format", .size = 6, .type = TOKEN_DIRECTIVE, .subtype = TD_FORMAT },
    {.symbol = "raw", .size = 3, .type = TOKEN_DIRECTIVE, .subtype = TD_FORMAT_RAW },
    {.symbol = "rom", .size = 3, .type = TOKEN_DIRECTIVE, .subtype = TD_FORMAT_ROM },
    {.symbol = "as", .size = 2, .type = TOKEN_DIRECTIVE, .subtype = TD_AS },
    {.symbol = "org", .size = 3, .type = TOKEN_DIRECTIVE, .subtype = TD_ORG },
    {.symbol = "include", .size = 7, .type = TOKEN_DIRECTIVE, .subtype = TD_INCLUDE },
    {.symbol = "incbin", .size = 6, .type = TOKEN_DIRECTIVE, .subtype = TD_INCBIN },
    {.symbol = "macro", .size = 5, .type = TOKEN_DIRECTIVE, .subtype = TD_MACRO },
    {.symbol = ".string", .size = 7, .type = TOKEN_DIRECTIVE, .subtype = TD_STRING },
    {.symbol = ".byte", .size = 5, .type = TOKEN_DIRECTIVE, .subtype = TD_BYTE },
    {.symbol = ".half", .size = 5, .type = TOKEN_DIRECTIVE, .subtype = TD_HALF },
    {.symbol = ".word", .size = 5, .type = TOKEN_DIRECTIVE, .subtype = TD_WORD },
    {.symbol = ".dword", .size = 6, .type = TOKEN_DIRECTIVE, .subtype = TD_DWORD },
    {.symbol = ".float32", .size = 8, .type = TOKEN_DIRECTIVE, .subtype = TD_FLOAT32 },
    {.symbol = ".float64", .size = 8, .type = TOKEN_DIRECTIVE, .subtype = TD_FLOAT64 },
    {.symbol = ".single", .size = 7, .type = TOKEN_DIRECTIVE, .subtype = TD_SINGLE },
    {.symbol = ".double", .size = 7, .type = TOKEN_DIRECTIVE, .subtype = TD_DOUBLE },
    {.symbol = ".zero", .size = 5, .type = TOKEN_DIRECTIVE, .subtype = TD_ZERO },
    {.symbol = ".space", .size = 6, .type = TOKEN_DIRECTIVE, .subtype = TD_SPACE },
    {.symbol = ".align", .size = 6, .type = TOKEN_DIRECTIVE, .subtype = TD_ALIGN },

    // general purpose registers
    {.symbol = "r0", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R0},
    {.symbol = "r1", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R1},
    {.symbol = "r2", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R2},
    {.symbol = "r3", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R3},
    {.symbol = "r4", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R4},
    {.symbol = "r5", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R5},
    {.symbol = "r6", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R6},
    {.symbol = "r7", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R7},
    {.symbol = "r8", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R8},
    {.symbol = "r9", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R9},
    {.symbol = "r10", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R10},
    {.symbol = "r11", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R11},
    {.symbol = "r12", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R12},
    {.symbol = "r13", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R13},
    {.symbol = "r14", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R14},
    {.symbol = "r15", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R15},
    {.symbol = "r16", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R16},
    {.symbol = "r17", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R17},
    {.symbol = "r18", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R18},
    {.symbol = "r19", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R19},
    {.symbol = "r20", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R20},
    {.symbol = "r21", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R21},
    {.symbol = "r22", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R22},
    {.symbol = "r23", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R23},
    {.symbol = "r24", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R24},
    {.symbol = "r25", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R25},
    {.symbol = "r26", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R26},
    {.symbol = "r27", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R27},
    {.symbol = "r28", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R28},
    {.symbol = "r29", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R29},
    {.symbol = "r30", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R30},
    {.symbol = "r31", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_R31},

    // single precision
    {.symbol = "s0", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S0},
    {.symbol = "s1", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S1},
    {.symbol = "s2", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S2},
    {.symbol = "s3", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S3},
    {.symbol = "s4", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S4},
    {.symbol = "s5", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S5},
    {.symbol = "s6", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S6},
    {.symbol = "s7", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S7},
    {.symbol = "s8", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S8},
    {.symbol = "s9", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_S9},
    {.symbol = "s10", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S10},
    {.symbol = "s11", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S11},
    {.symbol = "s12", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S12},
    {.symbol = "s13", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S13},
    {.symbol = "s14", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S14},
    {.symbol = "s15", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S15},
    {.symbol = "s16", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S16},
    {.symbol = "s17", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S17},
    {.symbol = "s18", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S18},
    {.symbol = "s19", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S19},
    {.symbol = "s20", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S20},
    {.symbol = "s21", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S21},
    {.symbol = "s22", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S22},
    {.symbol = "s23", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S23},
    {.symbol = "s24", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S24},
    {.symbol = "s25", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S25},
    {.symbol = "s26", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S26},
    {.symbol = "s27", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S27},
    {.symbol = "s28", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S28},
    {.symbol = "s29", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S29},
    {.symbol = "s30", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S30},
    {.symbol = "s31", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_S31},

    // double precision
    {.symbol = "d0", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D0},
    {.symbol = "d1", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D1},
    {.symbol = "d2", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D2},
    {.symbol = "d3", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D3},
    {.symbol = "d4", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D4},
    {.symbol = "d5", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D5},
    {.symbol = "d6", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D6},
    {.symbol = "d7", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D7},
    {.symbol = "d8", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D8},
    {.symbol = "d9", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_D9},
    {.symbol = "d10", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D10},
    {.symbol = "d11", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D11},
    {.symbol = "d12", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D12},
    {.symbol = "d13", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D13},
    {.symbol = "d14", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D14},
    {.symbol = "d15", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D15},
    {.symbol = "d16", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D16},
    {.symbol = "d17", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D17},
    {.symbol = "d18", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D18},
    {.symbol = "d19", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D19},
    {.symbol = "d20", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D20},
    {.symbol = "d21", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D21},
    {.symbol = "d22", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D22},
    {.symbol = "d23", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D23},
    {.symbol = "d24", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D24},
    {.symbol = "d25", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D25},
    {.symbol = "d26", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D26},
    {.symbol = "d27", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D27},
    {.symbol = "d28", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D28},
    {.symbol = "d29", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D29},
    {.symbol = "d30", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D30},
    {.symbol = "d31", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_D31},

    // vector registers
    {.symbol = "v0", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V0 },
    {.symbol = "v1", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V1 },
    {.symbol = "v2", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V2 },
    {.symbol = "v3", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V3 },
    {.symbol = "v4", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V4 },
    {.symbol = "v5", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V5 },
    {.symbol = "v6", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V6 },
    {.symbol = "v7", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V7 },
    {.symbol = "v8", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V8 },
    {.symbol = "v9", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_V9 },
    {.symbol = "v10", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V10 },
    {.symbol = "v11", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V11 },
    {.symbol = "v12", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V12 },
    {.symbol = "v13", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V13 },
    {.symbol = "v14", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V14 },
    {.symbol = "v15", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V15 },
    {.symbol = "v16", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V16 },
    {.symbol = "v17", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V17 },
    {.symbol = "v18", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V18 },
    {.symbol = "v19", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V19 },
    {.symbol = "v20", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V20 },
    {.symbol = "v21", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V21 },
    {.symbol = "v22", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V22 },
    {.symbol = "v23", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V23 },
    {.symbol = "v24", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V24 },
    {.symbol = "v25", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V25 },
    {.symbol = "v26", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V26 },
    {.symbol = "v27", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V27 },
    {.symbol = "v28", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V28 },
    {.symbol = "v29", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V29 },
    {.symbol = "v30", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V30 },
    {.symbol = "v31", .size = 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V31 },

    // fp subfixes per register
#define DEFINE_VSUBFIXEX_S4(VNAME, SIZE, REG) \
    {.symbol = VNAME ##".s4", .size = SIZE + 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V0_S4 + REG}
#define DEFINE_VSUBFIXEX_D2(VNAME, SIZE, REG) \
    {.symbol = VNAME ##".d2", .size = SIZE + 3, .type = TOKEN_REGISTER, .subtype = TOKEN_V0_D2 + REG}

    DEFINE_VSUBFIXEX_S4("v0", 2, 0),
    DEFINE_VSUBFIXEX_S4("v1", 2, 1),
    DEFINE_VSUBFIXEX_S4("v2", 2, 2),
    DEFINE_VSUBFIXEX_S4("v3", 2, 3),
    DEFINE_VSUBFIXEX_S4("v4", 2, 4),
    DEFINE_VSUBFIXEX_S4("v5", 2, 5),
    DEFINE_VSUBFIXEX_S4("v6", 2, 6),
    DEFINE_VSUBFIXEX_S4("v7", 2, 7),
    DEFINE_VSUBFIXEX_S4("v8", 2, 8),
    DEFINE_VSUBFIXEX_S4("v9", 2, 9),
    DEFINE_VSUBFIXEX_S4("v10", 3, 10),
    DEFINE_VSUBFIXEX_S4("v11", 3, 11),
    DEFINE_VSUBFIXEX_S4("v12", 3, 12),
    DEFINE_VSUBFIXEX_S4("v13", 3, 13),
    DEFINE_VSUBFIXEX_S4("v14", 3, 14),
    DEFINE_VSUBFIXEX_S4("v15", 3, 15),
    DEFINE_VSUBFIXEX_S4("v16", 3, 16),
    DEFINE_VSUBFIXEX_S4("v17", 3, 17),
    DEFINE_VSUBFIXEX_S4("v18", 3, 18),
    DEFINE_VSUBFIXEX_S4("v19", 3, 19),
    DEFINE_VSUBFIXEX_S4("v20", 3, 20),
    DEFINE_VSUBFIXEX_S4("v21", 3, 21),
    DEFINE_VSUBFIXEX_S4("v22", 3, 22),
    DEFINE_VSUBFIXEX_S4("v23", 3, 23),
    DEFINE_VSUBFIXEX_S4("v24", 3, 24),
    DEFINE_VSUBFIXEX_S4("v25", 3, 25),
    DEFINE_VSUBFIXEX_S4("v26", 3, 26),
    DEFINE_VSUBFIXEX_S4("v27", 3, 27),
    DEFINE_VSUBFIXEX_S4("v28", 3, 28),
    DEFINE_VSUBFIXEX_S4("v29", 3, 29),
    DEFINE_VSUBFIXEX_S4("v30", 3, 30),
    DEFINE_VSUBFIXEX_S4("v31", 3, 31),

    DEFINE_VSUBFIXEX_D2("v0", 2, 0),
    DEFINE_VSUBFIXEX_D2("v1", 2, 1),
    DEFINE_VSUBFIXEX_D2("v2", 2, 2),
    DEFINE_VSUBFIXEX_D2("v3", 2, 3),
    DEFINE_VSUBFIXEX_D2("v4", 2, 4),
    DEFINE_VSUBFIXEX_D2("v5", 2, 5),
    DEFINE_VSUBFIXEX_D2("v6", 2, 6),
    DEFINE_VSUBFIXEX_D2("v7", 2, 7),
    DEFINE_VSUBFIXEX_D2("v8", 2, 8),
    DEFINE_VSUBFIXEX_D2("v9", 2, 9),
    DEFINE_VSUBFIXEX_D2("v10", 3, 10),
    DEFINE_VSUBFIXEX_D2("v11", 3, 11),
    DEFINE_VSUBFIXEX_D2("v12", 3, 12),
    DEFINE_VSUBFIXEX_D2("v13", 3, 13),
    DEFINE_VSUBFIXEX_D2("v14", 3, 14),
    DEFINE_VSUBFIXEX_D2("v15", 3, 15),
    DEFINE_VSUBFIXEX_D2("v16", 3, 16),
    DEFINE_VSUBFIXEX_D2("v17", 3, 17),
    DEFINE_VSUBFIXEX_D2("v18", 3, 18),
    DEFINE_VSUBFIXEX_D2("v19", 3, 19),
    DEFINE_VSUBFIXEX_D2("v20", 3, 20),
    DEFINE_VSUBFIXEX_D2("v21", 3, 21),
    DEFINE_VSUBFIXEX_D2("v22", 3, 22),
    DEFINE_VSUBFIXEX_D2("v23", 3, 23),
    DEFINE_VSUBFIXEX_D2("v24", 3, 24),
    DEFINE_VSUBFIXEX_D2("v25", 3, 25),
    DEFINE_VSUBFIXEX_D2("v26", 3, 26),
    DEFINE_VSUBFIXEX_D2("v27", 3, 27),
    DEFINE_VSUBFIXEX_D2("v28", 3, 28),
    DEFINE_VSUBFIXEX_D2("v29", 3, 29),
    DEFINE_VSUBFIXEX_D2("v30", 3, 30),
    DEFINE_VSUBFIXEX_D2("v31", 3, 31),

#undef DEFINE_VSUBFIXEX_S4
#undef DEFINE_VSUBFIXEX_D2

    // aliases
    {.symbol = "zr", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R31 },
    {.symbol = "lr", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R30 },
    {.symbol = "sp", .size = 2, .type = TOKEN_REGISTER, .subtype = TOKEN_R29 },


    // instructions
    {.symbol = "nop", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_NOP },
    {.symbol = "bl", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_BL },
    {.symbol = "b", .size = 1, .type = TOKEN_INSTRUCTION, .subtype = TI_B },
    {.symbol = "adr", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ADR },

    {.symbol = "beq", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BEQ },
    {.symbol = "bez", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BEQ },
    {.symbol = "bne", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BNE },
    {.symbol = "bnz", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BNE },
    {.symbol = "blt", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BLT },
    {.symbol = "ble", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BLE },
    {.symbol = "bgt", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BGT },
    {.symbol = "bge", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BGE },
    {.symbol = "bcs", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BCS },
    {.symbol = "bhs", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BCS },
    {.symbol = "bcc", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BCC },
    {.symbol = "blo", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BCC },
    {.symbol = "bmi", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BMI },
    {.symbol = "bpl", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BPL },
    {.symbol = "bvs", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BVS },
    {.symbol = "bvc", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BVC },
    {.symbol = "bhi", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BHI },
    {.symbol = "bls", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BLS },
    {.symbol = "bal", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BAL },

    {.symbol = "add", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ADD },
    {.symbol = "adds", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_ADDS },
    {.symbol = "sub", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SUB },
    {.symbol = "subs", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_SUBS },

    {.symbol = "adc", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ADC },
    {.symbol = "adcs", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_ADCS },
    {.symbol = "sbc", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SBC },
    {.symbol = "sbcs", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_SBCS },

    {.symbol = "and", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_AND },
    {.symbol = "ands", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_ANDS },
    {.symbol = "or", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_OR },
    {.symbol = "orn", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ORN },
    {.symbol = "eor", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_EOR },
    {.symbol = "shl", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SHL },
    {.symbol = "shr", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SHR },
    {.symbol = "asr", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ASR },
    {.symbol = "ror", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ROR },
    {.symbol = "bic", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ROR },
    {.symbol = "bics", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ROR },

    {.symbol = "cmp", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_CMP },
    {.symbol = "cmn", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_CMN },
    {.symbol = "tst", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_TST },
    {.symbol = "not", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_NOT },
    {.symbol = "neg", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_NEG },
    {.symbol = "abs", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_ABS },

    {.symbol = "fmov", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FMOV },
    {.symbol = "fsmov", .size = 5, .type = TOKEN_INSTRUCTION, .subtype = TI_FSMOV },
    {.symbol = "fumov", .size = 5, .type = TOKEN_INSTRUCTION, .subtype = TI_FUMOV },
    {.symbol = "fcvt", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FCVT },
    {.symbol = "scvtf", .size = 5, .type = TOKEN_INSTRUCTION, .subtype = TI_SCVTF },
    {.symbol = "ucvtf", .size = 5, .type = TOKEN_INSTRUCTION, .subtype = TI_UCVTF },

    {.symbol = "fadd", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FADD },
    {.symbol = "fsub", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FSUB },
    {.symbol = "fmul", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FMUL },
    {.symbol = "fdiv", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FDIV },
    {.symbol = "fneg", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FNEG },
    {.symbol = "fabs", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FABS },
    {.symbol = "fins", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FINS },
    {.symbol = "fdup", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_FDUP },

    {.symbol = "ldp", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_LDP },
    {.symbol = "ld", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_LD },
    {.symbol = "ldsh", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_LDSH },
    {.symbol = "ldh", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_LDH },
    {.symbol = "ldsb", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_LDSB },
    {.symbol = "ldb", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_LDB },

    {.symbol = "stp", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_STP },
    {.symbol = "st", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_ST },
    {.symbol = "sth", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_STH },
    {.symbol = "stb", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_STB },

    {.symbol = "tbz", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_TBZ },
    {.symbol = "tbnz", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_TBNZ },
    {.symbol = "cbz", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_CBZ },
    {.symbol = "cbnz", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_CBNZ },

    {.symbol = "mul", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_MUL },
    {.symbol = "div", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_DIV },
    {.symbol = "udiv", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_UDIV },

    {.symbol = "mov", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_MOV },
    {.symbol = "movt", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_MOVT },
    {.symbol = "mvn", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_MVN },

    {.symbol = "ret", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_RET },
    {.symbol = "blr", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BLR },
    {.symbol = "br", .size = 2, .type = TOKEN_INSTRUCTION, .subtype = TI_BR },
    {.symbol = "eret", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_ERET },
    {.symbol = "brk", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_BRK },
    {.symbol = "svc", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SVC },
    {.symbol = "evc", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_EVC },
    {.symbol = "smc", .size = 3, .type = TOKEN_INSTRUCTION, .subtype = TI_SMC },
    {.symbol = "halt", .size = 4, .type = TOKEN_INSTRUCTION, .subtype = TI_HALT },

    // fp subfixes
};

#define MAKE_TOKEN(TYPE) { \
    tk.source_file = file_path;\
    tk.line = line;\
    tk.type = TYPE;\
    advance();\
}

#define MAKE_TOKEN_NO_ADVANCE(TYPE) { \
    tk.source_file = file_path;\
    tk.line = line;\
    tk.type = TYPE;\
}

#define MAKE_TOKEN_TWO(TYPE) { \
    tk.source_file = file_path;\
    tk.line = line;\
    tk.type = TYPE;\
    advance();\
    advance();\
}

void AsmLexer::set(const std::string& source_file, u8* ctn, u32 s)
{
    content = ctn;
    size = s;
    file_path = source_file;
    index = 0;
    line = 1;
    current = content[index];
}

Token AsmLexer::get_next()
{
    Token tk = {};
    tk.type = TOKEN_ERROR;
    tk.source_file = file_path;
    tk.line = line;

    skip_white_space();

    switch (current)
    {
    case ';':
    {
        u32 last_line = line;
        while (last_line == line && current != '\0')
        {
            advance();
        }

        // Fix some bugs
        MAKE_TOKEN_NO_ADVANCE(TOKEN_NEW_LINE);
    }
    break;
    case '#':
        tk = get_immediate();
        break;
    case '=':
        if (peek(1) == '=')
        {
            MAKE_TOKEN_TWO(TOKEN_EQUALEQUAL);
        }
        else
        {
            MAKE_TOKEN(TOKEN_EQUAL);
        }

        break;
    case '!':
        if (peek(1) == '=')
        {
            MAKE_TOKEN_TWO(TOKEN_NOTEQUAL);
        }
        else
        {
            MAKE_TOKEN(TOKEN_NOT);
        }
        break;
    case '<':
        if (peek(1) == '=')
        {
            MAKE_TOKEN_TWO(TOKEN_LESSEQUAL);
        }
        else if (peek(1) == '<')
        {
            MAKE_TOKEN_TWO(TOKEN_SHL);
        }
        else
        {
            MAKE_TOKEN(TOKEN_LESS);
        }
        break;
    case '>':
        if (peek(1) == '=')
        {
            MAKE_TOKEN_TWO(TOKEN_GREATEREQUAL);
        }
        else if (peek(1) == '>')
        {
            MAKE_TOKEN_TWO(TOKEN_SHR);
        }
        else
        {
            MAKE_TOKEN(TOKEN_GREATER);
        }
        break;
    case '$':
        MAKE_TOKEN(TOKEN_DOLLAR);
        break;
    case ',':
        MAKE_TOKEN(TOKEN_COMMA);
        break;
    case '(':
        MAKE_TOKEN(TOKEN_LEFT_PARENT);
        break;
    case ')':
        MAKE_TOKEN(TOKEN_RIGHT_PARENT);
        break;
    case '{':
        MAKE_TOKEN(TOKEN_LEFT_BRACE);
        break;
    case '}':
        MAKE_TOKEN(TOKEN_RIGHT_BRACE);
        break;
    case '[':
        MAKE_TOKEN(TOKEN_LEFT_KEY);
        break;
    case ']':
        MAKE_TOKEN(TOKEN_RIGHT_KEY);
        break;
    case '+':
        MAKE_TOKEN(TOKEN_PLUS);
        break;
    case '-':
        MAKE_TOKEN(TOKEN_MINUS);
        break;
    case '*':
        MAKE_TOKEN(TOKEN_STAR);
        break;
    case '/':
        MAKE_TOKEN(TOKEN_SLASH);
        break;
    case '^':
        MAKE_TOKEN(TOKEN_XOR);
        break;
    case '~':
        MAKE_TOKEN(TOKEN_NOT);
        break;
    case '&':
        MAKE_TOKEN(TOKEN_AND);
        break;
    case '|':
        MAKE_TOKEN(TOKEN_OR);
        break;
    case '"':
        tk = get_string();
        break;
    case '\'':
        tk = get_string();
        break;
    case '\n':
        MAKE_TOKEN(TOKEN_NEW_LINE);
        break;
    case '\0':
        MAKE_TOKEN(TOKEN_END_OF_FILE);
        break;
    default:
        if (is_alpha(current) || current == '_' || current == '.')
        {
            tk = get_symbol_or_label();
        }
        else if (is_num(current))
        {
            tk = get_immediate();
        }
        else
        {
            ErrorManager::error(file_path.c_str(), line, "invalid token '%c'", current);
            advance();
        }
        break;
    }

    return tk;
}

void AsmLexer::skip_white_space()
{
    while (current == ' '
        || current == '\r'
        || current == '\t')
    {
        advance();
    }
}

char AsmLexer::peek(u8 offset)
{
    return (index + offset < size) ? content[index + offset] : '\0';
}

void AsmLexer::advance()
{
    if (index + 1 < size)
    {
        if (current == '\n')
        {
            line++;
        }
        current = content[++index];
    }
    else
    {
        index = size;
        current = '\0';
    }
}

Token AsmLexer::get_symbol_or_label()
{
    Token tk = {
        .source_file = file_path,
        .line = line,
    };

    u64 start = index;
    while (is_alnum(current) || current == '_' || current == '.')
    {
        advance();
    }
    tk.str = std::string_view((char*)content + start, index - start);

    if (current == ':')
    {
        advance();
        tk.type = TOKEN_LABEL;
        return tk;
    }
    else
    {
        tk.type = TOKEN_SYMBOL;
    }

    for (auto& sym : symbols)
    {
        if (sym.size == tk.str.size())
        {
            for (u8 i = 0; i < sym.size; i++)
            {
                if (sym.symbol[i] != std::tolower(tk.str[i]))
                {
                    goto next;
                }
            }

            tk.type = sym.type;
            tk.subtype = sym.subtype;
        next:
            {}
        }
    }

    return tk;
}

Token AsmLexer::get_immediate()
{
    Token tk = {
        .source_file = file_path,
        .line = line,
        .type = TOKEN_IMMEDIATE,
    };

    if (current == '#')
    {
        advance();
    }

    int base = 10;
    if (current == '0')
    {
        if (peek(1) == 'X' || peek(1) == 'x')
        {
            base = 16;
            advance();
            advance();

            if (!is_hex(current))
            {
                ErrorManager::error(file_path.c_str(), line, "invalid constant", current);
                return tk;
            }
        }
        else if (peek(1) == 'b' || peek(1) == 'B')
        {
            base = 2;
            advance();
            advance();

            if (!is_bin(current))
            {
                ErrorManager::error(file_path.c_str(), line, "invalid constant", current);
                return tk;
            }
        }
    }

    u32 start = index;
    u32 i = 0;
    while (is_hex(current) && i < 32)
    {
        advance();
        i++;
    }

    if (i >= 32)
    {
        ErrorManager::error(file_path.c_str(), line, "constant number too long", current);
    }

    if (current == '.')
    {
        advance(); // Skip '.'

        while (is_num(current))
        {
            advance();
        }

        tk.d = std::strtod((char*)content + start, nullptr);
    }
    else
    {
        tk.u = std::stoull((char*)content + start, nullptr, base);
    }

    return tk;
}

bool AsmLexer::is_alpha(u8 c) const
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z');
}

bool AsmLexer::is_num(u8 c) const
{
    return (c >= '0' && c <= '9');
}

bool AsmLexer::is_alnum(u8 c) const
{
    return is_alpha(c) || is_num(c);
}

bool AsmLexer::is_hex(u8 c) const
{
    return
        is_num(c) ||
        (c >= 'A' && c <= 'F') ||
        (c >= 'a' && c <= 'f');
}

bool AsmLexer::is_bin(u8 c) const
{
    return c == '0' || c == '1';
}

Token AsmLexer::get_string()
{
    Token tk = {
        .source_file = file_path,
        .line = line,
        .type = TOKEN_STRING,
    };

    bool cuot = current == '\'';

    advance(); // " || '
    u32 start = index;
    while ((cuot ? current != '\'' : current != '"') && current != '\0')
    {
        if (current == '\'' || current == '"')
        {
            ErrorManager::error(file_path.c_str(), line, "inconsist string");
            return tk;
        }
        advance();
    }
    tk.str = std::string_view((char*)content + start, index - start);

    if (current == '\0')
    {
        ErrorManager::error(file_path.c_str(), line, "bad string");
    }
    else
    {
        advance(); // " || '
    }

    return tk;
}
