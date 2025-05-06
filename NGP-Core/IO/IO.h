/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"
#include <concepts>

struct CPUCore;

namespace IO
{

// The MMIO is segmented, each segment has a size of 4096 bytes.
static constexpr u32 IO_BASE = 0x1000'0000;

enum IOSegments
{
    DMA_SEGMENT = 0x0,
    IRQ_SEGMENT = 0x1,
    PAD_SEGMENT = 0x2,

    GPU_SEGMENT = 0x10,
};

static constexpr u32 DMA_BASE = IO_BASE | 0x0000;
static constexpr u32 IRQ_BASE = IO_BASE | 0x1000;
static constexpr u32 PAD_BASE = IO_BASE | 0x2000;
static constexpr u32 GPU_BASE = IO_BASE | 0x10000;


void initialize();
void shutdown();

u8 read_io_byte(CPUCore& core, VirtualAddress address);
u16 read_io_half(CPUCore& core, VirtualAddress address);
Word read_io_word(CPUCore& core, VirtualAddress address);
DWord read_io_dword(CPUCore& core, VirtualAddress address);
QWord read_io_qword(CPUCore& core, VirtualAddress address);

void write_io_byte(CPUCore& core, VirtualAddress address, u8 value);
void write_io_half(CPUCore& core, VirtualAddress address, u16 value);
void write_io_word(CPUCore& core, VirtualAddress address, Word value);
void write_io_dword(CPUCore& core, VirtualAddress address, DWord value);
void write_io_qword(CPUCore& core, VirtualAddress address, QWord value);

template<typename T>
T read_io(CPUCore& core, u32 address)
{
    if constexpr (std::same_as<T, u8>)
    {
        return read_io_byte(core, address);
    }
    else if constexpr (std::same_as<T, u16>)
    {
        return read_io_half(core, address);
    }
    else if constexpr (std::same_as<T, Word>)
    {
        return read_io_word(core, address);
    }
    else if constexpr (std::same_as<T, DWord>)
    {
        return read_io_dword(core, address);
    }
    else if constexpr (std::same_as<T, QWord>)
    {
        return read_io_qword(core, address);
    }
}

template<typename T>
void write_io(CPUCore& core, u32 address, T value)
{
    if constexpr (std::same_as<T, u8>)
    {
        write_io_byte(core, address, value);
    }
    else if constexpr (std::same_as<T, u16>)
    {
        write_io_half(core, address, value);
    }
    else if constexpr (std::same_as<T, Word>)
    {
        write_io_word(core, address, value);
    }
    else if constexpr (std::same_as<T, DWord>)
    {
        write_io_dword(core, address, value);
    }
    else if constexpr (std::same_as<T, QWord>)
    {
        write_io_qword(core, address, value);
    }
}

}
