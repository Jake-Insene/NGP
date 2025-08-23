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

struct EmulatorConfig
{
    CPUCore::ImplementationType impl_type;
};

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
        usize inst_counter;

        CPUCore& get_core() { return *core; }
    };

    static constexpr u64 CoreCount = CPUCore::CoreCount;
    static constexpr u64 ClockSpeed = CPUCore::ClockSpeed;
    static inline ThreadCore cores[CoreCount];

    static inline u64 frames_per_second = 60;
    static inline const char* bios_file = DefaultBIOSPath;
    static inline bool allow_continue = false;
    static inline bool pending_restart = false;

    static void initialize(const EmulatorConfig& config);
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
