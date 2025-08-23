/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/GU/GU.h"

#include "CPU/CPUCore.h"
#include "Memory/Bus.h"
#include "Video/GUDevice.h"


static inline void gu_irq_mask(VirtualAddress value)
{
}

static inline void gu_set_status(VirtualAddress value)
{
}

static inline void gu_set_ctr(VirtualAddress value)
{
}

IO::IODevice GU::get_io_device()
{
    return IO::IODevice
    {
        .base_address = IO::GU_BASE,

        .initialize = &GU::initialize,
        .shutdown = &GU::shutdown,
        .dispatch = &GU::dispatch,

        .read_byte = [](VirtualAddress) -> u8 { return 0; },
        .read_half = [](VirtualAddress) -> u16 { return 0; },
        .read_word = [](VirtualAddress) -> Word { return 0; },
        .read_dword = [](VirtualAddress) -> DWord { return 0; },
        .read_qword = [](VirtualAddress) -> QWord { return QWord(); },

        .write_byte = [](VirtualAddress, u8) {},
        .write_half = [](VirtualAddress, u16) {},
        .write_word = &GU::handle_write_word,
        .write_dword = [](VirtualAddress, DWord) {},
        .write_qword = [](VirtualAddress, QWord) {},
    };
}

void GU::initialize()
{
    get_registers().id = GU_GU1;
}

void GU::shutdown()
{}

void GU::dispatch()
{
    GUDevice::queue_dispatch();
}

void GU::handle_write_word(VirtualAddress local_address, Word value)
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
        if (value & QUEUE_START)
        {
            GUDevice::queue_execute(
                get_registers().queue_addr,
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

