#pragma once
#include "Header.h"

// Base
// 0 - 5 | opcode

// Call/Branch
// 6 - 31 | disp26

// SWI
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
    NGP_SWI,
    NGP_RET,
    NGP_HALT,

    NGP_BRANCH_COND,
    NGP_BINARY,
    NGP_FBINARY,
    NGP_MEMORY_IMMEDIATE,
    NGP_FMEMORY_IMMEDIATE,
    NGP_MEMORY_PAIR,

    NGP_LD_PC,
    NGP_LD_S_PC,
    NGP_LD_D_PC,
    NGP_LD_Q_PC,

    NGP_ADR_PC,

    NGP_IMMEDIATE,

    NGP_ADD_IMMEDIATE,
    NGP_ADDS_IMMEDIATE,
    NGP_SUB_IMMEDIATE,
    NGP_SUBS_IMMEDIATE,
    NGP_RSB_IMMEDIATE,

    NGP_AND_IMMEDIATE,
    NGP_ANDS_IMMEDIATE,
    NGP_OR_IMMEDIATE,
    NGP_ORN_IMMEDIATE,
    NGP_EOR_IMMEDIATE,

    NGP_TEST_AND_BRANCH,
    NGP_COMPARE_AND_BRANCH,
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
// 21 - 25 | imm5/src2
// 26 - 31 | opcode

// Memory (register)
// 6 - 10 | dest/src
// 11 - 15 | base
// 16 - 20 | index
// 21 - 25 | 0
// 26 - 31 | xxx
enum NGPBinary {
    // OPC Rd, Rfs, Rss
    NGP_ADC,
    NGP_SBC,

    // OPC Rd, Rfs, Rss, SHL/SHR/ASR #imm5
    NGP_ADD_SHL,
    NGP_ADD_SHR,
    NGP_ADD_ASR,
    NGP_SUB_SHL,
    NGP_SUB_SHR,
    NGP_SUB_ASR,
    // OPC Rd, Rfs, Rss
    NGP_MUL,
    NGP_UMUL,
    NGP_DIV,
    NGP_UDIV,

    // OPC Rd, Rfs, Rss, SHL/SHR/ASR #imm5
    NGP_RSB_SHL,
    NGP_RSB_SHR,
    NGP_RSB_ASR,

    // MVN Rd, Rs, SHL/SHR/ASR/ROR #imm5
    NGP_MVN_SHL,
    NGP_MVN_SHR,
    NGP_MVN_ASR,
    NGP_MVN_ROR,

    // OPC Rd, Rs, SHL/SHR/ASR #imm5
    NGP_AND_SHL,
    NGP_AND_SHR,
    NGP_AND_ASR,
    NGP_OR_SHL,
    NGP_OR_SHR,
    NGP_OR_ASR,
    NGP_ORN_SHL,
    NGP_ORN_SHR,
    NGP_ORN_ASR,
    NGP_EOR_SHL,
    NGP_EOR_SHR,
    NGP_EOR_ASR,

    // imm5 dont affect
    NGP_SHL,
    NGP_SHR,
    NGP_ASR,
    NGP_ROR,

    // OP Rd, Rfs, Rss, SHL/SHR/ASR imm5
    NGP_ADDS_SHL,
    NGP_ADDS_SHR,
    NGP_ADDS_ASR,
    NGP_SUBS_SHL,
    NGP_SUBS_SHR,
    NGP_SUBS_ASR,
    NGP_ANDS_SHL,
    NGP_ANDS_SHR,
    NGP_ANDS_ASR,

    // OP Rd, Rfs, Rss, Rts
    NGP_MADD,
    NGP_MSUB,
    NGP_UMADD,
    NGP_UMSUB,

    // SHL Rd, Rs, #imm5
    NGP_SHL_IMMEDIATE,
    // SHR Rd, Rs, #imm5
    NGP_SHR_IMMEDIATE,
    // ASR Rd, Rs, #imm5
    NGP_ASR_IMMEDIATE,
    // ROR Rd, Rs, #imm5
    NGP_ROR_IMMEDIATE,

    // OP Rd, Rs, SHL/SHR/ASR #imm5
    NGP_CMP_SHL,
    NGP_CMP_SHR,
    NGP_CMP_ASR,
    NGP_CMN_SHL,
    NGP_CMN_SHR,
    NGP_CMN_ASR,
    NGP_TST_SHL,
    NGP_TST_SHR,
    NGP_TST_ASR,

    // LDX/STX Rd/Rs {, Rd2/Rs2}, [Rb, Ri]
    NGP_LD,
    NGP_LDSH,
    NGP_LDH,
    NGP_LDSB,
    NGP_LDB,

    NGP_ST,
    NGP_STH,
    NGP_STB,

    // ABS Rd, Rs
    NGP_ABS,
};

// FBinary (Register)
// 6 - 10 | dest
// 11 - 15 | src1
// 16 - 20 | src2
// 21 - 25 | imm5/src3
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
    NGP_FNEG_D,

    NGP_FABS_S,
    NGP_FABS_D,
};

// Memory (Immediate)
// 6 - 10 | dest/src
// 11 - 15 | base
// 16 - 18 | opcode
// 19 | sub
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
};

// FMemory (Immediate)
// 6 - 10 | dest/src
// 11 - 15 | base
// 16 - 18 | opcode
// 19 | sub
// 20 - 31 | disp12
enum NGPFMemoryImmediate {
    NGP_LD_S_IMMEDIATE,
    NGP_LD_D_IMMEDIATE,
    NGP_LD_Q_IMMEDIATE,

    NGP_ST_S_IMMEDIATE,
    NGP_ST_D_IMMEDIATE,
    NGP_ST_Q_IMMEDIATE,
};

// Memory Pair
// 6 - 10 | dest1/src1
// 11 - 15 | dest2/src2
// 16 - 20 | base
// 21 - 23 | opcode
// 24 | add - sub
// 25 - 31 | imm8
enum NGPMemoryPair {
    // LDP Rd, [Rb, #[-/+]imm8]
    NGP_LDP,
    NGP_LDP_S,
    NGP_LDP_D,
    NGP_LDP_Q,

    NGP_STP,
    NGP_STP_S,
    NGP_STP_D,
    NGP_STP_Q,
};

// Immediate
// 6 - 10 | dest
// 11 - 15 | opcode
// 16 - 31 | imm16
enum NGPImmediate {
    NGP_MOV_IMMEDIATE,
    NGP_MOVT_IMMEDIATE,

    NGP_MVN_IMMEDIATE,
    NGP_MVNT_IMMEDIATE,

    NGP_CMP_IMMEDIATE,
    NGP_CMN_IMMEDIATE,
    NGP_TST_IMMEDIATE,

    NGP_FMOV_S_IMMEDIATE,
    NGP_FMOV_D_IMMEDIATE,
};

// Test and branch
// 6 - 10 | src
// 11 - 15 | imm5
// 16 | opc
// 17 - 31 | disp15
enum NGPTestBranch {
    NGP_TBZ,
    NGP_TBNZ,
};

// Compare and branch
// 6 - 10 | src
// 11 | opc
// 12 - 31 | disp20
enum NGPCompareBranch {
    NGP_CBZ,
    NGP_CBNZ,
};
