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

void invalid_read(void* core_ptr, u32 address) {
    // TODO: Generate a exception
}

void invalid_write(void* core_ptr, u32 address) {
    // TODO: Generate a exception
}

bool load_bios(const char* path) {
    std::ifstream file{ path, std::ios::binary | std::ios::ate };
    if (!file.is_open()) {
        return false;
    }

    u32 size = (u32)file.tellg();
    file.seekg(0);
    if (size > MB(4)) {
        return false;
    }

    file.read((char*)bios, size);

    file.close();

    return true;
}

void read_pc(void* core_ptr) {
    CPU::CPUCore* core = reinterpret_cast<CPU::CPUCore*>(core_ptr);
    u32 addr = core->pc;

    if (addr + 3 <= BIOS_END && core->current_el == 0) {
        core->ir = *reinterpret_cast<u32*>(addr + MAPPED_BIOS_ADDRESS);
    }
    else if (addr >= RAM_START && addr + 3 <= RAM_END) {
        core->ir = *(u32*)u64(addr);
    }
}

template<typename T>
inline T read_at(CPU::CPUCore* core, u32 addr) {
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


QWord read_qword(void* core_ptr, u32 addr) {
    return read_at<QWord>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr);
}

DWord read_dword(void* core_ptr, u32 addr) {
    return read_at<DWord>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr);
}

u32 read_word(void* core_ptr, u32 addr) {
    return read_at<u32>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr);
}

u16 read_half(void* core_ptr, u32 addr) {
    return read_at<u16>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr);
}

u8 read_byte(void* core_ptr, u32 addr) {
    return read_at<u8>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr);
}


template<typename T>
inline void write_at(CPU::CPUCore* core, u32 addr, T value) {
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

void write_qword(void* core_ptr, u32 addr, QWord qword) {
    write_at<QWord>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr, qword);
}

void write_dword(void* core_ptr, u32 addr, DWord dword) {
    write_at<DWord>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr, dword);
}

void write_word(void* core_ptr, u32 addr, u32 word) {
    write_at<u32>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr, word);
}

void write_half(void* core_ptr, u32 addr, u16 half) {
    write_at<u16>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr, half);
}

void write_byte(void* core_ptr, u32 addr, u8 byte) {
    write_at<u8>(reinterpret_cast<CPU::CPUCore*>(core_ptr), addr, byte);
}

}
