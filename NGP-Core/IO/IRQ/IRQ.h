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

static constexpr u32 IRQ_STATUS = IRQ_BASE | 0x00;
static constexpr u32 IRQ_ENABLE_MASK = IRQ_BASE | 0x04;

enum IRQEnableMaskBit
{
    IRQ_ENABLE_DMA = 0x1,
    IRQ_ENABLE_TIMER = 0x2,
    IRQ_ENABLE_SPU = 0x4,
    IRQ_ENABLE_EMD = 0x8,

    IRQ_ENABLE_VBLANK = 0x10,

    IRQ_ENABLE_PAD = 0x800'0000,
};

void irq_handle_write_word(CPUCore& core, VirtualAddress address, Word value);

}
