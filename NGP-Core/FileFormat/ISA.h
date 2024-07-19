// --------------------
// ISA.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "Core/Header.h"

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

// Registers X0-X31
// X31 = ZR,
// X30 = SP

enum NGPInstructionClass : u8 {
    NGP_BL,
    NGP_BRANCH,
    NGP_SWI,

    NGP_BRANCH_COND,
    NGP_LOGICAL_ADD_SUB,
    NGP_FBINARY,
    NGP_MEMORY_IMMEDIATE,
    NGP_FMEMORY_IMMEDIATE,
    NGP_MEMORY_PAIR,
    NGP_ADDITIONAL,
    NGP_NON_BINARY,

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

    NGP_AND_IMMEDIATE,
    NGP_ANDS_IMMEDIATE,
    NGP_OR_IMMEDIATE,
    NGP_ORN_IMMEDIATE,
    NGP_EOR_IMMEDIATE,

    NGP_TBZ,
    NGP_TBNZ,

    NGP_CBZ,
    NGP_CBNZ,
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
    NGP_BC,
    NGP_BNC,
    NGP_BN,
    NGP_BP,
    NGP_BO,
    NGP_BNO,
    NGP_BHI,
    NGP_BLS,
};

// Logical Sub Add
// 6 - 11 | opcode
// 12 - 16 | dest 
// 17 - 21 | src1
// 22 - 26 | src2
// 27 - 31 | imm5/src2
enum NGPLogicalSubAdd {

    // ADD Rd, Rfs, Rss, SHL/SHR/ASR #imm5
    NGP_ADD_SHL,
    NGP_ADD_SHR,
    NGP_ADD_ASR,
    
    // ADC Rd, Rfs, Rss
    NGP_ADC,
    
    // SUB Rd, Rfs, Rss, SHL/SHR/ASR #imm5
    NGP_SUB_SHL,
    NGP_SUB_SHR,
    NGP_SUB_ASR,
    
    // SBC Rd, Rfs, Rss
    NGP_SBC,

    // OPC Rd, Rs, SHL/SHR/ASR/ROR #imm5
    NGP_AND_SHL,
    NGP_AND_SHR,
    NGP_AND_ASR,
    NGP_AND_ROR,
    
    NGP_OR_SHL,
    NGP_OR_SHR,
    NGP_OR_ASR,
    NGP_OR_ROR,
    
    NGP_ORN_SHL,
    NGP_ORN_SHR,
    NGP_ORN_ASR,
    NGP_ORN_ROR,

    NGP_EOR_SHL,
    NGP_EOR_SHR,
    NGP_EOR_ASR,
    NGP_EOR_ROR,

    // OP Rd, Rfs, Rss, SHL/SHR/ASR/ROR imm5
    NGP_ADDS_SHL,
    NGP_ADDS_SHR,
    NGP_ADDS_ASR,
    NGP_ADDS_ROR,
    
    NGP_SUBS_SHL,
    NGP_SUBS_SHR,
    NGP_SUBS_ASR,
    NGP_SUBS_ROR,

    NGP_ANDS_SHL,
    NGP_ANDS_SHR,
    NGP_ANDS_ASR,
    NGP_ANDS_ROR,

    // OP Rd, Rfs, Rss, SHL/SHR/ASR/ROR imm5
    NGP_BIC_SHL,
    NGP_BIC_SHR,
    NGP_BIC_ASR,
    NGP_BIC_ROR,

    NGP_BICS_SHL,
    NGP_BICS_SHR,
    NGP_BICS_ASR,
    NGP_BICS_ROR,

    // OPC, Rd, Rfs, Rss
    NGP_ADCS,
    NGP_SBCS,
};

// Binary (FP & Register)
// 6 - 11 | opcode
// 12 - 16 | dest 
// 17 - 21 | src1
// 22 - 26 | src2
// 27 - 31 | src3
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
    NGP_LD_D,
    NGP_LD_Q,

    NGP_ST_S,
    NGP_ST_D,
    NGP_ST_Q,

    NGP_FNEG_S,
    NGP_FNEG_D,

    NGP_FABS_S,
    NGP_FABS_D,
};

// Memory (Immediate)
// 6 - 8 | opcode
// 9 - 13 | dest/src
// 14 - 18 | base
// 19 | sub
// 20 - 31 | disp12
enum NGPMemoryImmediate {
    // LD{...} Rd, [Rb, #[-/+]imm12]
    NGP_LD_IMMEDIATE,
    NGP_LDSH_IMMEDIATE,
    NGP_LDH_IMMEDIATE,
    NGP_LDSB_IMMEDIATE,
    NGP_LDB_IMMEDIATE,

    // ST{...} Rd, [Rb, #[-/+]imm12]
    NGP_ST_IMMEDIATE,
    NGP_STH_IMMEDIATE,
    NGP_STB_IMMEDIATE,
};

// Memory (FP & Immediate)
// 6 - 8 | opcode
// 9 - 13 | dest/src
// 14 - 18 | base
// 19 | sub
// 20 - 31 | disp12
enum NGPFMemoryImmediate {
    // LD{...} Rd, [Rb, #[-/+]imm12]
    NGP_LD_S_IMMEDIATE,
    NGP_LD_D_IMMEDIATE,
    NGP_LD_Q_IMMEDIATE,

    // ST{...} Rd, [Rb, #[-/+]imm12]
    NGP_ST_S_IMMEDIATE,
    NGP_ST_D_IMMEDIATE,
    NGP_ST_Q_IMMEDIATE,
};

// Memory Pair
// 6 - 8 | opcode
// 9 - 13 | dest1/src1
// 14 - 18 | dest2/src2
// 19 - 23 | base
// 24 | sub
// 25 - 31 | imm8
enum NGPMemoryPair {
    // LDP Rfs, Rss, [Rb, #[-/+]imm8]
    NGP_LDP,
    NGP_LDP_S,
    NGP_LDP_D,
    NGP_LDP_Q,

    // STP Rfs, Rss, [Rb, #[-/+]imm8]
    NGP_STP,
    NGP_STP_S,
    NGP_STP_D,
    NGP_STP_Q,
};

// Additional Opcode
// 6 - 11 | opcode
// 12 - 16 | dest 
// 17 - 21 | src1
// 22 - 26 | src2
// 27 - 31 | imm5/src2
enum NGPAdditional {
    // LD/ST{...} Rd/Rs, [Rb, Ri]
    NGP_LD,
    NGP_LDSH,
    NGP_LDH,
    NGP_LDSB,
    NGP_LDB,

    NGP_ST,
    NGP_STH,
    NGP_STB,

    // OP Rd, Rfs, Rss, Rts
    NGP_MADD,
    NGP_MSUB,
    NGP_UMADD,
    NGP_UMSUB,

    // OP Rd, Rfs, Rss
    NGP_UDIV,
    NGP_DIV,

    // OP Rd, Rfs, Rss
    // imm5 dont affect
    NGP_SHL,
    NGP_SHR,
    NGP_ASR,
    NGP_ROR,

    // ABS Rd, Rs
    NGP_ABS,
};

// Non Binary Opcode
// 6 - 11 | opcode
// 12 - 16 | dest 
// 17 - 21 | src1
// 22 - 26 | src2
// 27 - 31 | imm5/src2
enum NGPNonBinary {
    NGP_RET,
    NGP_HLT,
};

// Immediate
// 6 - 10 | opcode
// 11 - 15 | dest
// 16 - 31 | imm16
enum NGPImmediate {
    // MOVT Rd, #imm16
    NGP_MOVT_IMMEDIATE,

    // MVN Rd, #imm16
    NGP_MVN_IMMEDIATE,

    // FMOV Rd, #imm16
    NGP_FMOV_S_IMMEDIATE,
    NGP_FMOV_D_IMMEDIATE,
};
