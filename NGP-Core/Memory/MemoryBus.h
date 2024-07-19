// --------------------
// MemoryBus.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "Core/Header.h"

struct MemoryBus {
    static u8* biosAddress();
    static u8* ramAddress();
    static u8* romAddress();

    static bool loadBios(const char* path);
    static bool loadRom(const char* path);

    static void* getAddress(u32 addr, bool read);

    static bool readWord(u32 addr, u32& Word);
    static bool readHalf(u32 addr, u16& half);
    static bool readByte(u32 addr, u8& byte);

    static bool writeWord(u32 addr, u32 Word);
    static bool writeHalf(u32 addr, u16 half);
    static bool writeByte(u32 addr, u8 byte);

};