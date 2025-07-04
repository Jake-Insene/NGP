/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/IO.h"

#include "IO/Display/Display.h"
#include "IO/DMA/DMA.h"
#include "IO/GU/GU.h"
#include "IO/IRQ/IRQ.h"
#include "IO/Pad/Pad.h"
#include "IO/USI/USI.h"

#include "Memory/Bus.h"

#include "Platform/OS.h"

#include "CPU/CPUCore.h"

#include <vector>

namespace IO
{

static inline std::vector<IODevice> io_devices = {};

static inline IODevice get_default_io_device(VirtualAddress base_address)
{
    return IODevice
    {
        .base_address = base_address,

        .initialize = []() {},
        .shutdown = []() {},
        .dispatch = []() {},

        .read_byte = [](VirtualAddress va) -> u8 { Bus::invalid_read(va); return 0; },
        .read_half = [](VirtualAddress va) -> u16 { Bus::invalid_read(va); return 0; },
        .read_word = [](VirtualAddress va) -> Word { Bus::invalid_read(va); return 0; },
        .read_dword = [](VirtualAddress va) -> DWord { Bus::invalid_read(va); return 0; },
        .read_qword = [](VirtualAddress va) -> QWord { Bus::invalid_read(va); return QWord(); },

        .write_byte = [](VirtualAddress va, u8) { Bus::invalid_write(va); },
        .write_half = [](VirtualAddress va, u16) { Bus::invalid_write(va); },
        .write_word = [](VirtualAddress va, Word) { Bus::invalid_write(va); },
        .write_dword = [](VirtualAddress va, DWord) { Bus::invalid_write(va); },
        .write_qword = [](VirtualAddress va, QWord) { Bus::invalid_write(va); },
    };
}

void initialize()
{
    for (usize io_page = 0; io_page < LAST_SEGMENT; io_page++)
    {
        switch (io_page)
        {
        case IRQ_SEGMENT:
            io_devices.emplace_back(IRQ::get_io_device());
            break;
        case DMA_SEGMENT:
            io_devices.emplace_back(DMA::get_io_device());
            break;
        case PAD_SEGMENT:
            io_devices.emplace_back(Pad::get_io_device());
            break;
        case USI_SEGMENT:
            io_devices.emplace_back(USI::get_io_device());
            break;
        case DISPLAY_SEGMENT:
            io_devices.emplace_back(Display::get_io_device());
            break;
        case GU_SEGMENT:
            io_devices.emplace_back(GU::get_io_device());
            break;
        default:
            io_devices.emplace_back(get_default_io_device(IO_BASE | (io_page << SegmentBits)));
            break;
        }
    }

    for (auto& device : io_devices)
    {
        device.initialize();
    }
}

void shutdown()
{
    for (auto& device : io_devices)
    {
        device.shutdown();
    }

    io_devices.clear();
}

void dispatch()
{
    for (auto& device : io_devices)
    {
        device.dispatch();
    }
}

u8 read_io_byte(VirtualAddress address)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return 0;
    }

    VirtualAddress local_address = address & 0xFFF;
    return io_devices[io_segment].read_byte(local_address);
}

u16 read_io_half(VirtualAddress address)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return 0;
    }

    VirtualAddress local_address = address & 0xFFF;
    return io_devices[io_segment].read_half(local_address);
}

Word read_io_word(VirtualAddress address)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return 0;
    }

    VirtualAddress local_address = address & 0xFFF;
    return io_devices[io_segment].read_word(local_address);
}

DWord read_io_dword(VirtualAddress address)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return 0;
    }

    VirtualAddress local_address = address & 0xFFF;
    return io_devices[io_segment].read_dword(local_address);
}

QWord read_io_qword(VirtualAddress address)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return QWord();
    }

    VirtualAddress local_address = address & 0xFFF;
    return io_devices[io_segment].read_qword(local_address);
}

void write_io_byte(VirtualAddress address, u8 value)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return;
    }

    VirtualAddress local_address = address & 0xFFF;
    io_devices[io_segment].write_byte(local_address, value);
}

void write_io_half(VirtualAddress address, u16 value)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return;
    }

    VirtualAddress local_address = address & 0xFFF;
    io_devices[io_segment].write_half(local_address, value);
}

void write_io_word(VirtualAddress address, Word value)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return;
    }
 
    VirtualAddress local_address = address & 0xFFF;
    io_devices[io_segment].write_word(local_address, value);
}

void write_io_dword(VirtualAddress address, DWord value)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return;
    }

    VirtualAddress local_address = address & 0xFFF;
    io_devices[io_segment].write_dword(local_address, value);
}

void write_io_qword(VirtualAddress address, QWord value)
{
    const u32 io_segment = (address & 0x0FFF'F000) >> 12;
    if (io_segment >= LAST_SEGMENT)
    {
        Bus::invalid_write(address);
        return;
    }

    VirtualAddress local_address = address & 0xFFF;
    io_devices[io_segment].write_qword(local_address, value);
}


}
