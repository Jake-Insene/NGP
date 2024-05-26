#pragma once
#include "Header.h"

// Base
// 0 - 5 | opcode

// Call/Branch
// 6 - 31 | disp26

// SC/Ret
// 6 - 31 | imm26

// Binary (Immediate)
// 6 - 10 | dest
// 11 - 15 | src1
// 16 - 31 | imm16

// Memory PC
// 6 - 10 | dest/src
// 11 - 31 | rel

enum NGPInstructionClass : u8 {
    NGP_CALL,
    NGP_BRANCH,
    NGP_SC,
    NGP_RET,
    NGP_HALT,

    NGP_BRANCH_COND,
    NGP_BINARY,

    NGP_MEMORY_IMMEDIATE,

    NGP_LD_PC,

    NGP_ADR_PC,

    NGP_IMMEDIATE,

    NGP_ADD_IMMEDIATE,
    NGP_SUB_IMMEDIATE,

    NGP_AND_IMMEDIATE,
    NGP_OR_IMMEDIATE,
    NGP_XOR_IMMEDIATE,
    NGP_SHL_IMMEDIATE,
    NGP_SHR_IMMEDIATE,
};

// Branch (Conditional)
// 6 - 10 | cond
// 11 - 31 | disp22
enum NGPBranchConditional {
    BGP_BEQ,
    BGP_BNE,
    BGP_BLT,
    BGP_BLE,
    BGP_BGT,
    BGP_BGE,
};

// Binary (Register)
// 6 - 10 | dest 
// 11 - 15 | src1
// 16 - 20 | src2
// 21 - 31 | opcode

// Memory (register)
// 6 - 10 | dest/src
// 11 - 15 | base
// 16 - 20 | index
// 21 - 31 | xxx
enum NGPBinary {
    NGP_MOV,

    NGP_ADD,
    NGP_SUB,
    NGP_MUL,
    NGP_UMUL,
    NGP_DIV,
    NGP_UDIV,

    NGP_AND,
    NGP_OR,
    NGP_XOR,
    NGP_SHL,
    NGP_SHR,

    NGP_CMP,

    NGP_FMOV,
    NGP_FMOV_NC,

    NGP_FMOV_S2I,
    NGP_FMOV_I2S,

    NGP_FMOV_S2U,
    NGP_FMOV_U2S,

    NGP_FADD,
    NGP_FSUB,
    NGP_FMUL,
    NGP_FDIV,

    NGP_LD,
    NGP_LDSH,
    NGP_LDH,
    NGP_LDSB,
    NGP_LDB,

    NGP_ST,
    NGP_STH,
    NGP_STB,

    NGP_LD_F,
    NGP_ST_F,

    NGP_NOT,
    NGP_NEG,
};

// Memory (Immediate)
// 6 - 10 | dest/src
// 11 - 15 | base
// 16 - 19 | opcode
// 20 | add - sub
// 21 - 31 | disp12
enum NGPMemoryImmediate {
    NGP_LD_IMMEDIATE,
    NGP_LDSH_IMMEDIATE,
    NGP_LDH_IMMEDIATE,
    NGP_LDSB_IMMEDIATE,
    NGP_LDB_IMMEDIATE,

    NGP_ST_IMMEDIATE,
    NGP_STH_IMMEDIATE,
    NGP_STB_IMMEDIATE,

    NGP_LD_S_IMMEDIATE,
    NGP_ST_S_IMMEDIATE,
};

// Immediate
// 6 - 10 | dest
// 11 - 15 | opcode
// 16 - 31 | imm16
enum NGPImmediate {
    NGP_CMP_IMMEDIATE,
    NGP_CMP_IMMEDIATE_SHL16,

    NGP_MOV_IMMEDIATE,
    NGP_MOVT_IMMEDIATE,
    NGP_FMOV_IMMEDIATE,

    NGP_MOVN_IMMEDIATE,
    NGP_MOVTN_IMMEDIATE,
};
