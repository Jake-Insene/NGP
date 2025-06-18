/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"
#include <vector>

struct Bus
{

    // BIOS	00000000 - 003FFFFF = 4 MB
    // IO   100000000 - 1FFFFFFF = N MB
    // RAM	20000000 - FFFFFFFF = N MB
    // VRAM is in its own address space = N MB

    static constexpr u32 BIOS_START = 0x0000'0000;
    static constexpr u32 BIOS_END = 0x003F'FFFF;

    static constexpr u32 IO_START = 0x1000'0000;
    static constexpr u32 IO_END = 0x1FFF'FFFF;

    static constexpr u32 RAM_START = 0x2000'0000;

    static constexpr u32 BIOS_SIZE = MB(4U);

    static constexpr u32 MAX_ALLOWED_RAM = 0x1'0000'0000 - 0x2000'0000;
    static constexpr u32 MAX_ALLOWED_RAM_MB = MAX_ALLOWED_RAM / 1'024 / 1'024;

    static constexpr u64 MAPPED_BUS_ADDRESS_START = 0x2'0000'0000;

    enum CheckAddressResult
    {
        ValidAddress = 0,
        InvalidAddress,
    };

    enum CheckAddressFlags
    {
        WriteableAddress = 0x1,
        ReadeableAddress = 0x1,
    };

    enum PageAccess
    {
        PageNone = 0x0,
        PageRead = 0x1,
        PageWrite = 0x2,
        PageExecute = 0x4,
    };

    enum PageSize
    {
        Page4KB = 0x1000,
        Page64KB = 0x1'0000,
        Page1MB = 0x1'00000,
        Page4MB = 0x4'00000,
    };

    enum PageBits
    {
        Page4KBits = 12,
        Page64KBits = 16,
        Page1MBits = 20,
        Page4MBits = 22,
    };

    enum PageMask
    {
        Page4KBMask = 0xFFF,
        Page64KBMask = 0xFFFF,
        Page1MBMask = 0xF'FFFF,
        Page4MBMask = 0x3F'FFFF,
    };

    struct Page
    {
        PhysicalAddress physical_address;
        VirtualAddress page_address;
        // 2^20 == 1'048'575 page indexes, this is enough for 4 GB of address space
        Word page_index : 20;
        PageAccess access : 12;
    };

    static inline std::vector<Page> page_table;

    static inline PageSize page_size = Page4KB;
    static inline PageMask page_mask = Page4KBMask;
    static inline PageBits page_bits = Page4KBits;

    static inline PhysicalAddress bios;
    static inline PhysicalAddress io;
    static inline PhysicalAddress ram;

    // Default to 256 MB
    static inline Word ram_size = MB(256);
    // Default to 32 MB
    static inline Word vram_size = MB(32);

    static void initialize();
    static void shutdown();

    static void set_ram_size(Word new_size);
    static Word get_ram_size() { return ram_size; }

    static FORCE_INLINE void set_page_size(PageSize new_page_size)
    {
        page_size = new_page_size;
        page_mask = PageMask(new_page_size - 1);
        page_bits = PageBits(bits_of(page_mask));
    }

    static FORCE_INLINE PageSize get_page_size() { return page_size; }
    static FORCE_INLINE PageBits get_page_bits() { return page_bits; }
    static FORCE_INLINE PageMask get_page_mask() { return page_mask; }

    static FORCE_INLINE Page& get_page(VirtualAddress addr)
    {
        return page_table[addr >> page_bits];
    }

    static FORCE_INLINE Word get_page_index(VirtualAddress addr)
    {
        return addr >> page_bits;
    }

    static FORCE_INLINE Word get_page_offset(VirtualAddress addr)
    {
        return addr & page_mask;
    }

    static void set_vram_size(Word new_size) { vram_size = new_size; }
    static Word get_vram_size() { return vram_size; }

    static PhysicalAddress bios_start_address() { return PhysicalAddress(bios); }
    static PhysicalAddress ram_start_address() { return PhysicalAddress(ram); }
    static PhysicalAddress io_start_address() { return PhysicalAddress(io); }

    static void invalid_read(VirtualAddress addr);
    static void invalid_write(VirtualAddress addr);

    static bool load_bios(const char* path);

    static FORCE_INLINE CheckAddressResult check_virtual_address(VirtualAddress va, CheckAddressFlags flags)
    {
        VirtualAddress page_index = va >> page_bits;
        Page& page = page_table[page_index];
        if(flags & WriteableAddress && !(page.access & PageWrite))
            return InvalidAddress;
    
        if (flags & ReadeableAddress && !(page.access & PageRead))
            return InvalidAddress;
    
        return ValidAddress;
    }

    static FORCE_INLINE PhysicalAddress get_physical_addr(VirtualAddress pc)
    {
        return MAPPED_BUS_ADDRESS_START + PhysicalAddress(pc);
    }

    static QWord read_qword(VirtualAddress addr);
    static DWord read_dword(VirtualAddress addr);
    static Word read_word(VirtualAddress addr);
    static u16 read_half(VirtualAddress addr);
    static u8 read_byte(VirtualAddress addr);
    static i16 read_ihalf(VirtualAddress addr);
    static i8 read_ibyte(VirtualAddress addr);

    static void write_qword(VirtualAddress addr, QWord qword);
    static void write_dword(VirtualAddress addr, DWord dword);
    static void write_word(VirtualAddress addr, Word word);
    static void write_half(VirtualAddress addr, u16 half);
    static void write_byte(VirtualAddress addr, u8 byte);

};