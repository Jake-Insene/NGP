/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"


struct CPU {
    static void initialize();

    static void shutdown();

    static void dispatch();

    static void delay_for_timing();

    static void print_pegisters();

    static constexpr u32 MaxExceptionLevel = 2;
    static constexpr u32 RegisterCount = 36 + MaxExceptionLevel;

    struct ProgramStateRegister {
        u32 z : 1;
        u32 c : 1;
        u32 n : 1;
        u32 v : 1;

        u32 irq : 1;
        u32 el : 1;
        u32 halt : 1;

        u32 rem : 25;
    };

    struct GPRegisters {
        u32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10;
        u32 r11, r12, r13, r14, r15, r16, r17, r18, r19, r20;
        u32 r21, r22, r23, r24, r25, r26, r27, r28;
        u32 sp, lr, zr;
    };

    struct RegisterList {
        union {
            GPRegisters gpr;
            u32 list[32];
            i32 ilist[32];
        };
        u32 pc, ir;
        ProgramStateRegister psr;
        u32 cycle_counter;
        u32 el_ra[MaxExceptionLevel];
    };
    static_assert(sizeof(RegisterList) / 4 == RegisterCount, "RegisterList dont contain the correct number of registers");

    static inline RegisterList registers = {};
    static inline u32 instruction_counter = 0;

    static constexpr u32 SIMDRegistersCount = 32;
    union SIMD {
        QWord qfp[SIMDRegistersCount] = {};
        f64 dfp[SIMDRegistersCount * 2];
        u64 ncdfp[SIMDRegistersCount * 2];
        f32 sfp[SIMDRegistersCount * 4];
        u32 ncsfp[SIMDRegistersCount * 4];
    };
    static_assert(sizeof(SIMD) / 16 == SIMDRegistersCount, "SIMD dont contain the correct number of registers");

    static inline SIMD simd = {};

    enum FLAGS {
        ZERO_FLAG = 0,
        CARRY_FLAG = 1,
        NEGATIVE_FLAG = 2,
        OVERFLOW_FLAG = 3,

        INTERRUPT_FLAG = 4,
    };
};
