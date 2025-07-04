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


struct alignas(64) CPUCore
{
    enum class CPUType
    {
        Unknown = 0,
        V1,
    };

    static constexpr Word ZeroRegister = 31;
    static constexpr Word LinkRegister = 30;
    static constexpr Word StackPointerRegister = 29;
    static constexpr Word MaxExceptionLevel = 3;
    static constexpr Word MaxExceptionLevelCount = 4;
    static constexpr Word VBASize = sizeof(VirtualAddress) * 4;
    static constexpr Word ResetVBOffset = 0;
    static constexpr Word ExceptionVBOffset = 4;
    static constexpr Word IRQVBOffset = 8;

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
        SupervisorException = 0x0,
        ExtendedSupervisorException = 0x1,
        SecureMachineControllerException = 0x2,

        BreakpointException = 0x4,
        AccessViolationException = 0x5,
        DivideByZeroException = 0x6,
        BadSystemRegAlignment = 0x7,
        BadPCAlignment = 0x8,
        BadMemAlignment = 0x9,
    };

    enum ExceptionComment
    {
        CommentNone = 0,
        // On AccessViolation
        CantExecute = 1,
        CantRead = 2,
        CantWrite = 3,
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
        ProgramStateRegister spsr[3];
    };

    struct EDR
    {
        Word comment : 20;
        Word exception_code : 12;
    };

    struct EDR_EL
    {
        Word el1;
        Word el2;
        Word el3;
        Word edr_el[3];
    };

    struct ELR_EL
    {
        struct
        {
            VirtualAddress el1;
            VirtualAddress el2;
            VirtualAddress el3;
        };
        VirtualAddress elr_el[3];
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

    struct FAR_EL
    {
        Word far_el1;
        Word far_el2;
        Word far_el3;
        Word far_el[3];
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

    union Vec128
    {
        struct
        {
            f32 x, y, z, w;
        } vec4;

        struct
        {
            f64 x, y;
        } vec2;

        f32 s4[4];
        f64 d2[2];
    };

    union SIMDRegisterV1
    {
        Vec128 vec;
        QWord qw;
        f32 s;
        Word w;
        f64 d;
        DWord dw;
    };

    static CPUCore* create_cpu(CPUType type);
#if defined(NGP_BUILD_VAR)
    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    virtual usize dispatch(usize num_cycles) = 0;

    virtual void print_registers() = 0;

    virtual void set_psr(ProgramStateRegister new_psr) = 0;
    virtual ProgramStateRegister get_psr() = 0;
    virtual void set_pc(VirtualAddress new_pc) = 0;
    virtual VirtualAddress get_pc() = 0;

    virtual void external_handle_exception(ExceptionCode code, ExceptionComment comment, VirtualAddress addr) = 0;
#endif
};

