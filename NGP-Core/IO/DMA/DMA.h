/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "IO/IO.h"
#include "Memory/Bus.h"

#include <mutex>

struct CPUCore;

struct DMA
{
    enum Register
    {
        DMA_CHANNELS_START = 0x000,
        DMA_CHANNELS_END = 0x0FF,

        // DMA Channel Mask
        // [0] RAM
        // [1] EMD
        // [2] SPU
        // [3] GU
        DMA_IRQ_MASK = 0x100,
        DMA_IRQ_STATUS = 0x104,
        DMA_WAIT_ON_MASK = 0x108,
    };

    // DMA Channel Control Register
    // [0] IRQ Enable
    // [1] Start / Busy
    // [2 - 3] Priority
    // [4 - 5] Transfer direction:
    //         0(RAM -> DEVICE), 1(DEVICE -> RAM), 2(DEVICE -> DEVICE)
    // [5 - 7] Transfer Step:
    //         0(4 bytes/1 Word), 1(2 bytes/1 Half), 2(1 byte), 3(8 bytes/1 DWord),
    //         4(16 bytes/1 QWord)
    //
    // [31] Fill Mode: 0(Normal fill mode, same as a copy from dest -> src),
    //                 1(Fill dest memory range with the value in the src register)

    enum DMAChannel
    {
        DMA_CHANNEL_RAM = 0,
        DMA_CHANNEL_EMD = 1,
        DMA_CHANNEL_SPU = 2,
        DMA_CHANNEL_GU = 3,

        DMA_CHANNELS_MAX = 16,
    };

    enum DMAIRQMask
    {
        DMA_IRQ_MASK_RAM = 0x1,
        DMA_IRQ_MASK_EMD = 0x2,
        DMA_IRQ_MASK_SPU = 0x4,
        DMA_IRQ_MASK_GU = 0x4,
    };

    enum DMAPriority
    {
        DMA_PRIORITY_LOW = 0x0,
        DMA_PRIORITY_NORMAL = 0x1,
        DMA_PRIORITY_HIGH = 0x2,
    };

    enum DMADirection
    {
        // Ignored in DMA RAM Channel
        DMA_RAM_TO_DEVICE = 0x0,
        DMA_DEVICE_TO_RAM = 0x1,
        DMA_DEVICE_TO_DEVICE = 0x2,
    };

    enum DMAStep
    {
        DMA_WORD = 0x0,
        DMA_HALF = 0x1,
        DMA_BYTE = 0x2,
        DMA_DWORD = 0x3,
        DMA_QWORD = 0x4,
    };

    enum DMAControlChannelBit
    {
        DMA_IRQ = 0x1,
        DMA_START = 0x2,
        DMA_BUSY = 0x2,

        DMA_FILL_MODE_U32 = 0x8000'0000,
    };

    union DMAChannelInfo
    {
        struct
        {
            Word ctr;
            Word dst;
            Word src;
            Word cnt;
        };

        Word raw_regs[4];
    };

    struct DMARegisters
    {
        DMAChannelInfo channels[16];

        Word irq_mask;
        Word irq_status;
        Word wait_on_mask;
    };

    static inline std::mutex dma_mutex;

    static IO::IODevice get_io_device();
    static DMARegisters& get_registers()
    {
        return *(DMARegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IO::DMA_BASE);
    }

    static void initialize();
    static void shutdown();

    static void dispatch();

    static void handle_write_word(VirtualAddress local_address, Word value);

};
