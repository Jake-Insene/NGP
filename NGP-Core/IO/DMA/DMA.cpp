/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/DMA/DMA.h"

#include "Video/GU.h"
#include "Memory/Bus.h"


namespace DMA
{

enum DMAStatusFlags
{
    DMA_STATUS_ENABLE = 0x1,
};

static inline void dma_channel_write(DMAChannel channel, u8 reg, Word value)
{
    DMAChannelInfo& chn = dma_get_registers().channels[channel];

    chn.raw_regs[reg >> 2] = value;
    if (value & DMA_START)
    {
        switch (channel)
        {
        case DMA_RAM:
            break;
        case DMA_EMD:
            break;
        case DMA_SPU:
            break;
        case DMA_GU:
            ::GU::dma_send(chn.regs.dst, chn.regs.src, chn.regs.cnt, value);
            break;
        }
    }
}

static inline void dma_set_irq_mask(Word value)
{
    dma_get_registers().irq_mask = value;
}

static inline void dma_set_irq_status(Word value)
{
    dma_get_registers().irq_status = value;
}

static inline void dma_wait_on(Word value)
{
    dma_get_registers().wait_on_mask = value;
}

IO::IODevice dma_get_io_device()
{
    return IO::IODevice
    {
        .base_address = IO::DMA_BASE,

        .read_byte = [](VirtualAddress) -> u8 { return 0; },
        .read_half = [](VirtualAddress) -> u16 { return 0; },
        .read_word = [](VirtualAddress) -> Word { return 0; },
        .read_dword = [](VirtualAddress) -> DWord { return 0; },
        .read_qword = [](VirtualAddress) -> QWord { return QWord(); },

        .write_byte = [](VirtualAddress, u8) {},
        .write_half = [](VirtualAddress, u16) {},
        .write_word = &dma_handle_write_word,
        .write_dword = [](VirtualAddress, DWord) {},
        .write_qword = [](VirtualAddress, QWord) {},
    };
}

DMARegisters& dma_get_registers()
{
    return *(DMARegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IO::DMA_BASE);
}

void dma_handle_write_word(VirtualAddress address, Word value)
{
    if (address >= DMA_CHANNELS_START && address < DMA_CHANNELS_END)
    {
        dma_channel_write(DMAChannel((address & 0xF0) >> 4), (address & 0xF) >> 2, value);
        return;
    }

    switch (address)
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

}

