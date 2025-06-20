/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "IO/IO.h"
#include "Memory/Bus.h"

struct CPUCore;

struct IRQ
{

    enum Register
    {
        // [0] DMA
        // [1] TIMER
        // [2] SPU
        // [3] USI
        // [4] GU
        // [5] Display
        // [31] PAD
        IRQ_MASK = 0x00,
        IRQ_STATUS = 0x04,
    };

    enum IRQMask
    {
        IRQ_MASK_DMA = 0x1,
        IRQ_MASK_TIMER = 0x2,
        IRQ_MASK_SPU = 0x4,
        IRQ_MASK_USI = 0x8,
        IRQ_MASK_GU = 0x10,

        IRQ_MASK_PAD = 0x800'0000,
    };

    struct IRQRegisters
    {
        Word irq_mask;
        Word irq_status;
    };

    static IO::IODevice get_io_device();
    static IRQRegisters& get_registers()
    {
        return *(IRQRegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IO::IRQ_BASE);
    }

    static void initialize();
    static void shutdown();

    static void handle_write_word(VirtualAddress local_address, Word value);

};
