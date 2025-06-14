/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Memory/Bus.h"

#include "IO/IO.h"
#include "CPU/CPUCore.h"
#include "Emulator.h"
#include "Platform/OS.h"
#include <fstream>
#include <vector>

extern thread_local Emulator::ThreadCore* local_core;

namespace Bus
{

enum PageAccess
{
    None = 0x0,
    Read = 0x1,
    Write = 0x2,
    Execute = 0x4,
};

struct Page
{
    PhysicalAddress physical_address;
    VirtualAddress page_address;
	// 2^22 - 1 == 1'048'575 page indexes, this is enough for 4 GB of address space
    Word page_index : 20;
    Word access : 12;
};

static inline std::vector<Page> page_table;

// Default to 4 KB
static constexpr Word DefaultPageSize = KB(4);

static constexpr Word MaxPageCount4K = 0x1'0000'0000 >> bits_of(DefaultPageSize - 1);

static inline Word page_size = DefaultPageSize;
static inline Word page_mask = DefaultPageSize - 1;
static inline Word page_bits = bits_of(DefaultPageSize - 1);

static inline u8* bios = nullptr;
static inline u8* io = nullptr;
static inline u8* ram = nullptr;

// Default to 256 MB
static inline u32 ram_size = MB(256);
// Default to 32 MB
static inline u32 vram_size = MB(32);

void initialize()
{
    // VRAM is managed by the GU
    Word page_count = 0x1'0000'0000 >> page_bits;
    page_table.resize(page_count);

    bios = (u8*)OS::allocate_virtual_memory((void*)MAPPED_BUS_ADDRESS_START, BIOS_SIZE, OS::PAGE_READ_WRITE);

    io = (u8*)OS::allocate_virtual_memory((void*)(MAPPED_BUS_ADDRESS_START + IO_START), RAM_START - IO_START, OS::PAGE_READ_WRITE);

    ram = (u8*)OS::allocate_virtual_memory((void*)(MAPPED_BUS_ADDRESS_START + RAM_START), usize(ram_size), OS::PAGE_READ_WRITE);

    // By default every bios, ram and io page is accessible
    for(usize i = 0; i < page_count; i++)
    {
        VirtualAddress page_address = i << page_bits;
        page_table[i].page_index = i;

        page_table[i].physical_address = PhysicalAddress(MAPPED_BUS_ADDRESS_START + page_address);
        page_table[i].page_address = page_address;
        if(page_address < BIOS_END)
        {
            page_table[i].access = Read | Write | Execute;
        }
        else if(page_address >= IO_START && page_address < IO_END)
        {
            page_table[i].access = Read | Write;
        }
        else if(page_address >= RAM_START && page_address < RAM_START + ram_size)
        {
            page_table[i].access = Read | Write | Execute;
        }
        else
        {
            page_table[i].access = None;
        }
    }

#if !NDEBUG
    printf("DEBUG: BIOS mapped at: 0x%016llX\n", u64(bios));
    printf("DEBUG: IO mapped at:   0x%016llX\n", u64(io));
    printf(
        "DEBUG: RAM mapped at:  0x%016llX\n"
        "       RAM size: 0x%08X\n",
        u64(ram), ram_size
    );
#endif // !NDEBUG
}

void shutdown()
{
    OS::deallocate_virtual_memory(bios);
    OS::deallocate_virtual_memory(io);
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

void set_page_size(Word new_page_size)
{
    page_size = new_page_size;
    page_mask = new_page_size - 1;
    page_bits = bits_of(page_mask);
}

Word get_page_size()
{
    return page_size;
}

void set_vram_size(usize new_size)
{
    vram_size = MB(new_size);
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

void invalid_read(VirtualAddress addr)
{
    local_core->core->handle_exception(CPUCore::InvalidRead, addr);
}

void invalid_write(VirtualAddress addr)
{
    local_core->core->handle_exception(CPUCore::InvalidWrite, addr);
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
    VirtualAddress page_index = va >> page_bits;
    Page& page = page_table[page_index];
    if(flags & WriteableAddress && !(page.access & Write))
        return InvalidAddress;

    return ValidAddress;
}


template<typename T>
FORCE_INLINE static T read_at(VirtualAddress addr)
{
    u32 page_index = addr >> page_bits;
    if(page_table[page_index].access & Read)
    {
        return *(T*)(MAPPED_BUS_ADDRESS_START + addr);
    } 
    else
    {
        invalid_read(addr);
        return T();
    }
}

QWord read_qword(VirtualAddress addr)
{
    return read_at<QWord>(addr);
}

DWord read_dword(VirtualAddress addr)
{
    return read_at<DWord>(addr);
}

u32 read_word(VirtualAddress addr)
{
    return read_at<u32>(addr);
}

u16 read_half(VirtualAddress addr)
{
    return read_at<u16>(addr);
}

u8 read_byte(VirtualAddress addr)
{
    return read_at<u8>(addr);
}

i16 read_ihalf(VirtualAddress addr)
{
    return read_at<i16>(addr);
}

i8 read_ibyte(VirtualAddress addr)
{
    return read_at<i8>(addr);
}

template<typename T>
inline void write_at(VirtualAddress addr, T value)
{
    VirtualAddress page_index = addr >> page_bits;
    if(page_table[page_index].access & Write)
    {
        if ((addr >> 28) == 1)
        {
            IO::write_io<T>(addr, value);
        }
        else
        {
            *(T*)(MAPPED_BUS_ADDRESS_START + addr) = value;
        }
    }
    else
    {
        invalid_write(addr);
    }
}

void write_qword(VirtualAddress addr, QWord qword)
{
    write_at<QWord>(addr, qword);
}

void write_dword(VirtualAddress addr, DWord dword)
{
    write_at<DWord>(addr, dword);
}

void write_word(VirtualAddress addr, Word word)
{
    write_at<Word>(addr, word);
}

void write_half(VirtualAddress addr, u16 half)
{
    write_at<u16>(addr, half);
}

void write_byte(VirtualAddress addr, u8 byte)
{
    write_at<u8>(addr, byte);
}

}
