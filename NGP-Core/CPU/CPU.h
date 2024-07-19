// --------------------
// CPU.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "FileFormat/Rom.h"

struct QWord {
    u64 hi;
    u64 lo;
};

struct CPU {
    static void initialize();

    static void shutdown();

    static void dispatch();

    static void delayForTiming();

    static void printRegisters();

    static constexpr u32 CountOfRegisters = 37;
    static constexpr u32 MaxExceptionLevel = 2;

    struct RegisterList {
        u32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10;
        u32 r11, r12, r13, r14, r15, r16, r17, r18, r19, r20;
        u32 r21, r22, r23, r24, r25, r26, r27, r28;
        u32 sp;
        u32 lr, zr;
        u32 pc, ir;
        u32 halt, psr;
        u32 cycle_counter;
        u32 el_ra[MaxExceptionLevel];
    };

    static inline RegisterList registers = {};

    static constexpr u32 CountOfSIMDRegisters = 32;
    union SIMD{
        QWord qfp[CountOfSIMDRegisters] = {};
        f64 dfp[CountOfSIMDRegisters * 2];
        u64 ncdfp[CountOfSIMDRegisters * 2];
        f32 sfp[CountOfSIMDRegisters * 4];
        u32 ncsfp[CountOfSIMDRegisters * 4];
    };

    static inline SIMD simd = {};

    enum FLAGS {
        ZERO_FLAG = 0,
        CARRY_FLAG = 1,
        NEGATIVE_FLAG = 2,
        OVERFLOW_FLAG = 3,

        INTERRUPT_FLAG = 4,
    };
};
