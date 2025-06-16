/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/GUDevice/GUDevice.h"

#include "CPU/CPUCore.h"
#include "Memory/Bus.h"
#include "Video/GU.h"


static inline void gu_irq_mask(VirtualAddress value)
{
}

static inline void gu_set_status(VirtualAddress value)
{
}

static inline void gu_set_ctr(VirtualAddress value)
{
}

IO::IODevice GUDevice::get_io_device()
{
    return IO::IODevice
    {
        .base_address = IO::GU_BASE,

        .initialize = &GUDevice::initialize,
        .shutdown = &GUDevice::shutdown,
        .dispatch = &GUDevice::dispatch,

        .read_byte = [](VirtualAddress) -> u8 { return 0; },
        .read_half = [](VirtualAddress) -> u16 { return 0; },
        .read_word = [](VirtualAddress) -> Word { return 0; },
        .read_dword = [](VirtualAddress) -> DWord { return 0; },
        .read_qword = [](VirtualAddress) -> QWord { return QWord(); },

        .write_byte = [](VirtualAddress, u8) {},
        .write_half = [](VirtualAddress, u16) {},
        .write_word = &GUDevice::handle_write_word,
        .write_dword = [](VirtualAddress, DWord) {},
        .write_qword = [](VirtualAddress, QWord) {},
    };
}

void GUDevice::initialize()
{
    get_registers().id = GU_1;
}

void GUDevice::shutdown()
{}

void GUDevice::dispatch()
{
    GU::queue_dispatch();
}

void GUDevice::handle_write_word(VirtualAddress local_address, Word value)
{
    switch (local_address)
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
            ::GU::queue_execute(
                queue, queue_priority, get_registers().queue_addr,
                get_registers().queue_len
            );
        }
        break;
    case GU_QUEUE_STATE:
        break;
    case GU_QUEUE_ADDR:
        get_registers().queue_addr = value;
        break;
    case GU_QUEUE_LEN:
        get_registers().queue_len = value;
        break;
    }
}

