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

static constexpr VirtualAddress DMA_CHANNELS_START =    0x000;
static constexpr VirtualAddress DMA_CHANNELS_END =      0x0FF;

// DMA Channel Mask
// [0] RAM
// [1] EMD
// [2] SPU
// [3] GU
static constexpr VirtualAddress DMA_IRQ_MASK =          0x100;
static constexpr VirtualAddress DMA_IRQ_STATUS =        0x104;
static constexpr VirtualAddress DMA_WAIT_ON_MASK =      0x108;

// DMA Channel Control Register
// [0] Start / Busy
// [1 - 2] Priority
// 
// - GU:
//      [3 - 5] Dest Texture Format
//      [6 - 8] Src Texture Format
//      [9] Transfer Direction: 0(RAM -> VRAM), 1(VRAM -> RAM)
// 
// [30] Fill Mode: 0(Normal fill mode, same as a copy from dest -> src),
//                 1(Fill dest memory range with the value in the src register)
// [31] IRQ Enable

enum DMAChannel
{
    DMA_RAM = 0,
    DMA_EMD = 1,
    DMA_SPU = 2,
    DMA_GU = 3,

    DMA_CHANNELS_MAX = 16,
};

enum DMAIRQMask
{
    DMA_IRQ_MASK_RAM = 0x1,
    DMA_IRQ_MASK_EMD = 0x2,
    DMA_IRQ_MASK_SPU = 0x4,
    DMA_IRQ_MASK_GU = 0x4,
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

enum DMAGUTextureFormat
{
    GU_DMA_TEXTURE_R8 = 0x0,
    GU_DMA_TEXTURE_RG8 = 0x1,
    GU_DMA_TEXTURE_RGB8 = 0x2,
    GU_DMA_TEXTURE_RGBA8 = 0x3,
};

struct DMAChannelRegisters
{
    Word ctr;
    Word src;
    Word dst;
    Word cnt;
};

struct DMAChannelInfo
{
    union
    {
        DMAChannelRegisters regs;
        Word raw_regs[4];
    };
};

struct DMARegisters
{
    DMAChannelInfo channels[16];

    Word irq_mask;
    Word irq_status;
    Word wait_on_mask;
};

IODevice dma_get_io_device();
DMARegisters& dma_get_registers();

void dma_handle_write_word(VirtualAddress address, Word value);

}
