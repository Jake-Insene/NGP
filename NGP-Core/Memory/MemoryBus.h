#pragma once
#include "Core/Header.h"

struct MemoryBus {
    static u8* biosAddress();
    static u8* ramAddress();
    static u8* romAddress();

    static void emplaceBios(u8* bios, u32 bios_size);
    static void emplaceRom(u8* rom, u32 rom_size);

    static void* getAddress(u32 addr, bool read);

    static bool readWord(u32 addr, u32& Word);
    static bool readHalf(u32 addr, u16& half);
    static bool readByte(u32 addr, u8& byte);

    static bool writeWord(u32 addr, u32 Word);
    static bool writeHalf(u32 addr, u16 half);
    static bool writeByte(u32 addr, u8 byte);

};