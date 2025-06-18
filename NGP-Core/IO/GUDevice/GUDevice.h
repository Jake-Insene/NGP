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

struct GUDevice
{

    enum Register
    {
        // DMA Channel Mask
        // [0] Transfer
        // [1] Queue
        GU_IRQ_MASK =   0x0000,
        GU_IRQ_STATUS = 0x0004,

        // GU Control
        // [0] Restart
        GU_CTR =        0x0008,
        GU_ID =         0x000C,
    
        // Command Queue
        // [0 - 1] Queue Priority
        // [2 - 3] Queue Index
        // [31] -> Start execution
        GU_QUEUE_CTR =      0x0010,
        // Command Queue State
        // [0 - 1] Queue State 0
        // [2 - 3] Queue State 1
        // [4 - 5] Queue State 2
        // [6 - 7] Queue State 3
        GU_QUEUE_STATE =    0x0014,
        // Command List Address
        // [0 - 31] Command List Base Address.
        GU_QUEUE_ADDR = 0x0018,
        // Command List Address
        // [0 - 31] Command List Buffer Size, in words.
        GU_QUEUE_LEN =  0x001C,
    };


    enum GUIRQMask
    {
        GU_IRQ_MASK_QUEUE = 0x1,
    };

    enum GUControlBit
    {
        GU_RESTART_FLAG = 0x1,
    };

    enum GUID
    {
        GU_1 = 0,
    };

    enum GUQueueIndex
    {
        GU_QUEUE_INDEX0 = 0x0,
        GU_QUEUE_INDEX1 = 0x1,
        GU_QUEUE_INDEX2 = 0x2,
        GU_QUEUE_INDEX3 = 0x3,

        GU_QUEUE_INDEX_MAX = 0x4,
    };

    enum GUQueueControlBit
    {
        GU_QUEUE_PRIORITY_LOW =     0x0,
        GU_QUEUE_PRIORITY_NORMAL =  0x1,
        GU_QUEUE_PRIORITY_HIGH =    0x2,

        GU_QUEUE_START =            0x8000'0000,
    };

    enum GUTextureFormat
    {
        GU_FORMAT_RGBA8 = 0x0,
    };

    // GU Command Layout
    // [0 - 23] Command arguments.
    // [24 - 31] Command ID.
    enum GUCommand
    {
        GU_COMMAND_END = 0x0,

        // 0x01BBGGRR | RGB color in command arguments, 8 bits each component.
        // 0xYYYYXXXX | X, Y top left coordinates of the rectangle.
        // 0xHHHHWWWW | W, H size of the rectangle.
        GU_COMMAND_RECT = 0x1,

        // 0x02AAAAAA | Texture address in command arguments, aligned in 256 bytes.
        // 0xTFHHHWWW | W, H size of the texture, F for texture format,
        //              T is for the texture unit
        GU_COMMAND_TEXTURE_SET,
    };

    struct GURegisters
    {
        // 0x000
        Word irq_mask;
        Word irq_status;
        Word ctr;
        Word id;

        // 0x010
        Word queue_ctr;
        Word queue_state;
        Word queue_addr;
        Word queue_len;
    };

    static IO::IODevice get_io_device();
    static GURegisters& get_registers()
    {
        return *(GURegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IO::GU_BASE);
    }

    static void initialize();
    static void shutdown();

    static void dispatch();

    static void handle_write_word(VirtualAddress local_address, Word value);
};
