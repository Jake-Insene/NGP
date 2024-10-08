/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

struct OS {

    static void sleep(i32 milisec);

    static void* allocate_virtual_memory(void* address, u64 size);

};
