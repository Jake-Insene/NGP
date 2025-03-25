/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

static constexpr u32 CYCLES_PER_SECOND = MHZ(200);
static constexpr u32 FRAMES_PER_SECOND = 60;
static constexpr u32 CYCLES_PER_FRAME = CYCLES_PER_SECOND / FRAMES_PER_SECOND;

static constexpr u32 BIOS_REQUESTED_LEVEL = 2;

struct alignas(64) CPUCore {
    union ProgramStateRegister {
        struct {
            u32 z : 1;
            u32 c : 1;
            u32 n : 1;
            u32 v : 1;

            u32 halt : 1;

            u32 rem : 27;
        };
        u32 raw;
    };

    struct GPRegisters {
        u32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10;
        u32 r11, r12, r13, r14, r15, r16, r17, r18, r19, r20;
        u32 r21, r22, r23, r24, r25, r26, r27, r28;
        u32 sp, lr, zr;
    };

    static constexpr u32 SIMDRegistersCount = 32;

    union SIMDRegister {
        QWord qw;
        f32 s;
        Word w;
        f64 d;
        DWord dw;
    };

    static constexpr u32 MaxExceptionLevel = 3;
    union {
        GPRegisters gpr;
        u32 list[32];
        i32 ilist[32];
    };
    SIMDRegister simd[SIMDRegistersCount];

    u32 pc;
    u32 offset;
    
    u32* mem_pc;

    u32 cycle_counter;
    u32 inst_counter;

    // System registers
    u32 current_el;
    ProgramStateRegister psr;

    ProgramStateRegister spsr_el[MaxExceptionLevel];
    u32 elr_el[MaxExceptionLevel - 1];
    u32 vtr_el[MaxExceptionLevel - 1];

    void initialize();

    void shutdown();

    void handle_pc_change();

    u32 fetch_inst();

    void dispatch(u32 num_cycles);

    void print_pegisters();
};

