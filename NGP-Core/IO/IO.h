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

namespace IO {

static constexpr u32 IRQ_STATUS = 0x1000'0110;
static constexpr u32 IRQ_ENABLE_MASK = 0x1000'0114;

using IOFuncRead = Word(*)();
using IOFuncWrite = void(*)(Word);

void initialize();
void shutdown();

void* io_start_address();

u8 read_io_byte(CPUCore* core, u32 address);
u16 read_io_half(CPUCore* core, u32 address);
Word read_io_word(CPUCore* core, u32 address);
DWord read_io_dword(CPUCore* core, u32 address);
QWord read_io_qword(CPUCore* core, u32 address);

void write_io_byte(CPUCore* core, u32 address, u8 value);
void write_io_half(CPUCore* core, u32 address, u16 value);
void write_io_word(CPUCore* core, u32 address, Word value);
void write_io_dword(CPUCore* core, u32 address, DWord value);
void write_io_qword(CPUCore* core, u32 address, QWord value);

template<typename T>
T read_io(CPUCore* core, u32 address) {
    if constexpr (std::same_as<T, u8>) {
        return read_io_byte(core, address);
    }
    else if constexpr (std::same_as<T, u16>) {
        return read_io_half(core, address);
    }
    else if constexpr (std::same_as<T, Word>) {
        return read_io_word(core, address);
    }
    else if constexpr (std::same_as<T, DWord>) {
        return read_io_dword(core, address);
    }
    else if constexpr (std::same_as<T, QWord>) {
        return read_io_qword(core, address);
    }
}

template<typename T>
void write_io(CPUCore* core, u32 address, T value) {
    if constexpr (std::same_as<T, u8>) {
        write_io_byte(core, address, value);
    }
    else if constexpr (std::same_as<T, u16>) {
        write_io_half(core, address, value);
    }
    else if constexpr (std::same_as<T, Word>) {
        write_io_word(core, address, value);
    }
    else if constexpr (std::same_as<T, DWord>) {
        write_io_dword(core, address, value);
    }
    else if constexpr (std::same_as<T, QWord>) {
        write_io_qword(core, address, value);
    }
}

}
