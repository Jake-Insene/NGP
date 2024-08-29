/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/IO.h"
#include "IO/DMA.h"
#include "IO/Pad.h"
#include "Memory/Bus.h"
#include "Platform/OS.h"
#include <stdio.h>

namespace IO {

struct IORegisters {
    // 0x1000'000 - 0x1000'0100
    // 16 bytes each channel
    DMAChannelRegs dma_channels[16];

    // 0x1000'0100
    u32 dma_enable;
    // 0x1000'0104
    u32 dma_irq;
    // 0x1000'0108
    u32 dma_priority;
    // 0x1000'010C
    u32 dma_wait_on;

    // 0x1000'0110
    u32 irq_status;
    // 0x1000'0114
    u32 irq_enable_mask;
    // 0x1000'0118
    u32 padding_0;
    // 0x1000'011C
    u32 padding_1;

    // 0x1000'0120
    u32 gpu_status;
    // 0x1000'0124
    u32 gpu_irq;
    // 0x1000'0128
    u32 padding_2;
    // 0x1000'012C
    u32 padding_3;
};

u8* io_mem = nullptr;

void initialize() {
    io_mem = (u8*)OS::allocate_virtual_memory((void*)u64(Bus::IO_START), Bus::IO_SIZE);

#if !NDEBUG
    printf("DEBUG: IO mapped at 0x%p", io_mem);
#endif // !NDEBUG

    pad_reset();
}

void shutdown() {}

void* io_start_address() {
    return io_mem;
}

u8 read_io_byte(CPU::CPUCore* core, u32 io_address) {
    return io_mem[io_address];
}

u16 read_io_half(CPU::CPUCore* core, u32 io_address) {
    return *(u16*)u64(io_address);
}

Word read_io_word(CPU::CPUCore* core, u32 io_address) {
    return *(Word*)u64(io_address);
}

DWord read_io_dword(CPU::CPUCore* core, u32 io_address) {
    return *(DWord*)u64(io_address);
}

QWord read_io_qword(CPU::CPUCore* core, u32 io_address) {
    return *(QWord*)u64(io_address);
}

void write_io_byte(CPU::CPUCore* core, u32 io_address, u8 value) {
    // TODO: what could we do?
}

void write_io_half(CPU::CPUCore* core, u32 io_address, u16 value) {
    // TODO: what could we do?
}

void write_io_word(CPU::CPUCore* core, u32 address, Word value) {
    if (address < 0x1000'0100) {
        dma_channel_write((address >> 4) & 0xF, (address >> 2) & 0x3, value);
    }
    else if (address == DMA_ENABLE_MASK) {
        dma_set_enable(value);
    }
    else if (address == DMA_IRQ_MASK) {
        dma_set_irq(value);
    }
    else if (address == DMA_PRIORITY_MASK) {
        dma_set_priority(value);
    }
    else if (address == DMA_WAIT_ON_MASK) {
        dma_wait_on(value);
    }
    else if (address == DMA_IRQ_MASK) {

    }
    else if (address == DMA_ENABLE_MASK) {

    }
}

void write_io_dword(CPU::CPUCore* core, u32 io_address, DWord value) {
    // TODO: what could we do?
}

void write_io_qword(CPU::CPUCore* core, u32 io_address, QWord value) {
    // TODO: what could we do?
}


}
