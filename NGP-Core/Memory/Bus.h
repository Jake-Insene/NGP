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
// IO   100000000 - 00000FFF = 4 KB
// RAM	20000000 - 1FFFFFFF = 256 MB

static constexpr u32 BIOS_START = 0x0000'0000;
static constexpr u32 IO_START = 0x1000'0000;
static constexpr u32 RAM_START = 0x2000'0000;

static constexpr u32 BIOS_END = 0x003F'FFFF;
static constexpr u32 IO_END = 0x1000'0FFF;
static constexpr u32 RAM_END = 0x1FFF'FFFF;

static constexpr u32 BIOS_SIZE = MB(4);
static constexpr u32 IO_SIZE = KB(4);
static constexpr u32 RAM_SIZE = MB(256);

void initialize();
void shutdown();

void set_ram_size(u32 new_size);

u8* bios_start_address();
u8* ram_start_address();

void invalid_read(CPUCore* core, VirtualAddress addr);
void invalid_write(CPUCore* core, VirtualAddress addr);

bool load_bios(const char* path);

u32 read_pc(CPUCore* core, u32 pc);

QWord read_qword(CPUCore* core, VirtualAddress addr);
DWord read_dword(CPUCore* core, VirtualAddress addr);
u32 read_word(CPUCore* core, VirtualAddress addr);
u16 read_half(CPUCore* core, VirtualAddress addr);
u8 read_byte(CPUCore* core, VirtualAddress addr);

void write_qword(CPUCore* core, VirtualAddress addr, QWord qword);
void write_dword(CPUCore* core, VirtualAddress addr, DWord dword);
void write_word(CPUCore* core, VirtualAddress addr, u32 word);
void write_half(CPUCore* core, VirtualAddress addr, u16 half);
void write_byte(CPUCore* core, VirtualAddress addr, u8 byte);

};