/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/CPU.h"
#include "Memory/MemoryBus.h"
#include "Video/Window.h"
#include "Platform/Time.h"
#include <stdio.h>
#include <math.h>

#ifdef _WIN32
#include <Video/Impl/GPUD12.h>
#endif // _WIN32

#define DEBUGGING 1

int main(int /*argc*/, char** /*argv*/) {
    if (system("\"..\\Build\\Debug-x64\\ngpas.exe\" ../NGPAS/Examples/main.asm") != 0) {
        return -1;
    }

    Time::initialize();
    CPU::initialize();

    Window::initialize(960, 544);
    if (!MemoryBus::load_rom("../NGPAS/Examples/main.ngp")) {
        return -1;
    }

#ifdef _WIN32
    GPU::initialize(GPUD12::get());
#endif

    f64 elapsed = Time::get_time();
    u32 fps = 0;

    while (Window::is_open) {
        f64 start = Time::get_time();
        // For debugging
#if DEBUGGING == 1
        if (CPU::registers.psr.halt) {
            break;
        }
#endif // DEBUGGING

        Window::update();
        CPU::dispatch();

        elapsed += Time::get_time() - start;
        fps += 1;

        if (elapsed >= 1.0) {
            printf("Frame Rate: %f, FPS: %d, MIPS: %d\n", elapsed, fps, CPU::instruction_counter);

            fps = 0;
            elapsed = 0.0;
            CPU::instruction_counter = 0;
        }
    }

    CPU::print_pegisters();

    GPU::shutdown();
    Window::shutdown();
    CPU::shutdown();
    Time::initialize();

    return 0;
}
