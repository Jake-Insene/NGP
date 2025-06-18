/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Memory/Bus.h"

#include "Bus.h"
#include "Core/Header.h"
#include "IO/IO.h"
#include "CPU/CPUCore.h"
#include "Emulator.h"
#include "Platform/OS.h"
#include <fstream>

extern thread_local Emulator::ThreadCore* local_core;


void Bus::initialize()
{
    // VRAM is managed by the GU
    Word page_count = 0x1'0000'0000 >> page_bits;
    page_table.resize(page_count);

    bios = (PhysicalAddress)OS::allocate_virtual_memory((void*)MAPPED_BUS_ADDRESS_START, BIOS_SIZE, OS::PAGE_READ_WRITE);

    io = (PhysicalAddress)OS::allocate_virtual_memory((void*)(MAPPED_BUS_ADDRESS_START + IO_START), RAM_START - IO_START, OS::PAGE_READ_WRITE);

    ram = (PhysicalAddress)OS::allocate_virtual_memory((void*)(MAPPED_BUS_ADDRESS_START + RAM_START), usize(ram_size), OS::PAGE_READ_WRITE);

    // By default every bios, ram and io page is accessible
    for(usize i = 0; i < page_count; i++)
    {
        VirtualAddress page_address = i << page_bits;
        page_table[i].page_index = i;

        page_table[i].physical_address = PhysicalAddress(MAPPED_BUS_ADDRESS_START + page_address);
        page_table[i].page_address = page_address;
        if(page_address < BIOS_END)
        {
            page_table[i].access = PageAccess(PageRead | PageWrite | PageExecute);
        }
        else if(page_address >= IO_START && page_address < IO_END)
        {
            page_table[i].access = PageAccess(PageRead | PageWrite);
        }
        else if(page_address >= RAM_START && page_address < RAM_START + ram_size)
        {
            page_table[i].access = PageAccess(PageRead | PageWrite | PageExecute);
        }
        else
        {
            page_table[i].access = PageNone;
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

void Bus::shutdown()
{
    OS::deallocate_virtual_memory((void*)bios);
    OS::deallocate_virtual_memory((void*)io);
    OS::deallocate_virtual_memory((void*)ram);
}

void Bus::set_ram_size(u32 new_size)
{
    if (new_size >= MAX_ALLOWED_RAM)
    {
        printf("error: invalid ram size %d MB, the maximum allowed is %d MB\n", new_size, MAX_ALLOWED_RAM_MB);
    }
    ram_size = new_size;
}

void Bus::invalid_read(VirtualAddress addr)
{
    local_core->core->external_handle_exception(CPUCore::AccessViolationException, CPUCore::CannotRead, addr);
}

void Bus::invalid_write(VirtualAddress addr)
{
    local_core->core->external_handle_exception(CPUCore::AccessViolationException, CPUCore::CannotWrite, addr);
}

bool Bus::load_bios(const char* path)
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

template<typename T>
static FORCE_INLINE T read_at(VirtualAddress addr)
{
    const Word page_index = Bus::get_page_index(addr);
    if(Bus::page_table[page_index].access & Bus::PageRead)
    {
        return *(T*)(Bus::MAPPED_BUS_ADDRESS_START + addr);
    }
    else
    {
        Bus::invalid_read(addr);
        return T();
    }
}

QWord Bus::read_qword(VirtualAddress addr)
{
    return read_at<QWord>(addr);
}

DWord Bus::read_dword(VirtualAddress addr)
{
    return read_at<DWord>(addr);
}

u32 Bus::read_word(VirtualAddress addr)
{
    return read_at<u32>(addr);
}

u16 Bus::read_half(VirtualAddress addr)
{
    return read_at<u16>(addr);
}

u8 Bus::read_byte(VirtualAddress addr)
{
    return read_at<u8>(addr);
}

i16 Bus::read_ihalf(VirtualAddress addr)
{
    return read_at<i16>(addr);
}

i8 Bus::read_ibyte(VirtualAddress addr)
{
    return read_at<i8>(addr);
}

template<typename T>
inline void write_at(VirtualAddress addr, T value)
{
    VirtualAddress page_index = Bus::get_page_index(addr);
    if(Bus::page_table[page_index].access & Bus::PageWrite)
    {
        if ((addr >> 28) == 1)
        {
            IO::write_io<T>(addr, value);
        }
        else
        {
            *(T*)(Bus::MAPPED_BUS_ADDRESS_START + addr) = value;
        }
    }
    else
    {
        Bus::invalid_write(addr);
    }
}

void Bus::write_qword(VirtualAddress addr, QWord qword)
{
    write_at<QWord>(addr, qword);
}

void Bus::write_dword(VirtualAddress addr, DWord dword)
{
    write_at<DWord>(addr, dword);
}

void Bus::write_word(VirtualAddress addr, Word word)
{
    write_at<Word>(addr, word);
}

void Bus::write_half(VirtualAddress addr, u16 half)
{
    write_at<u16>(addr, half);
}

void Bus::write_byte(VirtualAddress addr, u8 byte)
{
    write_at<u8>(addr, byte);
}
