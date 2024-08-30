/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/CPU.h"
#include "Memory/Bus.h"
#include "Video/GPU.h"
#include "Video/Window.h"
#include "Platform/OS.h"
#include "Platform/Time.h"
#include "Platform/Thread.h"
#include "IO/IO.h"
#include <cstdio>
#include <memory>

#define DEBUGGING 1

void start_cores();
void end_cores();
void print_cores();

ThreadID* core_threads = nullptr;
CPU* cores = nullptr;
u32 number_of_cores = 2;

void print_help() {
    puts(
        "usage: ngpas [options]\n"
        "options:\n"
        "\t-bios <path> set the bios file\n"
    );
}

int main(int argc, char** argv) {
    printf("NGP Emulator %s\n", NGP_VERSION);

    const char* bios_file = "../BIOS/BIOS.BIN";

    i32 index = 1;
    while (index < argc) {
        char* arg = argv[index++];

        if (arg[0] == '-') {
            // Options
            arg++;
            u32 arg_len = u32(strlen(arg));

            if (arg_len == 4) {
                if (std::memcmp(arg, "help", 4) == 0) {
                    print_help();
                    return 0;
                }
                else if (std::memcmp(arg, "bios", 4) == 0) {
                    printf("error: -bios require a path");
                    bios_file = argv[index++];
                    return -1;
                }
                else {
                    printf("error: unknown option '%s'\n", arg--);
                    return -1;
                }
            }
        }
        else {
            printf("error: invalid argument '%s'\n", arg);
        }
    }

    Time::initialize();
    
    Bus::initialize();
    if (!Bus::load_bios(bios_file)) {
        printf("error: invalid bios file '%s'", bios_file);
        return -1;
    }

    start_cores();
    IO::initialize();

    Window::initialize(960, 540);

#ifdef _WIN32
    GPU::initialize(DriverApi::D3D12);
#endif

    // The main thread use the main core
    CPU& main_core = cores[0];

    f64 elapsed = 0.0;
    u32 cycle_counter = 0;
    u32 fps = 0;

    while (Window::is_open) {
        f64 start = Time::get_time();
#if DEBUGGING
        // For debugging
        if (main_core.psr.halt) {
            break;
        }
#endif // DEBUGGING
        
        Window::update();
        fps++;
        
        main_core.dispatch();

        elapsed += Time::get_time() - start;
        cycle_counter += main_core.cycle_counter;
        main_core.cycle_counter = 0;

        if (elapsed >= 1.0) {
            printf(
                "Core 0:\n"
                "\tFrame Rate: %f\n"
                "\tFPS: %d\n"
                "\tMIPS: %d\n"
                "\tCPS: %d\n",
                elapsed, fps, main_core.inst_counter, cycle_counter
            );

            elapsed = 0.0;
            main_core.inst_counter = 0;
            cycle_counter = 0;
            fps = 0;
        }
    }

    end_cores();
    print_cores();

    GPU::shutdown();
    Window::shutdown();
    IO::shutdown();
    Bus::shutdown();
    Time::shutdown();

    return 0;
}

void thread_core_callback(void* arg) {
    u32 core_index = *reinterpret_cast<u32*>(&arg);

    CPU& core = cores[core_index];

    core.pc = Bus::BIOS_START;
    core.current_el = 0;

    f64 elapsed = 0.0;
    u32 cycle_counter = 0;

    while (true) {
        if (core.psr.halt) {
            OS::sleep(1);
            continue;
        }

        f64 start = Time::get_time();
        core.dispatch();

        elapsed += Time::get_time() - start;
        cycle_counter += core.cycle_counter;
        core.cycle_counter = 0;

        if (elapsed >= 1.0) {
            printf(
                "Core: %d\n"
                "\tMIPS: %d\n"
                "\tCPS: %d\n",
                core_index, core.inst_counter, cycle_counter
            );

            elapsed = 0.0;
            core.inst_counter = 0;
            cycle_counter = 0;
        }
    }
}

void start_cores() {
    core_threads = new ThreadID[number_of_cores]{};
    cores = new CPU[number_of_cores]{};

    cores[0].initialize();

    for (u32 core = 1; core < number_of_cores; core++) {
        cores[core].initialize();
        core_threads[core] = Thread::create(thread_core_callback, *reinterpret_cast<void**>(&core));

        // All core are disable by default
        cores[core].psr.halt = true;

        Thread::start(core_threads[core]);
    }
}

void end_cores() {
    for (u32 core = 1; core < number_of_cores; core++) {
        Thread::terminate(core_threads[core]);

        cores[core].shutdown();
    }
}

void print_cores() {
    for (u32 core = 0; core < number_of_cores; core++) {
        printf("Core: %d\n", core);
        cores[core].print_pegisters();
    }
}
