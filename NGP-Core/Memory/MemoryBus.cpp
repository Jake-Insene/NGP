/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Memory/MemoryBus.h"
#include "Core/Constants.h"
#include "FileFormat/Rom.h"
#include "CPU/CPU.h"
#include <fstream>

u8 BIOS[MB(4)] = {};
u8 RAM[MB(256)] = {};

u8* ROM = nullptr;
u32 rom_loaded_size = 0;

RomHeader header = {};

u8* MemoryBus::bios_start_address() {
    return BIOS;
}

u8* MemoryBus::ram_start_address() {
    return RAM;
}

u8* MemoryBus::rom_address() {
    return ROM;
}

bool MemoryBus::load_bios(const char* path) {
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

bool MemoryBus::load_rom(const char* path) {
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

void* MemoryBus::get_address(u32 addr, bool read) {
    // memory fetching
    CPU::registers.cycle_counter++;

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

QWord MemoryBus::read_qword(u32 addr) {
    QWord* real_addr = (QWord*)get_address(addr, true);

    if (real_addr != nullptr) {
        return *real_addr;
    }

    // make a exception
    return {};
}

u64 MemoryBus::read_dword(u32 addr) {
    u64* real_addr = (u64*)get_address(addr, true);

    if (real_addr != nullptr) {
        return *real_addr;
    }

    // make a exception
    return 0;
}

u32 MemoryBus::read_word(u32 addr) {
    u32* real_addr = (u32*)get_address(addr, true);

    if (real_addr != nullptr) {
        return *real_addr;
    }

    // make a exception
    return 0;
}

u16 MemoryBus::read_half(u32 addr) {
    u16* real_addr = (u16*)get_address(addr, true);

    if (real_addr != nullptr) {
        return *real_addr;
    }

    // make a exception
    return 0;
}

u8 MemoryBus::read_byte(u32 addr) {
    u8* real_addr = (u8*)get_address(addr, true);

    if (real_addr != nullptr) {
        return *real_addr;
    }

    // make a exception
    return 0;
}

void MemoryBus::write_qword(u32 addr, QWord qword) {
    QWord* real_addr = (QWord*)get_address(addr, true);

    if (real_addr != nullptr) {
        *real_addr = qword;
    }
}

void MemoryBus::write_dword(u32 addr, u64 dword) {
    u64* real_addr = (u64*)get_address(addr, true);

    if (real_addr != nullptr) {
        *real_addr = dword;
    }
}

void MemoryBus::write_word(u32 addr, u32 word) {
    u32* real_addr = (u32*)get_address(addr, false);

    if (real_addr != nullptr) {
        *real_addr = word;
    }
}

void MemoryBus::write_half(u32 addr, u16 half) {
    u16* real_addr = (u16*)get_address(addr, false);

    if (real_addr != nullptr) {
        *real_addr = half;
    }
}

void MemoryBus::write_byte(u32 addr, u8 byte) {
    u8* real_addr = (u8*)get_address(addr, false);

    if (real_addr != nullptr) {
        *real_addr = byte;
    }
}
