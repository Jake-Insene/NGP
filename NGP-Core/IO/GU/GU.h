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

// DMA Channel Mask
// [0] Transfer
// [1] Queue
static constexpr VirtualAddress GU_IRQ_MASK =   GU_BASE | 0x0000;
static constexpr VirtualAddress GU_IRQ_STATUS = GU_BASE | 0x0004;

// GU Control
// [0] Restart
static constexpr VirtualAddress GU_CTR =        GU_BASE | 0x0008;
static constexpr VirtualAddress GU_ID =         GU_BASE | 0x000C;

// Transfer Protocol
static constexpr VirtualAddress GU_TRANSFER_INADDR =    GU_BASE | 0x0010;
static constexpr VirtualAddress GU_TRANSFER_INFMT =     GU_BASE | 0x0014;
static constexpr VirtualAddress GU_TRANSFER_OUTADDR =   GU_BASE | 0x0018;
static constexpr VirtualAddress GU_TRANSFER_OUTFMT =    GU_BASE | 0x001C;

// [0 - 7] -> GUTransferCommand
// [31] -> Start execution
static constexpr VirtualAddress GU_TRANSFER_CTR =       GU_BASE | 0x0020;
static constexpr VirtualAddress GU_TRANSFER_IRQ_MASK =  GU_BASE | 0x0024;
static constexpr VirtualAddress GU_TRANSFER_ID =        GU_BASE | 0x0028;

// Command Queue
// [0 - 2] Command List Type (Graphics, MISC)
// [3 - 4] Execution Priority (Low, Normal, High)
static constexpr VirtualAddress GU_QUEUE_CMD =      GU_BASE | 0x0030;
static constexpr VirtualAddress GU_QUEUE_ADDR =     GU_BASE | 0x0034;
// [31] -> Start execution
static constexpr VirtualAddress GU_QUEUE_CTR =      GU_BASE | 0x0038;
static constexpr VirtualAddress GU_QUEUE_STATE =    GU_BASE | 0x003C;

enum GUIRQMask
{
    GU_IRQ_MASK_TRANSFER = 0x1,
    GU_IRQ_MASK_QUEUE = 0x2,
};

enum GUControlBit
{
    GU_RESTART_FLAG = 0x1,
};

enum GUID
{
    GU1 = 0,
};

enum GUTransferControlBit
{
    GU_TRANSFER_START = 0x8000'0000,
};

enum GUTransferCommand
{
    GU_TRANSFER_TEXTURE_COPY = 0x0,
};

enum TransferID
{
    TRANSFER1 = 0,
};

enum GUQueueControlBit
{
    GU_QUEUE_START = 0x8000'0000,
};

enum GUQueueCommand
{
    GU_QUEUE_CMD_RUN = 0x0,
};

enum GUQueuePriority
{
    GU_QUEUE_PRIORITY_LOW = 0x1,
    GU_QUEUE_PRIORITY_NORMAL = 0x2,
    GU_QUEUE_PRIORITY_HIGH = 0x4,
};

struct GURegisters
{
    // 0x00
    Word irq_mask;
    Word irq_status;
    Word ctr;
    Word id;

    // 0x10
    VirtualAddress transfer_in_address;
    VirtualAddress transfer_in_fmt;

    VirtualAddress transfer_out_address;
    VirtualAddress transfer_out_fmt;

    // 0x0020
    Word transfer_ctr;
    Word transfer_irq_mask;
    Word transfer_id;
    Word padding[1];

    // 0x0030
    Word queue_cmd;
    Word queue_addr;
    Word queue_ctr;
    Word queue_state;
};

GURegisters& get_gu_registers();

void gu_handle_write_word(VirtualAddress address, Word value);

}
