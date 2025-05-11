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

static constexpr VirtualAddress DMA_ENABLE_MASK =   DMA_BASE | 0x100;
static constexpr VirtualAddress DMA_IRQ_MASK =      DMA_BASE | 0x104;
static constexpr VirtualAddress DMA_PRIORITY_MASK = DMA_BASE | 0x108;
static constexpr VirtualAddress DMA_WAIT_ON_MASK =  DMA_BASE | 0x10C;


enum DMAChannel
{
    DMA_RAM = 0,
    DMA_GU = 1,
    DMA_EMD = 2,
    DMA_SPU = 3,

    DMA_CHANNELS_MAX = 16,
};

enum DMAEnableMaskBit
{
    DMA_RAM_MASK = 0x1,
    DMA_GU_MASK = 0x2,
    DMA_EMD_MASK = 0x4,
    DMA_SPU_MASK = 0x8,
};

enum DMAControlChannelBit
{
    DMA_START = 0x1,
    DMA_BUSY = 0x1,
};

enum DMAStatusFlags
{
    DMA_STATUS_ENABLE = 0x1,
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

    Word enable_mask;
    Word irq_mask;
    Word priority_mask;
    Word wait_on_mask;
};

DMARegisters& get_dma_registers();

void dma_handle_write_word(CPUCore& core, VirtualAddress address, Word value);

}
