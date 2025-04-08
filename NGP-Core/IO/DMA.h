/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

namespace IO
{



enum DMAChannel
{
    DMA_RAM = 0,
    DMA_GPU = 1,
    DMA_IOPU = 2,
    DMA_CPU = 3,
    DMA_SPU = 4,
};

enum DMAChannelBit
{
    DMA_RAM_MASK = 0x1,
    DMA_GPU_MASK = 0x2,
    DMA_EMD_MASK = 0x4,
    DMA_SPU_MASK = 0x8,
};

enum DMACtrFlags
{
    DMA_START = 0x1,
    DMA_BUSY = 0x1,
};

enum DMAChannelRegister
{
    DMA_CTR = 0,
    DMA_SRC = 1,
    DMA_DST = 2,
    DMA_CNT = 3,
};

struct DMAChannelRegs
{
    u32 ctr;
    u32 src;
    u32 dst;
    u32 cnt;
};

void dma_channel_write(u8 channel, u8 reg, u32 value);

void dma_set_enable(u32 value);
void dma_set_irq(u32 value);
void dma_set_priority(u32 value);
void dma_wait_on(u32 channel);

}
