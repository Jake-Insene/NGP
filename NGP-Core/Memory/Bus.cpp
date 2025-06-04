#include "Bus.h"
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

static inline u8* ram = nullptr;
static inline u8* bios = nullptr;
static inline u8* io = nullptr;

// Default to 256 MB
usize ram_size = MB(256);
// Default to 32 MB
usize vram_size = MB(32);

void initialize()
{
    bios = (u8*)OS::allocate_virtual_memory((void*)MAPPED_BUS_ADDRESS_START, BIOS_SIZE, OS::PAGE_READ_WRITE);

    io = (u8*)OS::allocate_virtual_memory((void*)(MAPPED_BUS_ADDRESS_START + IO_START), RAM_START - IO_START, OS::PAGE_READ_WRITE);

    ram = (u8*)OS::allocate_virtual_memory((void*)(MAPPED_BUS_ADDRESS_START + RAM_START), ram_size, OS::PAGE_READ_WRITE);

#if !NDEBUG
    printf("DEBUG: BIOS mapped at: 0x%p\n", bios);
    printf("DEBUG: IO mapped at:   0x%p\n", io);
    printf("DEBUG: RAM mapped at:  0x%p\n", ram);
#endif // !NDEBUG
}

void shutdown()
{
    OS::deallocate_virtual_memory(bios);
    OS::deallocate_virtual_memory(ram);
}

void set_ram_size(u32 new_size)
{
    if (new_size >= MAX_ALLOWED_RAM_MB)
    {
        printf("error: invalid ram size %d MB, the maximum allowed is %d MB\n", new_size, MAX_ALLOWED_RAM_MB);
    }
    ram_size = MB(new_size);
}

u32 get_ram_size()
{
    return (u32)ram_size;
}

void set_vram_size(usize new_size)
{
    vram_size = new_size;
}

usize get_vram_size()
{
    return vram_size;
}

u8* bios_start_address()
{
    return bios;
}

u8* ram_start_address()
{
    return ram;
}

u8* io_start_address()
{
    return io;
}

void invalid_read(CPUCore& core, VirtualAddress addr)
{
    // TODO: Generate a exception
    core.make_exception(CPUCore::InvalidRead, 0, 0);
}

void invalid_write(CPUCore& core, VirtualAddress addr)
{
    // TODO: Generate a exception
    core.make_exception(CPUCore::InvalidWrite, 0, 0);
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
    if (size != BIOS_SIZE)
    {
        return false;
    }

    file.read((char*)bios, size);

    file.close();

    return true;
}

CheckAddressResult check_virtual_address(VirtualAddress va, CheckAddressFlags flags)
{
    if (va > BIOS_END && va < IO_START)
    {
        return InvalidVirtualAddress;
    }
    else if (flags & WriteableAddress && va >= IO_START && va < RAM_START)
    {
        return InvalidVirtualAddress;
    }
    else if (va >= RAM_START && va < RAM_START + ram_size)
    {
        return ValidVirtualAddress;
    }

    return InvalidVirtualAddress;
}


template<typename T>
FORCE_INLINE static T read_at(CPUCore& core, VirtualAddress addr)
{
    return *(T*)(MAPPED_BUS_ADDRESS_START + addr);
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

i16 read_ihalf(CPUCore& core, VirtualAddress addr)
{
    return read_at<i16>(core, addr);
}

i8 read_ibyte(CPUCore& core, VirtualAddress addr)
{
    return read_at<i8>(core, addr);
}

template<typename T>
inline void write_at(CPUCore& core, VirtualAddress addr, T value)
{
    if ((addr >> 28) == 1)
    {
        return IO::write_io<T>(core, addr, value);
    }

    *(T*)(MAPPED_BUS_ADDRESS_START + addr) = value;
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
