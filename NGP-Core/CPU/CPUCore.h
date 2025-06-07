/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"
#include "Memory/Bus.h"

#undef OVERFLOW


static constexpr u32 DEFUALT_CORE_CLOCK_SPEED = MHZ(100);

struct alignas(64) CPUCore
{
    enum class CPUType
    {
        Unknown = 0,
        V1,
    };

    static constexpr u32 ZeroRegister = 31;
    static constexpr u32 LinkRegister = 30;
    static constexpr u32 StackPointerRegister = 29;
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
        AccessViolation = 0x7,
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

    static CPUCore* create_cpu(CPUType type);

    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    virtual usize dispatch(usize num_cycles) = 0;

    virtual void print_registers() = 0;

    virtual void set_psr(ProgramStateRegister psr) = 0;
    virtual ProgramStateRegister get_psr() = 0;
    virtual void set_pc(VirtualAddress new_pc) = 0;
    virtual VirtualAddress get_pc() = 0;

    virtual void set_clock_speed(usize new_clock_speed) = 0;
    virtual usize get_clock_speed() = 0;

    virtual void handle_exception(ExceptionCode, VirtualAddress addr) = 0;
};

