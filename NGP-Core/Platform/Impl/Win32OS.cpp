// --------------------
// Win32OS.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "Platform/OS.h"
#include <Windows.h>


void OS::sleep(i32 milisec) {
    Sleep(milisec);
}
