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

static constexpr VirtualAddress DMA_CHANNELS_START =    DMA_BASE | 0x000;
static constexpr VirtualAddress DMA_CHANNELS_END =      DMA_BASE | 0x0FF;

// DMA Channel Mask
// [0] RAM
// [1] EMD
// [2] SPU
static constexpr VirtualAddress DMA_IRQ_MASK =      DMA_BASE | 0x100;
static constexpr VirtualAddress DMA_IRQ_STATUS =    DMA_BASE | 0x104;
static constexpr VirtualAddress DMA_WAIT_ON_MASK =  DMA_BASE | 0x108;


enum DMAChannel
{
    DMA_RAM = 0,
    DMA_EMD = 1,
    DMA_SPU = 2,

    DMA_CHANNELS_MAX = 16,
};

enum DMAIRQMask
{
    DMA_IRQ_MASK_RAM = 0x1,
    DMA_IRQ_MASK_EMD = 0x2,
    DMA_IRQ_MASK_SPU = 0x4,
};

enum DMAControlChannelBit
{
    DMA_START = 0x1,
    DMA_BUSY = 0x1,
    
    DMA_PRIORITY_LOW = 0x2,
    DMA_PRIORITY_NORMAL = 0x4,
    DMA_PRIORITY_HIGH = 0x6,

    DMA_IRQ = 0x8000'0000,
};

struct DMAChannelRegisters
{
    u32 ctr;
    u32 src;
    u32 dst;
    u32 cnt;
};

struct DMAChannelInfo
{
    union
    {
        DMAChannelRegisters regs;
        u32 raw_regs[4];
    };
};

struct DMARegisters
{
    DMAChannelInfo channels[16];

    Word irq_mask;
    Word irq_status;
    Word wait_on_mask;
};

DMARegisters& get_dma_registers();

void dma_handle_write_word(CPUCore& core, VirtualAddress address, Word value);

}
