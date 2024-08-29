/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/IO.h"
#include "IO/DMA.h"

namespace IO {

enum DMAStatusFlags {
    DMA_STATUS_ENABLE = 0x1,
};

struct DMAChannelInfo {
    u32 status;
} dma_channels[16];

void dma_channel_write(u8 channel, u8 reg, u32 value) {
    u32* chnl = (u32*)io_start_address();
    chnl += (channel * 10);

    if (reg == DMA_CTR) {
        if (value & DMA_START) {
            if (dma_channels[channel].status & DMA_ENABLE_MASK) {
            }
            else {
                // TODO: Generate a exception
            }
        }
    }

    chnl[reg] = value;
}

void dma_set_enable(u32 value) {
    u32* io = (u32*)io_start_address();

    if (value & DMA_RAM_MASK) {
        dma_channels[DMA_RAM].status = DMA_STATUS_ENABLE;
    }

    io[DMA_ENABLE_MASK] = value;
}

void dma_set_irq(u32 value) {
    u32* io = (u32*)io_start_address();
    io[DMA_IRQ_MASK] = value;
}

void dma_set_priority(u32 value) {
    u32* io = (u32*)io_start_address();
    io[DMA_PRIORITY_MASK] = value;
}

void dma_wait_on(u32 value) {
    u32* io = (u32*)io_start_address();
    io[DMA_WAIT_ON_MASK] = value;
}

}

