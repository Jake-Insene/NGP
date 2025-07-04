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

struct Display
{

    enum Register
    {
        // [0] HBLANK
        DISPLAY_IRQ_MASK =      0x0000,
        DISPLAY_IRQ_STATUS =    0x0004,
        
        // Display Control
        // [0] Enable
        // [1] Present
        DISPLAY_CTR =   0x0008,
        DISPLAY_ID =    0x000C,
        
        DISPLAY_BUFFER_ADDR = 0x0010,
        // [0 - 13] Width
        // [14 - 27] Height
        // [28 - 31] Display Format
        DISPLAY_FORMAT = 0x0014,
    };

    enum DisplayIRQMask
    {
        IRQ_MASK_HBLANK = 0x1,
    };

    enum DisplayControlBit
    {
        ENABLE = 0x1,
        PRESENT = 0x2,
    };

    enum DisplayID
    {
        DISPLAY_1 = 0x0,
    };

    enum DisplayFormat
    {
        FORMAT_RGBA8 = 0x0,
        FORMAT_RGB8 = 0x1,
        FORMAT_RGBA4 = 0x2,
        FORMAT_RGB565 = 0x3,
        FORMAT_RGBA5551 = 0x4,
    };

    struct DisplayRegisters
    {
        Word irq_mask;
        Word irq_status;
        Word ctr;
        Word id;
        Word buffer_addr;
        Word format;
    };

    static IO::IODevice get_io_device();
    static DisplayRegisters& get_registers()
    {
        return *(DisplayRegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IO::DISPLAY_BASE);
    }

    static void initialize();
    static void shutdown();

    static void handle_write_word(VirtualAddress local_address, Word value);

};