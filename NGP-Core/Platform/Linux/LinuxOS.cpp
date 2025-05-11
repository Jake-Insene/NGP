/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/OS.h"

#include <sys/mman.h>


static inline i32 get_protection(OS::PageAccess access)
{
    switch(access)
    {
    case OS::PAGE_NONE:
        return PROT_NONE;
    case OS::PAGE_READ_ONLY:
        return PROT_READ;
    case OS::PAGE_READ_WRITE:
        return PROT_READ | PROT_WRITE;
    case OS::PAGE_NO_ACCESS:
        return PROT_NONE;
    }

    return 0;
}


void OS::initialize()
{

}

void OS::shutdown()
{

}

void OS::sleep(i32 milisec)
{

}

void* OS::allocate_virtual_memory(void* address, u64 size, PageAccess access)
{
    return mmap64(address, size, get_protection(access), MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
}

void OS::deallocate_virtual_memory(void* address)
{
    munmap(address, 0);
}

u32 OS::exception_handler(void*)
{
    return 0;
}
