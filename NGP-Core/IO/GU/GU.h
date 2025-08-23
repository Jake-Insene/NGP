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

struct GU
{

    enum Register
    {
        // GU Interrupt Mask
        // [0] Queue Execution End
        GU_IRQ_MASK =   0x0000,
        GU_IRQ_STATUS = 0x0004,

        // GU Control
        // [0] Reset
        GU_CTR =        0x0008,
        GU_ID =         0x000C,
    
        // Command Queue
        // [31] -> Start execution
        GU_QUEUE_CTR =      0x0010,
        // [0 - 31] Queue State.
        // Command Queue State
        GU_QUEUE_STATE =    0x0014,
        // Command List Address
        // [0 - 31] Command List Base Address.
        GU_QUEUE_ADDR = 0x0018,
        // Command List Address
        // [0 - 31] Command List Buffer Size, in words.
        GU_QUEUE_LEN =  0x001C,
    };


    enum IRQMask
    {
        IRQ_MASK_QUEUE = 0x1,
    };

    enum GUControlBit
    {
        RESET = 0x1,
    };

    enum GUID
    {
        GU_GU1 = 0,
    };

    enum QueueControlBit
    {
        QUEUE_START = 0x8000'0000,
    };

    enum QueueState
    {
        QUEUE_FREE = 0x0,
        QUEUE_BUSY = 0x1,
        QUEUE_ERROR_BAD_ADDRESS = 0x2,
        QUEUE_ERROR_BAD_LEN = 0x3,
    };

    enum TextureFormat
    {
        TEXTURE_FORMAT_RGBA8 = 0x0,
        TEXTURE_FORMAT_RGB8 = 0x1,
        TEXTURE_FORMAT_RGBA4 = 0x2,
        TEXTURE_FORMAT_RGB565 = 0x3,
        TEXTURE_FORMAT_RGBA5551 = 0x4,
    };

    // GU Command Layout
    // [0 - 23] Command arguments.
    // [24 - 31] Command ID.
    enum Command
    {
        CMD_END = 0x0,

        // 0x01AAAAAA | Draw Buffer Address in command arguments, aligned in 256 bytes
        // 0x0FHHHWWW | W, H size of the framebuffer, F for draw buffer texture format,
        // 0xYYYYXXXX | X, Y offset.
        CMD_DRAW_BUFFER = 0x1,
        
        // 0x02AAAAAA | Texture address in command arguments, aligned in 256 bytes.
        // 0xTFHHHWWW | W, H size of the texture, F for texture format,
        //              T is for the texture unit.
        CMD_TEXTURE_SET = 0x2,

        // 0x20BBGGRR | RGB color in command arguments, 8 bits each component.
        // 0xYYYYXXXX | X, Y top left coordinates of the rectangle.
        // 0xHHHHWWWW | W, H size of the rectangle.
        CMD_RECT = 0x20,

        // 0x21BBGGRR | RGB color in command arguments, 8 bits each component.
        // 0xYYYYXXXX | X, Y top left coordinates of the first vertex of the triangle.
        // 0xYYYYXXXX | X, Y top left coordinates of the second vertex of the triangle.
        // 0xYYYYXXXX | X, Y top left coordinates of the third vertex of the triangle.
        CMD_TRIANGLE = 0x21,
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
