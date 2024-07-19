// --------------------
// MemoryBus.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "Memory/MemoryBus.h"
#include "Core/Constants.h"
#include "FileFormat/Rom.h"
#include "CPU/CPU.h"
#include <fstream>
#include <memory>

u8 BIOS[MB(4)] = {};
u8 RAM[MB(256)] = {};

u8* ROM = nullptr;
u32 rom_loaded_size = 0;

RomHeader header = {};

u8* MemoryBus::biosAddress()
{
    return BIOS;
}

u8* MemoryBus::ramAddress()
{
    return RAM;
}

u8* MemoryBus::romAddress()
{
    return ROM;
}

bool MemoryBus::loadBios(const char* path) {
    std::ifstream file{ path, std::ios::binary | std::ios::ate };
    if (!file.is_open()) {
        return false;
    }

    u32 size = (u32)file.tellg();
    file.seekg(0);
    if (size >= MB(4)) {
        return false;
    }

    file.read((char*)BIOS, size);

    file.close();
    return true;
}

bool MemoryBus::loadRom(const char* path) {
    std::ifstream file{ path, std::ios::binary | std::ios::ate };
    if (!file.is_open()) {
        return false;
    }

    u32 size = (u32)file.tellg();
    file.seekg(0);
    if (size < sizeof(RomHeader)) {
        file.close();
        return false;
    }

    file.read((char*)&header, sizeof(RomHeader));

    u32 rom_size = header.check_sum;
    if (rom_size != size - sizeof(RomHeader) || header.magic != RomSignature) {
        file.close();
        return false;
    }

    ROM = new u8[rom_size];
    file.read((char*)ROM, rom_size);
    rom_loaded_size = rom_size;

    file.close();

    CPU::registers.pc = header.target_address;
    return true;
}

void* MemoryBus::getAddress(u32 addr, bool read)
{
    if (addr <= BIOSEnd && read) {
        return BIOS + (addr - BIOSStart);
    }
    else if (addr <= RAMEnd) {
        return RAM + (addr - RAMStart);
    }
    else if (addr < (ROMStart + rom_loaded_size) && read) {
        u32 rom_addr = (addr - ROMStart);
        return ROM + rom_addr;
    }

    return nullptr;
}

bool MemoryBus::readWord(u32 addr, u32& Word)
{
    u32* real_addr = (u32*)getAddress(addr, true);

    if (real_addr != nullptr) {
        Word = *real_addr;
        return true;
    }

    return false;
}

bool MemoryBus::readHalf(u32 addr, u16& half)
{
    u16* real_addr = (u16*)getAddress(addr, true);

    if (real_addr != nullptr) {
        half = *real_addr;
        return true;
    }

    return false;
}

bool MemoryBus::readByte(u32 addr, u8& byte)
{
    u8* real_addr = (u8*)getAddress(addr, true);

    if (real_addr != nullptr) {
        byte = *real_addr;
        return true;
    }

    return false;
}

bool MemoryBus::writeWord(u32 addr, u32 Word)
{
    u32* real_addr = (u32*)getAddress(addr, false);

    if (real_addr != nullptr) {
        *real_addr = Word;
        return true;
    }

    return false;
}

bool MemoryBus::writeHalf(u32 addr, u16 half)
{
    u16* real_addr = (u16*)getAddress(addr, false);

    if (real_addr != nullptr) {
        *real_addr = half;
        return true;
    }

    return false;
}

bool MemoryBus::writeByte(u32 addr, u8 byte)
{
    u8* real_addr = (u8*)getAddress(addr, false);

    if (real_addr != nullptr) {
        *real_addr = byte;
        return true;
    }

    return false;
}
