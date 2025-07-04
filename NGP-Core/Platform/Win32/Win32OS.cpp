/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/OS.h"

#include "Emulator.h"
#include "Platform/Header.h"
#include <cstdio>

extern thread_local Emulator::ThreadCore* local_core;


DWORD get_protection(OS::PageAccess access)
{
    switch(access)
    {
    case OS::PAGE_NONE:
        return 0;
    case OS::PAGE_READ_ONLY:
        return PAGE_READONLY;
    case OS::PAGE_READ_WRITE:
        return PAGE_READWRITE;
    case OS::PAGE_NO_ACCESS:
        return PAGE_NOACCESS;
    case OS::PAGE_READ_WRITE_EXECUTE:
        return PAGE_EXECUTE_READWRITE;
    }

    return 0;
}

void OS::initialize()
{
}

void OS::shutdown()
{}

void OS::sleep(i32 milisec)
{
    Sleep(milisec);
}

void* OS::allocate_virtual_memory(void* address, usize size, PageAccess access)
{
    return VirtualAlloc(address, size, MEM_COMMIT | MEM_RESERVE, get_protection(access));
}

void OS::deallocate_virtual_memory(void* address)
{
    VirtualFree(address, 0, MEM_RELEASE);
}

u32 OS::exception_handler(void* ptr)
{
    EXCEPTION_POINTERS* exception_info = (EXCEPTION_POINTERS*)ptr;
    if (exception_info && exception_info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    {
        PhysicalAddress address = exception_info->ExceptionRecord->ExceptionInformation[1];

        if((address & 0xFFFF'FFFF'0000'0000) == Bus::MAPPED_BUS_ADDRESS_START)
        {
            printf(
                "error: trying to read/write to inaccessible memory address: %016llX\n"
                "\tvirtual address: %08X\n",
                address, VirtualAddress(address)
            );
            local_core->get_core().external_handle_exception(CPUCore::AccessViolationException, CPUCore::CommentNone, VirtualAddress(address));
        }
        else
        {
            printf(
                "error: emulator crash while read/write to address: %016llX\n",
                address
            );
        }
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

