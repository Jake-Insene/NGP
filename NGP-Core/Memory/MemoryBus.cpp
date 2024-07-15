#include "Memory/MemoryBus.h"
#include "Core/Constants.h"
#include <memory>

u8 BIOS[MB(4)] = {};
u8 RAM[MB(256)] = {};

u8* ROM = nullptr;
u32 rom_loaded_size = 0;

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

void MemoryBus::emplaceBios(u8* bios, u32 bios_size)
{
    std::copy(bios, bios + bios_size, BIOS);
}

void MemoryBus::emplaceRom(u8* rom, u32 rom_size)
{
    if (rom_loaded_size < rom_size) {
        if (ROM != nullptr) {
            delete ROM;
        }
        ROM = new u8[rom_size];
    }

    std::memcpy(ROM, rom, rom_size);
    rom_loaded_size = rom_size;
}

void* MemoryBus::getAddress(u32 addr, bool read)
{
    if (addr <= BIOSEnd && read) {
        return BIOS + (addr - BIOSStart);
    }
    else if (addr <= RAMEnd) {
        return RAM + (addr - RAMStart);
    }
    // Implement Rom read
    else if (addr <= (ROMStart + rom_loaded_size) && read) {
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
