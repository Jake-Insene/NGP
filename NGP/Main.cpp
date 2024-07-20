// --------------------
// Main.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "CPU/CPU.h"
#include "Memory/MemoryBus.h"
#include "GPU/Display.h"
#include "Platform/Time.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#include <GPU/Impl/GPUD12.h>
#endif // _WIN32

#define DEBUGGING 1

int main(int argc, char** argv) {
    if (system("\"..\\Build\\Debug-x64\\ngpas.exe\" ../NGPAS/Examples/main.asm") != 0) {
        return -1;
    }

    Time::initialize();
    CPU::initialize();

    Display::initialize(800, 600);
    if (!MemoryBus::loadRom("../NGPAS/Examples/main.ngp")) {
        return -1;
    }

#ifdef _WIN32
    GPU::initialize(new GPUD12());
#endif

    while (Display::is_open) {
        // For debugging
#if DEBUGGING == 1
        if (CPU::registers.psr.halt) {
            break;
        }
#endif // DEBUGGING

        Display::update();

        CPU::dispatch();
        CPU::delayForTiming();
        
        CPU::registers.cycle_counter = 0;
    }

    CPU::printRegisters();
    
    GPU::shutdown();
    Display::shutdown();
    CPU::shutdown();
    Time::initialize();

    return 0;
}
