/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

struct CPUCore;

namespace Bus {

// BIOS	00000000 - 003FFFFF = 4 MB
// IO   100000000 - 1FFFFFFF = N MB
// RAM	20000000 - FFFFFFFF = N MB
// VRAM is in its own address space = N MB

static constexpr u32 BIOS_START = 0x0000'0000;
static constexpr u32 BIOS_END = 0x003F'FFFF;

static constexpr u32 IO_START = 0x1000'0000;
static constexpr u32 IO_END = 0x1FFF'FFFF;

static constexpr u32 RAM_START = 0x2000'0000;

static constexpr u32 BIOS_SIZE = MB(4U);

static constexpr u32 MAX_ALLOWED_RAM = 0x1'0000'0000 - 0x2000'0000;
static constexpr u32 MAX_ALLOWED_RAM_MB = MAX_ALLOWED_RAM / 1'024 / 1'024;

static constexpr u64 MAPPED_BUS_ADDRESS_START = 0x1'0000'0000;

enum CheckAddressResult
{
    ValidVirtualAddress = 0,
    InvalidVirtualAddress,
};

enum CheckAddressFlags
{
    WriteableAddress = 0x1,
};

void initialize();
void shutdown();

void set_ram_size(u32 new_size);
u32 get_ram_size();

void set_vram_size(usize new_size);
usize get_vram_size();

u8* bios_start_address();
u8* ram_start_address();
u8* io_start_address();

void invalid_read(CPUCore& core, VirtualAddress addr);
void invalid_write(CPUCore& core, VirtualAddress addr);

bool load_bios(const char* path);

CheckAddressResult check_virtual_address(VirtualAddress va, CheckAddressFlags flags);


FORCE_INLINE PhysicalAddress get_physical_addr(CPUCore& core, VirtualAddress pc)
{
    return u64(pc) + MAPPED_BUS_ADDRESS_START;
}

template<typename T>
FORCE_INLINE T read_at(CPUCore& core, VirtualAddress addr)
{
    return *(T*)(MAPPED_BUS_ADDRESS_START + addr);
}

FORCE_INLINE QWord read_qword(CPUCore& core, VirtualAddress addr)
{
    return read_at<QWord>(core, addr);
}

FORCE_INLINE DWord read_dword(CPUCore& core, VirtualAddress addr)
{
    return read_at<DWord>(core, addr);
}

FORCE_INLINE u32 read_word(CPUCore& core, VirtualAddress addr)
{
    return read_at<u32>(core, addr);
}

FORCE_INLINE u16 read_half(CPUCore& core, VirtualAddress addr)
{
    return read_at<u16>(core, addr);
}

FORCE_INLINE u8 read_byte(CPUCore& core, VirtualAddress addr)
{
    return read_at<u8>(core, addr);
}

void write_qword(CPUCore& core, VirtualAddress addr, QWord qword);
void write_dword(CPUCore& core, VirtualAddress addr, DWord dword);
void write_word(CPUCore& core, VirtualAddress addr, u32 word);
void write_half(CPUCore& core, VirtualAddress addr, u16 half);
void write_byte(CPUCore& core, VirtualAddress addr, u8 byte);

};