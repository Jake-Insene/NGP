/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/Debug/Debug.h"

#include "Memory/Bus.h"

#include <cstdio>


namespace IO
{

static void debug_enable()
{

}

static void debug_write(Word value)
{
    if(get_debug_registers().ctr & DEBUG_ENABLE)
    {
        std::fputc(value, stderr);
    }
}

DebugRegisters& get_debug_registers()
{
    return *(DebugRegisters*)(Bus::MAPPED_BUS_ADDRESS_START + DEBUG_BASE);
}

void debug_handle_write_word(CPUCore& core, VirtualAddress address, Word value)
{
    switch (address)
    {
    case DEBUG_CTR:
    {
        if (value & DEBUG_ENABLE)
        {
            debug_enable();
        }
        get_debug_registers().ctr = value;
    }
        break;
    case DEBUG_IRQ_MASK:
        get_debug_registers().irq_mask = value;
        break;
    case DEBUG_FIFO:
        debug_write(value);
        get_debug_registers().fifo = value;
        break;
    }
}

}
