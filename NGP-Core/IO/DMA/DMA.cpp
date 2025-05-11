/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/DMA/DMA.h"

#include "Memory/Bus.h"


namespace IO
{

struct DMAChannelState
{
    u32 status;
};

static inline DMAChannelState channels[DMA_CHANNELS_MAX] = {};

void dma_channel_write(u8 channel, u8 reg, u32 value)
{
    switch (reg)
    {
    case 0: // DMA_CTR
        if (value & DMA_START)
        {

        }
        break;
    case 1: // DMA_SRC
        break;
    case 2: // DMA_DST
        break;
    case 3: // DMA_CNT
        break;
    }

    get_dma_registers().channels[channel].raw_regs[reg & 0x3] = value;
}

void dma_set_enable(u8 channel, u32 value)
{
    channels[channel].status = DMA_STATUS_ENABLE;
    
    get_dma_registers().enable_mask = value;
}

void dma_set_irq(u32 value)
{
    get_dma_registers().irq_mask = value;
}

void dma_set_priority(u32 value)
{
    get_dma_registers().priority_mask = value;
}

void dma_wait_on(u32 value)
{
    get_dma_registers().wait_on_mask = value;
}

DMARegisters& get_dma_registers()
{
    return *(DMARegisters*)(Bus::MAPPED_BUS_ADDRESS_START + DMA_BASE);
}

void dma_handle_write_word(CPUCore& core, VirtualAddress address, Word value)
{
    if (address >= DMA_CHANNELS_START && address < DMA_CHANNELS_END)
    {
        dma_channel_write((address & 0xF0) >> 4, (address & 0xF) >> 2, value);
        return;
    }

    switch (address)
    {
    case DMA_ENABLE_MASK:
        dma_set_enable((address & 0xF0) >> 4, value);
        break;
    case DMA_IRQ_MASK:
        dma_set_irq(value);
        break;
    case DMA_PRIORITY_MASK:
        dma_set_priority(value);
        break;
    case DMA_WAIT_ON_MASK:
        dma_wait_on(value);
        break;
    }
}

}

