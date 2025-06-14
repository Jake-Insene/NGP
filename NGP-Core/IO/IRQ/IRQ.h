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

// [0] DMA
// [1] TIMER
// [2] SPU
// [3] EMD
// [4] GU
// [5] Display
// [31] PAD
static constexpr VirtualAddress IRQ_MASK =   0x00;
static constexpr VirtualAddress IRQ_STATUS = 0x04;

enum IRQMask
{
    IRQ_MASK_DMA = 0x1,
    IRQ_MASK_TIMER = 0x2,
    IRQ_MASK_SPU = 0x4,
    IRQ_MASK_EMD = 0x8,
    IRQ_MASK_GU = 0x10,

    IRQ_MASK_PAD = 0x800'0000,
};

struct IRQRegisters
{
    Word irq_mask;
    Word irq_status;
};

IODevice irq_get_io_device();
IRQRegisters& irq_get_registers();

void irq_handle_write_word(VirtualAddress address, Word value);

}
