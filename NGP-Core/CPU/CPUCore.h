/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

#define PROFILE 1

static constexpr u32 DEFUALT_CORE_CLOCK_SPEED = MHZ(100);

static constexpr u32 BIOS_REQUESTED_LEVEL = 2;

struct alignas(64) CPUCore
{
    union ProgramStateRegister
    {
        struct
        {
            u32 ZERO : 1;
            u32 CARRY : 1;
            u32 NEGATIVE : 1;
            u32 OVERFLOW : 1;

            u32 HALT : 1;
        };
        u32 raw;
    };

    struct GPRegisters
    {
        u32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10;
        u32 r11, r12, r13, r14, r15, r16, r17, r18, r19, r20;
        u32 r21, r22, r23, r24, r25, r26, r27, r28;
        u32 sp, lr, zr;
        // ZR is here for debugging purposes and alignment
    };

    static constexpr u32 SIMDRegistersCount = 32;

    union SIMDRegister
    {
        QWord qw;
        f32 s;
        Word w;
        f64 d;
        DWord dw;
    };

    static constexpr u32 MaxExceptionLevel = 2;
    static constexpr u32 MaxExceptionLevelCount = 3;

    union
    {
        GPRegisters gpr;
        u32 list[32];
        i32 ilist[32];
    };
    SIMDRegister simd[SIMDRegistersCount];

    // System registers
    u32 current_el;
    ProgramStateRegister psr;

    // Level 0 has no registers
    ProgramStateRegister spsr_el[MaxExceptionLevelCount - 1];
    u32 elr_el[MaxExceptionLevelCount - 1];
    u32 vtr_el[MaxExceptionLevelCount - 1];

    u32 offset;
    u32 pc;
    u32* mem_pc;
    u64 clock_speed;
    u64 cycle_counter;
    u64 cycles_in_second;
    u64 pending_register_read_write1;
#if PROFILE
    u64 inst_counter;
#endif

    void initialize();
    void shutdown();

    void handle_pc_change();

    u32 fetch_inst();

    void dispatch(u64 num_cycles);

    void print_pegisters();
};

