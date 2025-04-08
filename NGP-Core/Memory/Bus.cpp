/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Memory/Bus.h"
#include "IO/IO.h"
#include "CPU/CPUCore.h"
#include "Platform/OS.h"
#include <fstream>

namespace Bus
{

u8* ram = nullptr;
u8* bios = nullptr;

// Default to 256 MB
u32 ram_size = MB(256);

static constexpr u64 MAPPED_BIOS_ADDRESS = 0x1'0000'0000;

void initialize()
{
    bios = (u8*)OS::allocate_virtual_memory((void*)MAPPED_BIOS_ADDRESS, BIOS_SIZE);
    ram = (u8*)OS::allocate_virtual_memory((void*)u64(RAM_START), ram_size);

#if !NDEBUG
    printf("DEBUG: BIOS mapped at 0x%p\n", bios);
    printf("DEBUG: RAM mapped at 0x%p\n", ram);
#endif // !NDEBUG
}

void shutdown()
{
    // There is not reason to deallocate bios/ram, it just will make the exit slower
}

void set_ram_size(u32 new_size)
{
    if (new_size >= MAX_ALLOWED_RAM_MB)
    {
        printf("error: invalid ram size %d MB, the maximum allowed is %d MB\n", new_size, MAX_ALLOWED_RAM_MB);
    }
    ram_size = new_size;
}

u32 get_ram_size()
{
    return ram_size;
}

u8* bios_start_address()
{
    return bios;
}

u8* ram_start_address()
{
    return ram;
}

void invalid_read(CPUCore& core, VirtualAddress addr)
{
    // TODO: Generate a exception
}

void invalid_write(CPUCore& core, VirtualAddress addr)
{
    // TODO: Generate a exception
    __debugbreak();
}

bool load_bios(const char* path)
{
    std::ifstream file{ path, std::ios::binary | std::ios::ate };
    if (!file.is_open())
    {
        return false;
    }

    u32 size = (u32)file.tellg();
    file.seekg(0);
    if (size != MB(4))
    {
        return false;
    }

    file.read((char*)bios, size);

    file.close();

    return true;
}

PhysicalAddress get_physical_addr(CPUCore& core, VirtualAddress addr)
{
    if (addr < BIOS_END && core.current_el == BIOS_REQUESTED_LEVEL)
    {
        return (u64(addr) + MAPPED_BIOS_ADDRESS);
    }
    else if (addr >= RAM_START && addr < get_ram_size())
    {
        return (u64)addr;
    }
    else
    {
        invalid_read(core, addr);
    }

    return 0;
}

template<typename T>
inline T read_at(CPUCore& core, VirtualAddress addr)
{
    if (addr + sizeof(T) <= BIOS_END)
    {
        return *reinterpret_cast<T*>(addr + MAPPED_BIOS_ADDRESS);
    }
    else if (addr >= IO_START && addr + sizeof(T) <= IO_END && core.current_el > 0)
    {
        return IO::read_io<T>(core, addr);
    }
    else if (addr >= RAM_START)
    {
        return *reinterpret_cast<T*>(PhysicalAddress(addr));
    }

    invalid_read(core, addr);

    return T();
}


QWord read_qword(CPUCore& core, VirtualAddress addr)
{
    return read_at<QWord>(core, addr);
}

DWord read_dword(CPUCore& core, VirtualAddress addr)
{
    return read_at<DWord>(core, addr);
}

u32 read_word(CPUCore& core, VirtualAddress addr)
{
    return read_at<u32>(core, addr);
}

u16 read_half(CPUCore& core, VirtualAddress addr)
{
    return read_at<u16>(core, addr);
}

u8 read_byte(CPUCore& core, VirtualAddress addr)
{
    return read_at<u8>(core, addr);
}


template<typename T>
inline void write_at(CPUCore& core, VirtualAddress addr, T value)
{
    if (addr + sizeof(T) <= BIOS_END)
    {
        *reinterpret_cast<T*>(addr + MAPPED_BIOS_ADDRESS) = value;
    }
    else if (addr >= IO_START && addr + sizeof(T) <= IO_END && core.current_el > 0)
    {
        IO::write_io<T>(core, addr, value);
    }
    else if (addr >= RAM_START)
    {
        *reinterpret_cast<T*>(PhysicalAddress(addr)) = value;
    }
    else
    {
        invalid_write(core, addr);
    }
}

void write_qword(CPUCore& core, VirtualAddress addr, QWord qword)
{
    write_at<QWord>(core, addr, qword);
}

void write_dword(CPUCore& core, VirtualAddress addr, DWord dword)
{
    write_at<DWord>(core, addr, dword);
}

void write_word(CPUCore& core, VirtualAddress addr, u32 word)
{
    write_at<u32>(core, addr, word);
}

void write_half(CPUCore& core, VirtualAddress addr, u16 half)
{
    write_at<u16>(core, addr, half);
}

void write_byte(CPUCore& core, VirtualAddress addr, u8 byte)
{
    write_at<u8>(core, addr, byte);
}

}
