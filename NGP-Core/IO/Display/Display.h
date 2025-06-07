/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "IO/IO.h"

struct CPUCore;

namespace IO
{

// [0] HBLANK
static constexpr VirtualAddress DISPLAY_IRQ_MASK =	DISPLAY_BASE | 0x0000;
static constexpr VirtualAddress DISPLAY_STATUS =	DISPLAY_BASE | 0x0004;
static constexpr VirtualAddress DISPLAY_CTR =	    DISPLAY_BASE | 0x0008;
static constexpr VirtualAddress DISPLAY_ID =	    DISPLAY_BASE | 0x000C;
static constexpr VirtualAddress DISPLAY_BUFFER =	DISPLAY_BASE | 0x0010;
// [0 - 13] Width
// [14 - 27] Height
// [28 - 31] Display Format
static constexpr VirtualAddress DISPLAY_FORMAT =	DISPLAY_BASE | 0x0014;
// Write anything to trigger a presentation
static constexpr VirtualAddress DISPLAY_PRESENT =   DISPLAY_BASE | 0x0018;


enum DisplayIRQMask
{
    DISPLAY_IRQ_MASK_HBLANK = 0x1,
};

enum DisplayControlBit
{
    DISPLAY_ENABLE = 0x1,
};

enum DisplayFormat
{
    DISPLAY_FORMAT_RGB8 = 0,
    DISPLAY_FORMAT_RGBA8 = 1,
    DISPLAY_FORMAT_RGB565 = 2,
    DISPLAY_FORMAT_RGBA4 = 3,
};

struct DisplayRegisters
{
    Word irq_mask;
    Word irq_status;
    Word ctr;
    Word id;
    Word buffer;
    Word format;
};

DisplayRegisters& get_display_registers();

void display_handle_write_word(VirtualAddress address, Word value);

}