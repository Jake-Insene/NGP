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
static constexpr VirtualAddress IO_BASE = 0x1000'0000;

static constexpr VirtualAddress IRQ_BASE =      IO_BASE | 0x0000'0000;
static constexpr VirtualAddress DMA_BASE =      IO_BASE | 0x0000'1000;
static constexpr VirtualAddress PAD_BASE =      IO_BASE | 0x0000'2000;
static constexpr VirtualAddress USI_BASE =      IO_BASE | 0x0000'3000;
static constexpr VirtualAddress DISPLAY_BASE =  IO_BASE | 0x0000'4000;

static constexpr VirtualAddress GU_BASE =       IO_BASE | 0x0001'0000;

static constexpr Word SegmentSize = 0x1000;
static constexpr Word SegmentBits = 12;
static constexpr Word SegmentMask = SegmentSize - 1;

enum IOSegments
{
    IRQ_SEGMENT = 0x0,
    DMA_SEGMENT = 0x1,
    PAD_SEGMENT = 0x2,
    USI_SEGMENT = 0x3,
    DISPLAY_SEGMENT = 0x4,

    GU_SEGMENT = 0x10,

    LAST_SEGMENT,
};

struct IODevice
{
    VirtualAddress base_address;

    void(*initialize)();
    void(*shutdown)();
    void(*dispatch)();

    u8(*read_byte)(VirtualAddress);
    u16(*read_half)(VirtualAddress);
    Word(*read_word)(VirtualAddress);
    DWord(*read_dword)(VirtualAddress);
    QWord(*read_qword)(VirtualAddress);
    void(*write_byte)(VirtualAddress, u8);
    void(*write_half)(VirtualAddress, u16);
    void(*write_word)(VirtualAddress, Word);
    void(*write_dword)(VirtualAddress, DWord);
    void(*write_qword)(VirtualAddress, QWord);
};

void initialize();
void shutdown();

void dispatch();

u8 read_io_byte(VirtualAddress address);
u16 read_io_half(VirtualAddress address);
Word read_io_word(VirtualAddress address);
DWord read_io_dword(VirtualAddress address);
QWord read_io_qword(VirtualAddress address);

void write_io_byte(VirtualAddress address, u8 value);
void write_io_half(VirtualAddress address, u16 value);
void write_io_word(VirtualAddress address, Word value);
void write_io_dword(VirtualAddress address, DWord value);
void write_io_qword(VirtualAddress address, QWord value);

template<typename T>
T read_io(u32 address)
{
    if constexpr (std::same_as<T, u8>)
    {
        return read_io_byte(address);
    }
    else if constexpr (std::same_as<T, u16>)
    {
        return read_io_half(address);
    }
    else if constexpr (std::same_as<T, Word>)
    {
        return read_io_word(address);
    }
    else if constexpr (std::same_as<T, DWord>)
    {
        return read_io_dword(address);
    }
    else if constexpr (std::same_as<T, QWord>)
    {
        return read_io_qword(address);
    }
}

template<typename T>
void write_io(u32 address, T value)
{
    if constexpr (std::same_as<T, u8>)
    {
        write_io_byte(address, value);
    }
    else if constexpr (std::same_as<T, u16>)
    {
        write_io_half(address, value);
    }
    else if constexpr (std::same_as<T, Word>)
    {
        write_io_word(address, value);
    }
    else if constexpr (std::same_as<T, DWord>)
    {
        write_io_dword(address, value);
    }
    else if constexpr (std::same_as<T, QWord>)
    {
        write_io_qword(address, value);
    }
}

}
