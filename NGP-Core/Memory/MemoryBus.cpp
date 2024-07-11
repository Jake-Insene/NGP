#include "Memory/MemoryBus.h"
#include "Core/Constants.h"
#include <memory>

u8 RAM[MB(128)] = {};
u8 BIOS[MB(2)] = {};

u8* ROM = nullptr;
u32 rom_current_size = 0;

u8* MemoryBus::ram_address()
{
    return RAM;
}

u8* MemoryBus::bios_address()
{
    return BIOS;
}

u8* MemoryBus::rom_address()
{
    return ROM;
}

void MemoryBus::emplace_bios(u8* bios, u32 bios_size)
{
    std::copy(bios, bios + bios_size, BIOS);
}

void MemoryBus::emplace_rom(u8* rom, u32 rom_size)
{
    if (rom_current_size < rom_size) {
        if (ROM == nullptr) {
            delete ROM;
        }
        ROM = new u8[rom_size];
    }

    std::copy(rom, rom + rom_size, ROM);
    rom_current_size = rom_size;
}

void* MemoryBus::get_real_address(u32 addr, bool read)
{
    if (addr <= RAMEnd) {
        return RAM + (addr - RAMStart);
    }
    else if (addr <= BIOSEnd && read) {
        return BIOS + (addr - BIOSStart);
    }
    else if (addr <= ROMEnd && read) {
        u32 rom_addr = (addr - ROMStart);
        if (rom_addr < rom_current_size) {
            return ROM + rom_addr;
        }
    }

    return nullptr;
}

bool MemoryBus::read_word(u32 addr, u32& Word)
{
    u32* real_addr = (u32*)get_real_address(addr, true);

    if (real_addr != nullptr) {
        Word = *real_addr;
        return true;
    }

    return false;
}

bool MemoryBus::read_half(u32 addr, u16& half)
{
    u16* real_addr = (u16*)get_real_address(addr, true);

    if (real_addr != nullptr) {
        half = *real_addr;
        return true;
    }

    return false;
}

bool MemoryBus::read_byte(u32 addr, u8& byte)
{
    u8* real_addr = (u8*)get_real_address(addr, true);

    if (real_addr != nullptr) {
        byte = *real_addr;
        return true;
    }

    return false;
}

bool MemoryBus::write_word(u32 addr, u32 Word)
{
    u32* real_addr = (u32*)get_real_address(addr, false);

    if (real_addr != nullptr) {
        *real_addr = Word;
        return true;
    }

    return false;
}

bool MemoryBus::write_half(u32 addr, u16 half)
{
    u16* real_addr = (u16*)get_real_address(addr, false);

    if (real_addr != nullptr) {
        *real_addr = half;
        return true;
    }

    return false;
}

bool MemoryBus::write_byte(u32 addr, u8 byte)
{
    u8* real_addr = (u8*)get_real_address(addr, false);

    if (real_addr != nullptr) {
        *real_addr = byte;
        return true;
    }

    return false;
}
