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
    NGP_FBINARY,
    NGP_MEMORY_IMMEDIATE,

    NGP_LD_PC,
    NGP_LD_S_PC,
    NGP_LD_D_PC,

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
// 6 - 9 | cond
// 10 - 31 | disp22
enum NGPBranchConditional {
    NGP_BEQ,
    NGP_BNE,
    NGP_BLT,
    NGP_BLE,
    NGP_BGT,
    NGP_BGE,
    NGP_BCS,
    NGP_BNC,
    NGP_BSS,
    NGP_BNS,
    NGP_BOS,
    NGP_BNO,
    NGP_BHI,
    NGP_BLS,
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
    // src2 = imm5
    // mov Rd, Rs, #shl #imm5
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

    // src2 = imm5
    // cmp Rd, Rs, #shl #imm5
    NGP_CMP,

    NGP_LD,
    NGP_LDSH,
    NGP_LDH,
    NGP_LDSB,
    NGP_LDB,

    NGP_ST,
    NGP_STH,
    NGP_STB,

    // src2 = imm5
    // not Rd, Rs, #shl #imm5
    NGP_NOT,
    // src2 = imm5
    // neg Rd, Rs, #shl #imm5
    NGP_NEG,
    // src2 = imm5
    // abs Rd, Rs, #shl #imm5
    NGP_ABS,
};

// FBinary (Register)
// 6 - 10 | dest
// 11 - 15 | src1
// 16 - 20 | src2
// 21 - 31 | opcode
enum NGPFBinary {
    NGP_FMOV_S,
    NGP_FMOV_D,
    NGP_FMOV_NC_W_S,
    NGP_FMOV_NC_S_W,
    NGP_FMOV_NC_W_D,
    NGP_FMOV_NC_D_W,

    NGP_FCVTZS_S,
    NGP_FCVTZU_S,
    NGP_FCVTZS_D,
    NGP_FCVTZU_D,

    NGP_SCVTF_S,
    NGP_UCVTF_S,
    NGP_SCVTF_D,
    NGP_UCVTF_D,

    NGP_FADD_S,
    NGP_FSUB_S,
    NGP_FMUL_S,
    NGP_FDIV_S,

    NGP_FADD_D,
    NGP_FSUB_D,
    NGP_FMUL_D,
    NGP_FDIV_D,

    NGP_LD_S,
    NGP_ST_S,

    NGP_LD_D,
    NGP_ST_D,

    NGP_FNEG_S,
    NGP_FABS_S,
    NGP_FNEG_D,
    NGP_FABS_D,
};

// Memory (Immediate)
// 6 - 10 | dest/src
// 11 - 15 | base
// 16 - 18 | opcode
// 19 | add - sub
// 20 - 31 | disp12
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

    NGP_LD_D_IMMEDIATE,
    NGP_ST_D_IMMEDIATE,
};

// Immediate
// 6 - 10 | dest
// 11 - 15 | opcode
// 16 - 31 | imm16
enum NGPImmediate {
    NGP_CMP_IMMEDIATE,
    
    NGP_MOV_IMMEDIATE,
    NGP_MOVT_IMMEDIATE,
    NGP_FMOV_S_IMMEDIATE,
    NGP_FMOV_D_IMMEDIATE,

    NGP_MOVN_IMMEDIATE,
};
