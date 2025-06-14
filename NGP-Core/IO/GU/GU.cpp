/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/GU/GU.h"

#include "CPU/CPUCore.h"
#include "Memory/Bus.h"
#include "Video/GU.h"


namespace IO
{

static void gu_irq_mask(VirtualAddress value)
{
}

static void gu_set_status(VirtualAddress value)
{
}

static void gu_set_ctr(VirtualAddress value)
{
}

IODevice gu_get_io_device()
{
    return IODevice
    {
        .base_address = GU_BASE,

        .read_byte = [](VirtualAddress) -> u8 { return 0; },
        .read_half = [](VirtualAddress) -> u16 { return 0; },
        .read_word = [](VirtualAddress) -> Word { return 0; },
        .read_dword = [](VirtualAddress) -> DWord { return 0; },
        .read_qword = [](VirtualAddress) -> QWord { return QWord(); },

        .write_byte = [](VirtualAddress, u8) {},
        .write_half = [](VirtualAddress, u16) {},
        .write_word = &gu_handle_write_word,
        .write_dword = [](VirtualAddress, DWord) {},
        .write_qword = [](VirtualAddress, QWord) {},
    };
}

GURegisters& gu_get_registers()
{
    return *(GURegisters*)(Bus::MAPPED_BUS_ADDRESS_START + GU_BASE);
}

void gu_handle_write_word(VirtualAddress address, Word value)
{
    switch (address)
    {
    case GU_IRQ_MASK:
        gu_irq_mask(value);
        break;
    case GU_IRQ_STATUS:
        gu_set_status(value);
        break;
    case GU_CTR:
        gu_set_ctr(value);
        break;
    case GU_ID:
        // Ignored
        break;
    case GU_QUEUE_CTR:
        if (value & GU_QUEUE_START)
        {
            u8 queue_priority = value & 0x3;
            u8 queue = (value >> 2) & 0x3;
            GU::queue_execute(
                queue, queue_priority, gu_get_registers().queue_addr,
                gu_get_registers().queue_len
            );
        }
        break;
    case GU_QUEUE_STATE:
        break;
    case GU_QUEUE_ADDR:
        gu_get_registers().queue_addr = value;
        break;
    case GU_QUEUE_LEN:
        gu_get_registers().queue_len = value;
        break;
    }
}


}