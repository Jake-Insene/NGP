/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

struct MemoryBus {
    static u8* bios_start_address();
    static u8* ram_start_address();
    static u8* rom_address();

    static bool load_bios(const char* path);
    static bool load_rom(const char* path);

    static void* get_address(u32 addr, bool read);

    static QWord read_qword(u32 addr);
    static u64 read_dword(u32 addr);
    static u32 read_word(u32 addr);
    static u16 read_half(u32 addr);
    static u8 read_byte(u32 addr);

    static void write_qword(u32 addr, QWord qword);
    static void write_dword(u32 addr, u64 dword);
    static void write_word(u32 addr, u32 word);
    static void write_half(u32 addr, u16 half);
    static void write_byte(u32 addr, u8 byte);

};