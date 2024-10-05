/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

// Base
// 0 - 5 | opcode

// Call/Branch
// 6 - 31 | disp26

// Binary (Immediate)
// 6 - 10 | dest
// 11 - 15 | src1
// 16 - 31 | imm16

// Load/Adr PC
// 6 - 10 | dest/src
// 11 - 31 | rel

// Registers X0-X31
// R31 = ZR,
// R30 = LR
// R29 = SP

enum NGPInstructionClass : u8 {
    NGP_BL = 0x0,
    NGP_B = 0x1,
    NGP_B_COND = 0x2,
    
    NGP_LOGICAL_ADD_SUB = 0x03,
    NGP_FP_OP = 0x4,
    
    NGP_LOAD_STORE_IMMEDIATE = 0x5,
    NGP_LOAD_STORE_FP_IMMEDIATE = 0x6,
    NGP_LOAD_STORE_REGISTER = 0x7,
    NGP_LOAD_STORE_PAIR = 0x8,

    NGP_ADDITIONAL = 0x9,
    NGP_NON_BINARY = 0xA,

    NGP_LD_PC = 0xB,
    NGP_LD_S_PC = 0xC,
    NGP_LD_D_PC = 0xD,
    NGP_LD_Q_PC = 0xE,
    NGP_ADR_PC = 0xF,

    NGP_IMMEDIATE = 0x10,

    NGP_ADD_IMMEDIATE = 0x11,
    NGP_ADDS_IMMEDIATE = 0x12,
    NGP_SUB_IMMEDIATE = 0x13,
    NGP_SUBS_IMMEDIATE = 0x14,

    NGP_AND_IMMEDIATE = 0x15,
    NGP_ANDS_IMMEDIATE = 0x16,
    NGP_OR_IMMEDIATE = 0x17,
    NGP_ORN_IMMEDIATE = 0x18,
    NGP_EOR_IMMEDIATE = 0x19,

    NGP_TBZ = 0x1A,
    NGP_TBNZ = 0x1B,

    NGP_CBZ = 0x1C,
    NGP_CBNZ = 0x1D,
};

// Branch (Conditional)
// 6 - 9 | cond
// 10 - 31 | disp22
enum NGPBranchConditional {
    NGP_BEQ = 0x0,
    NGP_BNE = 0x1,
    NGP_BLT = 0x2,
    NGP_BLE = 0x3,
    NGP_BGT = 0x4,
    NGP_BGE = 0x5,
    NGP_BCS = 0x6,
    NGP_BCC = 0x7,
    NGP_BMI = 0x8,
    NGP_BPL = 0x9,
    NGP_BVS = 0xA,
    NGP_BVC = 0xB,
    NGP_BHI = 0xC,
    NGP_BLS = 0xD,
    NGP_BAL = 0xE,
};

// Logical Sub Add
// 6 - 11 | opcode
// 12 - 16 | dest 
// 17 - 21 | src1
// 22 - 26 | src2
// 27 - 31 | imm5/src3
enum NGPLogicalSubAdd {

    // ADD Rd, Rfs, Rss, SHL/SHR/ASR #imm5
    NGP_ADD_SHL = 0x0,
    NGP_ADD_SHR = 0x1,
    NGP_ADD_ASR = 0x2,
    
    // ADC Rd, Rfs, Rss
    NGP_ADC = 0x3,
    
    // SUB Rd, Rfs, Rss, SHL/SHR/ASR #imm5
    NGP_SUB_SHL = 0x4,
    NGP_SUB_SHR = 0x5,
    NGP_SUB_ASR = 0x6,
    
    // SBC Rd, Rfs, Rss
    NGP_SBC = 0x7,

    // OPC Rd, Rs, SHL/SHR/ASR/ROR #imm5
    NGP_AND_SHL = 0x8,
    NGP_AND_SHR = 0x9,
    NGP_AND_ASR = 0xA,
    NGP_AND_ROR = 0xB,
    
    NGP_OR_SHL = 0xC,
    NGP_OR_SHR = 0xD,
    NGP_OR_ASR = 0xE,
    NGP_OR_ROR = 0xF,
    
    NGP_ORN_SHL = 0x10,
    NGP_ORN_SHR = 0x11,
    NGP_ORN_ASR = 0x12,
    NGP_ORN_ROR = 0x13,

    NGP_EOR_SHL = 0x14,
    NGP_EOR_SHR = 0x15,
    NGP_EOR_ASR = 0x16,
    NGP_EOR_ROR = 0x17,

    NGP_ADDS_SHL = 0x18,
    NGP_ADDS_SHR = 0x19,
    NGP_ADDS_ASR = 0x1A,
    NGP_ADDS_ROR = 0x1B,
    
    NGP_SUBS_SHL = 0x1C,
    NGP_SUBS_SHR = 0x1D,
    NGP_SUBS_ASR = 0x1E,
    NGP_SUBS_ROR = 0x1F,

    NGP_ANDS_SHL = 0x20,
    NGP_ANDS_SHR = 0x21,
    NGP_ANDS_ASR = 0x22,
    NGP_ANDS_ROR = 0x23,

    NGP_BIC_SHL = 0x24,
    NGP_BIC_SHR = 0x25,
    NGP_BIC_ASR = 0x26,
    NGP_BIC_ROR = 0x27,

    NGP_BICS_SHL = 0x28,
    NGP_BICS_SHR = 0x29,
    NGP_BICS_ASR = 0x2A,
    NGP_BICS_ROR = 0x2B,

    // OPC, Rd, Rfs, Rss
    NGP_ADCS = 0x2C,
    NGP_SBCS = 0x2D,
};

// Binary (FP)
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
    // OP Rd, [Rb, #[-/+]imm12]
    NGP_LD_IMMEDIATE = 0x0,
    NGP_LDSH_IMMEDIATE = 0x1,
    NGP_LDH_IMMEDIATE = 0x2,
    NGP_LDSB_IMMEDIATE = 0x3,
    NGP_LDB_IMMEDIATE = 0x4,

    NGP_ST_IMMEDIATE = 0x5,
    NGP_STH_IMMEDIATE = 0x6,
    NGP_STB_IMMEDIATE = 0x7,
};

// Memory (FP & Immediate)
// 6 - 8 | opcode
// 9 - 13 | dest/src
// 14 - 18 | base
// 19 | sub
// 20 - 31 | disp12
enum NGPLoadStoreFP {
    // OP Rd, [Rb, #[-/+]imm12]
    NGP_LD_S_IMMEDIATE = 0x0,
    NGP_LD_D_IMMEDIATE = 0x1,
    NGP_LD_Q_IMMEDIATE = 0x2,

    NGP_ST_S_IMMEDIATE = 0x3,
    NGP_ST_D_IMMEDIATE = 0x4,
    NGP_ST_Q_IMMEDIATE = 0x5,
};

// Load Store (Register)
// 6 - 16 | opcode
// 17 - 21 | dest/src
// 22 - 26 | base
// 27 - 31 | index
enum NGPLoadStoreRegister {
    // OP Rd/Rs, [Rb, Ri]
    NGP_LD = 0x0,
    NGP_LDSH = 0x1,
    NGP_LDH = 0x2,
    NGP_LDSB = 0x3,
    NGP_LDB = 0x4,

    NGP_ST = 0x5,
    NGP_STH = 0x6,
    NGP_STB = 0x7,

    NGP_LD_S = 0x8,
    NGP_LD_D = 0x9,
    NGP_LD_Q = 0xA,

    NGP_ST_S = 0xB,
    NGP_ST_D = 0xC,
    NGP_ST_Q = 0xD,
};

// Memory Pair
// 6 - 8 | opcode
// 9 - 13 | dest1/src1
// 14 - 18 | dest2/src2
// 19 - 23 | base
// 24 | sub
// 25 - 31 | imm8
enum NGPLoadStorePair {
    // LDP/STP Rfs, Rss, [Rb, #[-/+]imm8]
    NGP_LDP = 0x0,
    NGP_LDP_S = 0x1,
    NGP_LDP_D = 0x2,
    NGP_LDP_Q = 0x3,

    NGP_STP = 0x4,
    NGP_STP_S = 0x5,
    NGP_STP_D = 0x6,
    NGP_STP_Q = 0x7,
};

// Additional Opcode
// 6 - 11 | opcode
// 12 - 16 | dest 
// 17 - 21 | src1
// 22 - 26 | src2
// 27 - 31 | imm5/src2
enum NGPAdditional {
    // MADD/MSUB Rd, Rfs, Rss, Rts
    NGP_MADD = 0x0,
    NGP_MSUB = 0x1,

    // UDIV/DIV Rd, Rfs, Rss
    NGP_UDIV = 0x2,
    NGP_DIV = 0x3,

    // OP Rd, Rfs, Rss
    // imm5 dont affect
    NGP_SHL = 0x4,
    NGP_SHR = 0x5,
    NGP_ASR = 0x6,
    NGP_ROR = 0x7,

    // ABS Rd, Rs
    NGP_ABS = 0x8,
};

// Non Binary Opcode
// 6 - 15 | opcode
// 16 - 21 | op
// 22 - 26 | src1
// 27 - 31 | src2
enum NGPNonBinary {
    NGP_RET = 0x0,

    NGP_BLR = 0x1,
    NGP_BR = 0x2,

    NGP_ERET = 0x3,

    NGP_BRK = 0x4,
    NGP_HLT = 0x5,
    NGP_SIT = 0x6,

    NGP_MSR = 0x7,
    NGP_MRS = 0x8,

    NGP_NOP = 0x9,
};

// Immediate
// 6 - 10 | opcode
// 11 - 15 | dest
// 16 - 31 | imm16
enum NGPImmediate {
    // MOVT Rd, #imm16
    NGP_MOVT_IMMEDIATE = 0x0,

    // MVN Rd, #imm16
    NGP_MVN_IMMEDIATE = 0x1,

    // FMOV Rd, #imm16
    NGP_FMOV_S_IMMEDIATE = 0x2,
    NGP_FMOV_D_IMMEDIATE = 0x3,
};
