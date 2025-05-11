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

static constexpr VirtualAddress GU_IRQ_MASK =   GU_BASE | 0x0000;
static constexpr VirtualAddress GU_STATUS =     GU_BASE | 0x0004;
static constexpr VirtualAddress GU_CTR =        GU_BASE | 0x0008;
static constexpr VirtualAddress GU_ID =         GU_BASE | 0x000C;

// Display Target
static constexpr VirtualAddress GU_DISPLAYADR = GU_BASE | 0x0010;
// [0 - 11]     Width
// [12 - 23]    Height
// [24 - 31]    Display Format
static constexpr VirtualAddress GU_DISPLAYFMT = GU_BASE | 0x0014;

// Transfer Protocol
static constexpr VirtualAddress GU_TRANSFER_INADDR =    GU_BASE | 0x0100;
static constexpr VirtualAddress GU_TRANSFER_INFMT =     GU_BASE | 0x0104;
static constexpr VirtualAddress GU_TRANSFER_OUTADDR =   GU_BASE | 0x0108;
static constexpr VirtualAddress GU_TRANSFER_OUTFMT =    GU_BASE | 0x010C;

static constexpr VirtualAddress GU_TRANSFER_COUNT =     GU_BASE | 0x0110;
// [0 - 1] -> GUTransferCommand
// [31] -> Start execution
static constexpr VirtualAddress GU_TRANSFER_CTR =       GU_BASE | 0x0114;
static constexpr VirtualAddress GU_TRANSFER_IRQ_MASK =  GU_BASE | 0x0118;
static constexpr VirtualAddress GU_TRANSFER_ID =        GU_BASE | 0x011C;

// Command Queue
// [0 - 2] Command List Type (Graphics, MISC)
// [3 - 4] Execution Priority (Low, Normal, High)
static constexpr VirtualAddress GU_QUEUE_CMD =  GU_BASE | 0x0120;
static constexpr VirtualAddress GU_QUEUE_ADDR = GU_BASE | 0x0124;
// [31] -> Start execution
static constexpr VirtualAddress GU_QUEUE_CTR =      GU_BASE | 0x0128;
static constexpr VirtualAddress GU_QUEUE_IRQ_MASK = GU_BASE | 0x012C;

static constexpr VirtualAddress GU_QUEUE_STATE =    GU_BASE | 0x0130;

enum GUControlBit
{
    GU_ENABLE_FLAG = 0x1,
};

enum GUQueueCommandFlags
{
    GU_QUEUE_EXECUTE_GRAPHICS = 0x0,
    GU_QUEUE_EXECUTE_MISC = 0x1,

    GU_QUEUE_PRIORITY_LOW = 0x8,
    GU_QUEUE_PRIORITY_NORMAL = 0x10,
    GU_QUEUE_PRIORITY_HIGH = 0x20,
};

enum GUTransferCommand
{
    GU_TRANSFER_DISPLAY = 0x1,
    GU_TRANSFER_TEXTURE_COPY = 0x2,
};

struct GURegisters
{
    Word irq_mask;
    Word status;
    Word ctr;
    Word id;

    VirtualAddress display_address;
    Word display_format;

    Word padding2[(GU_TRANSFER_INADDR - GU_DISPLAYFMT) / 4 - 1];
    // 0x0100
    VirtualAddress transfer_in_address;
    VirtualAddress transfer_in_fmt;

    VirtualAddress transfer_out_address;
    VirtualAddress transfer_out_fmt;

    Word transfer_count;
    Word transfer_ctr;
    Word transfer_irq_mask;
    Word transfer_id;

    Word queue_cmd;
    Word queue_addr;
    Word queue_ctr;
    Word queue_irq_mask;
    Word queue_state;
};

GURegisters& get_gu_registers();

void gu_handle_write_word(CPUCore& core, VirtualAddress address, Word value);

}
