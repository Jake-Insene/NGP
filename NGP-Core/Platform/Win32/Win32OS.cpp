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

static inline OS::PageFaultHandler page_fault_handler = {};


DWORD get_protection(OS::PageAccess access)
{
    switch(access)
    {
    case OS::PAGE_READ_ONLY:
        return PAGE_READONLY;
    case OS::PAGE_READ_WRITE:
        return PAGE_READWRITE;
    case OS::PAGE_NO_ACCESS:
        return PAGE_NOACCESS;
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

void* OS::allocate_virtual_memory(void* address, u64 size, PageAccess access)
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

        if ((address & 0xF000'0000) == 0x1000'0000)
        {
            Emulator::allow_continue = true;
            Emulator::handle_readwrite_interrupt(VirtualAddress(address), exception_info->ExceptionRecord->ExceptionInformation[0] == 0);
        }
        else
        {
            printf(
                "error: trying to write to inaccesible memory address: 0x%p\n"
                "\tvirtual address: %08X\n", 
                (void*)address, VirtualAddress(address)
            );
        }
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

