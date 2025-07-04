/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "CPU/CPUCore.h"
#include "FileFormat/ISA.h"
#include "CPU/JIT/X86/X86JIT.h"


struct alignas(64) NGPV1 : CPUCore
{
    enum FetchType
    {
        FetchAlignCheck = 0,
        FetchAlignNoCheck = 0,
    };

    union
    {
        GPRegisters gpr;
        u32 list[32];
        i32 ilist[32];
    };
    SIMDRegisterV1 simd[SIMDRegistersCount];

    // CPU Registers
    // System registers
    ProgramStateRegister psr;

    struct
    {
        // Saved Program State Registers
        SPSR_EL spsr;
        // Exception Description Registers
        EDR_EL edr;
        // Exception Return Registers
        ELR_EL elr;
        // Vector Base Address Register
        // Composition
        // VBAR + 0x00 -> Reset Handler
        // VBAR + 0x04 -> Exception Handler
        // VBAR + 0x08 -> IRQ Handler
        // VBAR + 0x0C -> Unused
        VBAR_EL vbar;

        FAR_EL far;
    } system_regs;

    // PC registers
    VirtualAddress pc;
    
    // Jitter
    JIT::X86JIT jitter;

    // instruction cache
    Word pc_page_index;
    Word pc_page_offset;
    const Word* pc_page_addr;

#if defined(NGP_BUILD_VAR)
#define OVERRIDE override
#else
#define OVERRIDE
#endif

    void initialize() OVERRIDE;
    void shutdown() OVERRIDE;

    usize dispatch(usize num_cycles) OVERRIDE;

    void print_registers() OVERRIDE;

    void set_psr(ProgramStateRegister new_psr) OVERRIDE;
    ProgramStateRegister get_psr() OVERRIDE;
    void set_pc(VirtualAddress new_pc) OVERRIDE;
    VirtualAddress get_pc() OVERRIDE;

    void external_handle_exception(ExceptionCode code, ExceptionComment comment, VirtualAddress addr) OVERRIDE;

    usize run(usize num_cycles);

    void handle_pc_change();
    Word fetch_next_inst();

    void make_exception(ExceptionCode code, VirtualAddress vec_offset, u16 comment);
    void return_exception();

    void handle_breakpoint(u16 comment);
};

