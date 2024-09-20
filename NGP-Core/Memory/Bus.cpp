/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Memory/Bus.h"
#include "IO/IO.h"
#include "CPU/CPU.h"
#include "Platform/OS.h"
#include <fstream>

namespace Bus {

u8* ram = nullptr;
u8* bios = nullptr;

static constexpr u64 MAPPED_BIOS_ADDRESS = 0x1'0000'0000;

void initialize() {
    bios = (u8*)OS::allocate_virtual_memory((void*)MAPPED_BIOS_ADDRESS, BIOS_SIZE);
    ram = (u8*)OS::allocate_virtual_memory((void*)u64(RAM_START), RAM_SIZE);

#if !NDEBUG
    printf("DEBUG: BIOS mapped at 0x%p\n", bios);
    printf("DEBUG: RAM mapped at 0x%p\n", ram);
#endif // !NDEBUG
}

void shutdown() {
    // There is not reason to deallocate bios/ram, it just will make the exit slower
}

u8* bios_start_address() {
    return bios;
}

u8* ram_start_address() {
    return ram;
}

void invalid_read(CPU* core, VirtualAddress addr) {
    // TODO: Generate a exception
}

void invalid_write(CPU* core, VirtualAddress addr) {
    // TODO: Generate a exception
}

bool load_bios(const char* path) {
    std::ifstream file{ path, std::ios::binary | std::ios::ate };
    if (!file.is_open()) {
        return false;
    }

    u32 size = (u32)file.tellg();
    file.seekg(0);
    if (size != MB(4)) {
        return false;
    }

    file.read((char*)bios, size);

    file.close();

    return true;
}

u32 read_pc(CPU* core, u32 pc) {
    if (pc + 3 <= BIOS_END && core->current_el == 0) {
        return *reinterpret_cast<u32*>(pc + MAPPED_BIOS_ADDRESS);
    }
    else if (pc >= RAM_START && pc + 3 <= RAM_END) {
        return *(u32*)u64(pc);
    }
    else {
        invalid_read(core, pc);
    }

    return u32(-1);
}

template<typename T>
inline T read_at(CPU* core, VirtualAddress addr) {
    if (addr + (sizeof(T) + 1) <= BIOS_END && core->current_el == 0) {
        return *reinterpret_cast<T*>(addr + MAPPED_BIOS_ADDRESS);
    }
    else if (addr >= IO_START && addr + (sizeof(T) + 1) <= IO_START + IO_SIZE) {
        return IO::read_io<T>(core, addr);
    }
    else if (addr >= RAM_START && addr + (sizeof(T) + 1) <= RAM_END) {
        return *reinterpret_cast<T*>(u64(addr));
    }

    invalid_read(core, addr);

    return {};
}


QWord read_qword(CPU* core, VirtualAddress addr) {
    return read_at<QWord>(core, addr);
}

DWord read_dword(CPU* core, VirtualAddress addr) {
    return read_at<DWord>(core, addr);
}

u32 read_word(CPU* core, VirtualAddress addr) {
    return read_at<u32>(core, addr);
}

u16 read_half(CPU* core, VirtualAddress addr) {
    return read_at<u16>(core, addr);
}

u8 read_byte(CPU* core, VirtualAddress addr) {
    return read_at<u8>(core, addr);
}


template<typename T>
inline void write_at(CPU* core, VirtualAddress addr, T value) {
    if (addr + (sizeof(T) + 1) <= BIOS_END && core->current_el == 0) {
        *reinterpret_cast<T*>(addr + MAPPED_BIOS_ADDRESS) = value;
    }
    else if (addr >= IO_START && addr + (sizeof(T) + 1) <= IO_END) {
        IO::write_io<T>(core, addr, value);
    }
    else if (addr >= RAM_START && addr + (sizeof(T) + 1) <= RAM_END) {
        *reinterpret_cast<T*>(u64(addr)) = value;
    }
    else {
        invalid_write(core, addr);
    }
}

void write_qword(CPU* core, VirtualAddress addr, QWord qword) {
    write_at<QWord>(core, addr, qword);
}

void write_dword(CPU* core, VirtualAddress addr, DWord dword) {
    write_at<DWord>(core, addr, dword);
}

void write_word(CPU* core, VirtualAddress addr, u32 word) {
    write_at<u32>(core, addr, word);
}

void write_half(CPU* core, VirtualAddress addr, u16 half) {
    write_at<u16>(core, addr, half);
}

void write_byte(CPU* core, VirtualAddress addr, u8 byte) {
    write_at<u8>(core, addr, byte);
}

}
