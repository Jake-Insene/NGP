/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

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

u8* bios_start_address();
u8* ram_start_address();

void invalid_read(void* core_ptr, u32 address);
void invalid_write(void* core_ptr, u32 address);

bool load_bios(const char* path);

void read_pc(void* core_ptr);

QWord read_qword(void* core_ptr, u32 addr);
DWord read_dword(void* core_ptr, u32 addr);
u32 read_word(void* core_ptr, u32 addr);
u16 read_half(void* core_ptr, u32 addr);
u8 read_byte(void* core_ptr, u32 addr);

void write_qword(void* core_ptr, u32 addr, QWord qword);
void write_dword(void* core_ptr, u32 addr, DWord dword);
void write_word(void* core_ptr, u32 addr, u32 word);
void write_half(void* core_ptr, u32 addr, u16 half);
void write_byte(void* core_ptr, u32 addr, u8 byte);

};