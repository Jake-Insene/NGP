#include "Core/VirtualMachine.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>

int wWinMain(HINSTANCE, HINSTANCE, LPWSTR cmdline, int cmdShow) {
    initialize_display();

#ifndef NDEBUG
    AllocConsole();
#endif // NDEBUG

    system("\"..\\Build\\Debug-x64\\ngpas.exe\" ../NGPAS/Examples/main.s");

    int argc = 0;
    wchar_t** argv = CommandLineToArgvW(cmdline, &argc);
    
    VirtualMachine vm{};
    if (vm.load("../NGPAS/Examples/main.ngp") != STATUS_OK) {
        return -1;
    }
    vm.start();

    while (vm.display.is_open) {
        vm.display.update();
    }

    return 0;
}

#endif // _WIN32
