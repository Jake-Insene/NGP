/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "BuildConfig.h"

#include <thread>
#include <vector>


#define DEBUGGING 1

struct EmulatorConfig
{
    CPUCore::CPUType cpu_type;
    Word core_count;
    usize clock_speed;

    Word ram_size;
    Word vram_size;
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
#if defined(NGP_BUILD_VAR)
        CPUCore* core;
#else
        BuildConfig::CoreType core;
#endif
        std::thread thread;

        Signal signal;

        f64 elapsed;
        usize last_cycle_counter;
        usize cycle_counter;
#if defined(NGP_BUILD_VAR)
        usize clock_speed;
#else
        static constexpr usize clock_speed = BuildConfig::ClockSpeed;
#endif
        usize inst_counter;

#if defined(NGP_BUILD_VAR)
        CPUCore& get_core() { return *core; }
#else
        BuildConfig::CoreType& get_core() { return core; }
#endif
    };

#if defined(NGP_BUILD_VAR)
    static inline std::vector<ThreadCore> cores;
    static inline u64 core_count = 1;
    static inline u64 clock_cycles = MHZ(10);
#else
    static constexpr u64 core_count = BuildConfig::CoreCount;
    static constexpr u64 clock_cycles = BuildConfig::ClockSpeed;
    static inline ThreadCore cores[core_count];
#endif

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
