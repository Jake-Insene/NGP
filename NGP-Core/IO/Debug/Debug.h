/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "IO/IO.h"


struct CPUCore;


namespace IO
{

static constexpr VirtualAddress DEBUG_CTR =         DEBUG_BASE | 0x000;
static constexpr VirtualAddress DEBUG_IRQ_MASK =    DEBUG_BASE | 0x004;
static constexpr VirtualAddress DEBUG_FIFO =        DEBUG_BASE | 0x008;

enum DebugControlBit
{
    DEBUG_ENABLE = 0x1,
};

struct DebugRegisters
{
    Word ctr;
    Word irq_mask;
    Word fifo;
};

DebugRegisters& get_debug_registers();

void debug_handle_write_word(CPUCore& core, VirtualAddress address, Word value);

}
