/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/IO.h"

#include "IO/Debug/Debug.h"
#include "IO/DMA/DMA.h"
#include "IO/GU/GU.h"
#include "IO/IRQ/IRQ.h"
#include "IO/Pad/Pad.h"
#include "Memory/Bus.h"
#include "Platform/OS.h"
#include "CPU/CPUCore.h"
#include <stdio.h>

namespace IO
{

void initialize()
{
    pad_reset();
}

void shutdown()
{}

u8 read_io_byte(CPUCore& core, VirtualAddress address)
{
    return 0;
}

u16 read_io_half(CPUCore& core, VirtualAddress address)
{
    return 0;
}

Word read_io_word(CPUCore& core, VirtualAddress address)
{
    u32 io_segment = (address & 0x0FFF'F000) >> 12;

    switch (io_segment)
    {
    case PAD_SEGMENT:
        return pad_handle_read_word(core, address);
    }

    return 0;
}

DWord read_io_dword(CPUCore& core, VirtualAddress address)
{
    return 0;
}

QWord read_io_qword(CPUCore& core, VirtualAddress address)
{
    return {};
}

void write_io_byte(CPUCore& core, VirtualAddress address, u8 value)
{
    // TODO: what could we do?
}

void write_io_half(CPUCore& core, VirtualAddress address, u16 value)
{
    // TODO: what could we do?
}

void write_io_word(CPUCore& core, VirtualAddress address, Word value)
{
    u32 io_segment = (address & 0x0FFF'F000) >> 12;

    switch (io_segment)
    {
    case DMA_SEGMENT:
        dma_handle_write_word(core, address, value);
        break;
    case IRQ_SEGMENT:
        irq_handle_write_word(core, address, value);
        break;
    case PAD_SEGMENT:
        pad_handle_write_word(core, address, value);
        break;
    case DEBUG_SEGMENT:
        debug_handle_write_word(core, address, value);
        break;
    case GU_SEGMENT:
        gu_handle_write_word(core, address, value);
        break;
    }
}

void write_io_dword(CPUCore& core, VirtualAddress address, DWord value)
{
    // TODO: what could we do?
}

void write_io_qword(CPUCore& core, VirtualAddress address, QWord value)
{
    // TODO: what could we do?
}


}
