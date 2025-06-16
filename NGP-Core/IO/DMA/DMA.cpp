/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/DMA/DMA.h"

#include "Video/GU.h"
#include "Memory/Bus.h"


static inline void dma_channel_write(DMA::DMAChannel channel, u8 reg, Word value)
{
    DMA::DMAChannelInfo& chn = DMA::get_registers().channels[channel];
    chn.raw_regs[reg] = value;
}

static inline void dma_set_irq_mask(Word value)
{
    DMA::get_registers().irq_mask = value;
}

static inline void dma_set_irq_status(Word value)
{
    DMA::get_registers().irq_status = value;
}

static inline void dma_wait_on(Word value)
{
    DMA::get_registers().wait_on_mask = value;
}

IO::IODevice DMA::get_io_device()
{
    return IO::IODevice
    {
        .base_address = IO::DMA_BASE,

        .initialize = &DMA::initialize,
        .shutdown = &DMA::shutdown,
        .dispatch = &DMA::dispatch,

        .read_byte = [](VirtualAddress) -> u8 { return 0; },
        .read_half = [](VirtualAddress) -> u16 { return 0; },
        .read_word = [](VirtualAddress) -> Word { return 0; },
        .read_dword = [](VirtualAddress) -> DWord { return 0; },
        .read_qword = [](VirtualAddress) -> QWord { return QWord(); },

        .write_byte = [](VirtualAddress, u8) {},
        .write_half = [](VirtualAddress, u16) {},
        .write_word = &DMA::handle_write_word,
        .write_dword = [](VirtualAddress, DWord) {},
        .write_qword = [](VirtualAddress, QWord) {},
    };
}

void DMA::initialize()
{}

void DMA::shutdown()
{}

void DMA::dispatch()
{
    std::lock_guard<std::mutex> dma_mutex_guard{ dma_mutex };
    for (DMAChannel ch = DMA_CHANNEL_RAM; ch < DMA_CHANNELS_MAX; ((int&)ch)++)
    {
        DMAChannelInfo& channel = get_registers().channels[ch];
        if (channel.ctr & DMA_BUSY)
        {
            switch (ch)
            {
            case DMA_CHANNEL_GU:
                GU::dma_send(channel.dst, channel.src, channel.cnt, channel.ctr);
                break;
            default:
                break;
            }
        }
    }
}

void DMA::handle_write_word(VirtualAddress local_address, Word value)
{
    std::lock_guard<std::mutex> dma_mutex_guard{ dma_mutex };
    if (local_address >= DMA_CHANNELS_START && local_address < DMA_CHANNELS_END)
    {
        dma_channel_write(DMAChannel((local_address & 0xF0) >> 4), (local_address & 0xF) >> 2, value);
        return;
    }

    switch (local_address)
    {
    case DMA_IRQ_MASK:
        dma_set_irq_mask(value);
        break;
    case DMA_IRQ_STATUS:
        dma_set_irq_status(value);
        break;
    case DMA_WAIT_ON_MASK:
        dma_wait_on(value);
        break;
    }
}

