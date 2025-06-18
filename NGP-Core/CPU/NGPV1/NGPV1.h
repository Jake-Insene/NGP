/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "CPU/CPUCore.h"
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

    // Object fields
    u64 clock_speed;

    // Jitter
    JIT::X86JIT jitter;

    // instruction cache
    Word pc_page_index;
    Word pc_page_offset;
    const Word* pc_page_addr;

    virtual void initialize() override;
    virtual void shutdown() override;

    virtual usize dispatch(usize num_cycles) override;

    virtual void print_registers() override;

    virtual void set_psr(ProgramStateRegister psr) override;
    virtual ProgramStateRegister get_psr() override;
    virtual void set_pc(VirtualAddress new_pc) override;
    virtual VirtualAddress get_pc() override;

    virtual void set_clock_speed(usize new_clock_speed) override;
    virtual usize get_clock_speed() override;

    virtual void external_handle_exception(ExceptionCode code, ExceptionComment comment, VirtualAddress addr) override;

    usize run(usize num_cycles);

    void handle_pc_change();
    Word fetch_next_inst();

    void make_exception(ExceptionCode code, VirtualAddress vec_offset, u16 comment);
    void return_exception();

    void handle_breakpoint(u16 comment);
};

