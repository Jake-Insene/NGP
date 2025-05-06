/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/DMA/DMA.h"

#include "IO/IORegisters.h"

namespace IO
{

void dma_channel_write(u8 channel, u8 reg, u32 value)
{
    get_io_registers().dma.channels[channel].raw_regs[reg] = value;
}

void dma_set_enable(u32 value)
{
    if (value & DMA_RAM_MASK)
    {
        get_io_registers().dma.channels[DMA_RAM].status = DMA_STATUS_ENABLE;
    }

    get_io_registers().dma.enable_mask = value;
}

void dma_set_irq(u32 value)
{
    get_io_registers().dma.irq_mask = value;
}

void dma_set_priority(u32 value)
{
    get_io_registers().dma.priority_mask = value;
}

void dma_wait_on(u32 value)
{
    get_io_registers().dma.wait_on_mask = value;
}

void dma_handle_write_word(CPUCore& core, VirtualAddress address, Word value)
{
    if (address >= DMA_CHANNELS_START && address < 0x100)
    {
        dma_channel_write((address & 0xFFF) >> 4, (address & 0xFFF) >> 2, value);
        return;
    }

    switch (address)
    {
    case DMA_ENABLE_MASK:
        dma_set_enable(value);
        break;
    case DMA_IRQ_MASK:
        dma_set_irq(value);
        break;
    case DMA_PRIORITY_MASK:
        dma_set_enable(value);
        break;
    case DMA_WAIT_ON_MASK:
        dma_wait_on(value);
        break;
    }
}

}

