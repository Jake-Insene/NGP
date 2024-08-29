/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/OS.h"
#include "Platform/Header.h"


void OS::sleep(i32 milisec) {
    Sleep(milisec);
}

void* OS::allocate_virtual_memory(void* address, u64 size) {
    return VirtualAlloc(address, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}
