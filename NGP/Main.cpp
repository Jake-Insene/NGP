#include <CPU/CPU.h>
#include <GPU/Display.h>
#include <Platform/Time.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#include <GPU/Impl/GPUD12.h>
#endif // _WIN32

int main(int argc, char** argv) {
    if (system("\"..\\Build\\Debug-x64\\ngpas.exe\" ../NGPAS/Examples/main.asm") != 0) {
        return -1;
    }

    Time::initialize();
    CPU::initialize();

    Display::initialize(800, 600);
    if (!CPU::load("../NGPAS/Examples/main.ngp")) {
        return -1;
    }

#ifdef _WIN32
    GPU::initialize(new GPUD12());
#endif

    while (Display::is_open) {
        Display::update();

        CPU::dispatch();
        CPU::delayForTiming();
    }
    
    GPU::shutdown();
    Display::shutdown();
    CPU::shutdown();
    Time::initialize();

    return 0;
}
