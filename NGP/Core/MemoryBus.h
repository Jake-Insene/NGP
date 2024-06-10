#pragma once
#include "Header.h"
#include <fstream>

struct MemoryBus {
    static u8* ram_address();
    static u8* bios_address();
    static u8* rom_address();

    static void emplace_bios(std::ifstream& bios_file, u32 bios_size);
    static void emplace_rom(std::ifstream& rom_file, u32 rom_size);

    static void* get_real_address(u32 addr);

    static bool read_word(u32 addr, u32& word);
    static bool read_half(u32 addr, u16& half);
    static bool read_byte(u32 addr, u8& byte);

    static bool write_word(u32 addr, u32 word);
    static bool write_half(u32 addr, u16 half);
    static bool write_byte(u32 addr, u8 byte);

};