/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/OS.h"
#include <Windows.h>


void OS::sleep(i32 milisec) {
    Sleep(milisec);
}
