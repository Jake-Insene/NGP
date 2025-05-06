/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "IO/IO.h"
#include "IO/DMA/DMA.h"
#include "IO/IRQ/IRQ.h"
#include "IO/Pad/Pad.h"
#include "Memory/Bus.h"

namespace IO
{

#define IO_REGISTER_ALIGNMENT 0x1000

struct IORegisters
{
    struct alignas(IO_REGISTER_ALIGNMENT) DMARegisters
    {
        DMAChannelInfo channels[16];

        u32 enable_mask;
        u32 irq_mask;
        u32 priority_mask;
        u32 wait_on_mask;
    } dma;


    struct alignas(IO_REGISTER_ALIGNMENT) IRQRegisters
    {
        u32 irq_status;
        u32 enable_mask;
    } irq;

    struct alignas(IO_REGISTER_ALIGNMENT) PadRegisters
    {
        MainPad main_pad;
    } pad;
};

inline IORegisters& get_io_registers() { return *(IORegisters*)Bus::io_start_address(); }

}

