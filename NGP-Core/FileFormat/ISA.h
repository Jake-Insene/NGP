/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

// Base
// [0 - 5] | opcode

// Call/Branch
// [6 - 31] -> Disp26

// Binary (Immediate)
// [6 - 10] -> Dest
// [11 - 15] -> Src1
// [16 - 31] -> Imm16

// Load/Adr PC
// [6 - 10] -> Dest/Src
// [11 - 31] -> Rel21

// Registers X0-X31
// R31 = ZR,
// R30 = LR
// R29 = SP

enum NGPBaseOpcode : u8
{
    NGP_BL = 0x0,
    NGP_B = 0x1,
    NGP_B_COND = 0x2,

    NGP_ALU = 0x03,
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
// [6 - 9] -> Cond
// [10 - 31] -> Disp22/Imm22
enum NGPBranchCond
{
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
// [6 - 11] -> Opcode
// [12 - 16] -> Dest 
// [17 - 21] -> Src1
// [22 - 26] -> Src2
// [27 - 31] -> Imm5/Src3
enum NGPALU
{
    // Rd = R[Dest]
    // Rfs = R[Src1]
    // Rss = R[Src2]
    // Rts = R[Src3]

    // ADD Rd, Rfs, Rss, SHL/SHR/ASR (#imm5)
    NGP_ADD_SHL = 0x0,
    NGP_ADD_SHR = 0x1,
    NGP_ADD_ASR = 0x2,

    // ADC Rd, Rfs, Rss
    NGP_ADC = 0x3,

    // SUB Rd, Rfs, Rss, SHL/SHR/ASR (#imm5)
    NGP_SUB_SHL = 0x4,
    NGP_SUB_SHR = 0x5,
    NGP_SUB_ASR = 0x6,

    // SBC Rd, Rfs, Rss
    NGP_SBC = 0x7,

    // Rd = Rfs <OPC> Rss SHL/SHR/ASR/ROR (#imm5)
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
// [6 - 11] -> Opcode
// [12 - 16] -> Dest
// [17 - 21] -> Src1
// [22 - 26] -> Src2
// [27 - 31] -> Src3
enum NGPFBinary
{
    // Rd = R[Dest]
    // Rfs = R[Src1]
    // Rss = R[Src2]
    // Rts = R[Src3]

    // OPC Rd, Rfs, Rss [, Rts]
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
// [6 - 8] -> Opcode
// [9 - 13] -> Dest/Src
// [14 - 18] -> Base
// [19] -> Sub
// [20 - 31] -> Disp12/Imm12
enum NGPMemoryImmediate
{
    // Rd = R[Dest]
    // Rs = R[Src]
    // Rb = R[base]
    
    // LD[SH/H/SB/B] Rd, [Rb, #[-/+]Imm12]
    NGP_LD_IMMEDIATE = 0x0,
    NGP_LDSH_IMMEDIATE = 0x1,
    NGP_LDH_IMMEDIATE = 0x2,
    NGP_LDSB_IMMEDIATE = 0x3,
    NGP_LDB_IMMEDIATE = 0x4,

    // ST[H/B] Rs, [Rb, #[-/+]Imm12]
    NGP_ST_IMMEDIATE = 0x5,
    NGP_STH_IMMEDIATE = 0x6,
    NGP_STB_IMMEDIATE = 0x7,
};

// Memory (FP & Immediate)
// [6 - 8] -> Opcode
// [9 - 13] -> Dest/Src
// [14 - 18] -> Base
// [19] -> Sub
// [20 - 31] -> Disp12/Imm12
enum NGPMemoryFPImmediate
{
    // Rd = R[Dest]
    // Rs = R[Src]
    // Rb = R[Base]

    // LD[S/D/Q] Rd, [Rb, #[-/+]Imm12]
    NGP_LD_S_IMMEDIATE = 0x0,
    NGP_LD_D_IMMEDIATE = 0x1,
    NGP_LD_Q_IMMEDIATE = 0x2,

    // ST[S/D/Q] Rs, [Rb, #[-/+]Imm12]
    NGP_ST_S_IMMEDIATE = 0x3,
    NGP_ST_D_IMMEDIATE = 0x4,
    NGP_ST_Q_IMMEDIATE = 0x5,
};

// Load Store (Register)
// [6 - 16] -> Opcode
// [17 - 21] -> Dest/Src
// [22 - 26] -> Base
// [27 - 31] -> Index
enum NGPLoadStoreRegister
{
    // Rd = R[Dest]
    // Rfs = R[Src]
    // Rb = R[Base]
    // Ri = R[Index]

    //LD[SH/H/SB/B] Rd, [Rb, Ri]
    NGP_LD = 0x0,
    NGP_LDSH = 0x1,
    NGP_LDH = 0x2,
    NGP_LDSB = 0x3,
    NGP_LDB = 0x4,

    //ST[H/B] Rfs, [Rb, Ri]
    NGP_ST = 0x5,
    NGP_STH = 0x6,
    NGP_STB = 0x7,

    //LD[S/D/Q] Rd, [Rb, Ri]
    NGP_LD_S = 0x8,
    NGP_LD_D = 0x9,
    NGP_LD_Q = 0xA,

    //ST[S/D/Q] Rfs, [Rb, Ri]
    NGP_ST_S = 0xB,
    NGP_ST_D = 0xC,
    NGP_ST_Q = 0xD,
};

// Memory Pair
// [6 - 8] -> Opcode
// [9 - 13] -> Dest1/Src1
// [14 - 18] -> Dest2/Src2
// [19 - 23] -> Base
// [24] -> Sub
// [25 - 31] -> Imm7
enum NGPLoadStorePair
{
    // Read/Write memory twice, continuously
    // Rfs = R[Src1]
    // Rfd = R[Dest1]
    // Rss = R[Src2]
    // Rsd = R[Dest2]
    // Rb = R[base]

    // LDP[S/D/Q] Rfd, Rsd, [Rb, #[-/+]Imm7]
    NGP_LDP = 0x0,
    NGP_LDP_S = 0x1,
    NGP_LDP_D = 0x2,
    NGP_LDP_Q = 0x3,

    // STP[S/D/Q] Rfs, Rss, [Rb, #[-/+]Imm7]
    NGP_STP = 0x4,
    NGP_STP_S = 0x5,
    NGP_STP_D = 0x6,
    NGP_STP_Q = 0x7,
};

// SpecialMath Opcode
// [6 - 11] -> Opcode
// [12 - 16] -> Dest 
// [17 - 21] -> Src1
// [22 - 26] -> Src2
// [27 - 31] -> Imm5/Src2
enum NGPExtendedALU
{
    // Rd = R[Dest]
    // Rfs = R[Src1]
    // Rss = R[Src2]
    // Rts = R[Src3]

    // MADD/MSUB Rd, Rfs, Rss, Rts
    NGP_MADD = 0x0,
    NGP_MSUB = 0x1,

    // UDIV/DIV Rd, Rfs, Rss
    NGP_UDIV = 0x2,
    NGP_DIV = 0x3,

    // OP Rd, Rfs, Rss
    // Imm5 dont not affect
    NGP_SHL = 0x4,
    NGP_SHR = 0x5,
    NGP_ASR = 0x6,
    NGP_ROR = 0x7,

    // ABS Rd, Rfs
    NGP_ABS = 0x8,
};

// Non Binary Opcode
// [6 - 15] -> Opcode
// [16 - 21] -> Op
// [22 - 26] -> Src1
// [27 - 31] -> Src2
enum NGPNonBinary
{
    NGP_RET = 0x00,
    NGP_BR = 0x01,
    NGP_BLR = 0x2,

    // BRK #imm16
    NGP_BRK = 0x03,
    
    NGP_ERET = 0x04,
    NGP_WFI = 0x05,

    NGP_MSR = 0x06,
    NGP_MRS = 0x07,

    NGP_SYSSET = 0x08,
    NGP_SYSCLR = 0x09,
    NGP_HALT = 0x0A,

    NGP_NOP = 0x3FF,
};

// Immediate
// [6 - 10] -> Opcode
// [11 - 15] -> Dest
// [16 - 31] -> Imm16
enum NGPImmediate
{
    // Rd = R[Dest]

    // MOV Rd, #Imm16 << 16
    NGP_MOVT_IMMEDIATE = 0x0,

    // MVN Rd, ~(#Imm16)
    NGP_MVN_IMMEDIATE = 0x1,

    // FMOV Rd, #Imm16
    NGP_FMOV_S_IMMEDIATE = 0x2,
    NGP_FMOV_D_IMMEDIATE = 0x3,
};
