#include "Platform/OS.h"
#include <Windows.h>


void OS::sleep(i32 milisec) {
    Sleep(milisec);
}
