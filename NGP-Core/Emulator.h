/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "CPU/CPUCore.h"

#include <thread>
#include <vector>


#define DEBUGGING 1

struct Emulator
{
    static constexpr const char* DefaultBIOSPath = "BIOS.BIN";

    enum Signal
    {
        NONE = 0,
        RUN,
        END,
    };

    struct ThreadCore
    {
        CPUCore* core;
        std::thread thread;

        Signal signal;

        f64 elapsed;
        usize last_cycle_counter;
        usize cycle_counter;
        usize clock_speed;
        usize inst_counter;
    };

    static inline std::vector<ThreadCore> cores;
    static inline u32 number_of_cores = 1;
    static inline u64 clock_cycles = MHZ(100);
    static inline const char* bios_file = DefaultBIOSPath;
    static inline bool allow_continue = false;
    static inline bool pending_restart = false;

    static void initialize();
    static void shutdown();

    static void start_cores();
    static void end_cores();
    static void cores_restore_context();
    static void print_cores();
    static void signal_cores(Signal signal);

    static void loop();
    static void run();

    static void handle_readwrite_interrupt(VirtualAddress address, bool read);
};
