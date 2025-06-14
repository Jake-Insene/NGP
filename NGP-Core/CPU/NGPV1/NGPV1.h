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

    // Level 0 has no save registers.
    // Saved Program State Register IRQ
    ProgramStateRegister spsr_irq;
    // Saved Program State Register
    SPSR_EL spsr;
    // Exception Return Register
    ELR_EL elr;
    // Exception Code Register
    ECR_EL ecr;
    // Vector Base Address Register
    // Composition
    // 0x00 -> Reset Handler
    // 0x04 -> IRQ Handler
    // 0x08 -> Exception Handler
    // 0x0C -> Unused
    VBAR_EL vbar;

    // PC registers
    VirtualAddress pc;
    Word* mem_pc;

    // Object fields
    u64 clock_speed;

    // Jitter
    JIT::X86JIT jitter;

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

    virtual void handle_exception(ExceptionCode, VirtualAddress addr) override;

    usize run(usize num_cycles);

    void handle_pc_change();
    Word fetch_inst();

    void make_exception(ExceptionCode code, VirtualAddress vec_offset, u16 comment);
    void return_exception();

    void handle_breakpoint(u16 comment);
};

