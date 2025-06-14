/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

struct OS
{
    enum PageAccess
    {
        PAGE_NONE = 0x0,
        PAGE_READ_ONLY = 0x1,
        PAGE_READ_WRITE = 0x2,
        PAGE_NO_ACCESS = 0x3,
        PAGE_READ_WRITE_EXECUTE = 0x4,
    };

    using PageFaultHandler = void(*)(void*);

    static void initialize();
    static void shutdown();

    static void sleep(i32 milisec);

    static void* allocate_virtual_memory(void* address, usize size, PageAccess access);
    static void deallocate_virtual_memory(void* address);

    static u32 exception_handler(void*);
};
