/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"
#include "Memory/Bus.h"


#define PROFILE 1

static constexpr u32 DEFUALT_CORE_CLOCK_SPEED = MHZ(100);

struct alignas(64) CPUCore
{
    static constexpr u32 ZeroRegister = 31;
    static constexpr u32 MaxExceptionLevel = 3;
    static constexpr u32 MaxExceptionLevelCount = 4;
    static constexpr u32 VBASize = sizeof(VirtualAddress) * 4;

    enum PrivilegeLevel
    {
        EL0 = 0,
        EL1 = 1,
        EL2 = 2,
        EL3 = 3,

        UserMode = EL0,
        Supervisor = EL1,
        ExtendedSupervisor = EL2,
        SecureMachineController = EL3,
    };

    enum ExceptionCode
    {
        DivideByZeroException = 0x0,
        SupervisorException = 0x1,
        ExtendedSupervisorException = 0x2,
        SecureMachineControllerException = 0x3,

        Breakpoint = 0x4,
        InvalidRead = 0x5,
        InvalidWrite = 0x6,
    };

    union ProgramStateRegister
    {
        struct
        {
            u32 ZERO : 1;
            u32 CARRY : 1;
            u32 NEGATIVE : 1;
            u32 OVERFLOW : 1;

            u32 HALT : 1;

            u32 CURRENT_EL : 2;
        };
        u32 raw;
    };

    union SPSR_EL
    {
        struct
        {
            ProgramStateRegister el1;
            ProgramStateRegister el2;
            ProgramStateRegister el3;
        };
        ProgramStateRegister spsr[MaxExceptionLevel];
    };

    struct ELR_EL
    {
        struct
        {
            VirtualAddress el1;
            VirtualAddress el2;
            VirtualAddress el3;
        };
        VirtualAddress elr_el[MaxExceptionLevel];
    };

    struct VBAR_EL
    {
        struct
        {
            VirtualAddress el1;
            VirtualAddress el2;
            VirtualAddress el3;
        };
        VirtualAddress vbar_el[MaxExceptionLevel];
    };

    struct ECR_EL
    {
        Word el1;
        Word el2;
        Word el3;
        Word ecr_el[MaxExceptionLevel];
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

    union
    {
        GPRegisters gpr;
        u32 list[32];
        i32 ilist[32];
    };
    SIMDRegister simd[SIMDRegistersCount];

    // CPU Registers
    // System registers
    ProgramStateRegister psr;

    // Level 0 has no save registers.
    // Saved Program State Register IRQ
    ProgramStateRegister spsr_irq;
    // Saved Program State Register
    SPSR_EL spsr;
    // Exception Return Register
    ELR_EL elr;
    // Exception Code Register
    ECR_EL ecr;
    // Vector Table register
    // Composition
    // 0x00 -> Exception Handler
    // 0x04 -> Interrupt Handler
    VBAR_EL vbar;

    // PC registers
    VirtualAddress offset;
    VirtualAddress pc;
    Word* mem_pc;

    // Object fields
    u64 clock_speed;
    u64 cycle_counter;
    u64 cycles_in_second;
#if PROFILE
    u64 inst_counter;
#endif

    void initialize();
    void shutdown();

    void handle_pc_change();
    Word fetch_inst();

    void dispatch(u64 num_cycles);

    void print_registers();

    void make_exception(ExceptionCode code, VirtualAddress vec_offset, u16 comment);
    void return_exception();

    void handle_breakpoint(u16 comment);
};

