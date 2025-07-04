/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Emulator.h"

#include "IO/IO.h"
#include "Memory/Bus.h"
#include "Platform/Header.h"
#include "Platform/OS.h"
#include "Platform/Time.h"
#include "Video/GUDevice.h"
#include "Video/Window.h"

#include <cstdlib>
#include <cstdio>
#include <chrono>

thread_local Emulator::ThreadCore* local_core = nullptr;

void thread_core_callback(void* arg)
{
    u32 core_index = *reinterpret_cast<u32*>(&arg);

    Emulator::ThreadCore& thread = Emulator::cores[core_index];
    local_core = &thread;

    continue_execution:
#if defined(_WIN32)
    __try
#endif
    {
        while (true)
        {
            if (thread.get_core().get_psr().HALT)
            {
#if DEBUGGING
                printf(
                    "Core: %d, MIPS: %llu, CPS: %llu, Elapsed: %f\n",
                    core_index, thread.inst_counter / 1'000'000,
                    thread.cycle_counter, thread.elapsed
                );
                thread.inst_counter = 0;
                thread.elapsed = 0.0;
                thread.last_cycle_counter = 0;
                thread.cycle_counter = 0;
#endif
                return;
            }

            if (thread.elapsed >= 1.0 || thread.cycle_counter >= thread.clock_speed)
            {
#if DEBUGGING
                printf(
                    "Core: %d, MHZS: %llu, CPS: %llu, Elapsed: %f\n",
                    core_index, thread.inst_counter / 1'000'000,
                    thread.cycle_counter, thread.elapsed
                );
      
                thread.inst_counter = 0;
#endif
                if (thread.cycle_counter > thread.clock_speed)
                {
                    u64 cycles_passed = thread.cycle_counter - thread.clock_speed;
                    u32 to_wait = u32((cycles_passed * 1000) / thread.clock_speed);
                    OS::sleep(to_wait);
                }

                thread.elapsed = 0.0;
                thread.last_cycle_counter = thread.cycle_counter;

                if (thread.cycle_counter >= thread.clock_speed && thread.signal != Emulator::END)
                {
                    thread.signal = Emulator::NONE;
                }

                thread.cycle_counter = 0;
            }

            switch (thread.signal)
            {
            case Emulator::RUN:
            {
                auto start = Time::get_time();
                u64 cycles_per_step = thread.clock_speed / Emulator::frames_per_second;
                
                usize remain = thread.get_core().dispatch(cycles_per_step);

                auto dt = Time::get_time() - start;
                thread.elapsed += dt;
                thread.cycle_counter += cycles_per_step - remain;
                
                // WARNING: This is not precise
                thread.inst_counter += cycles_per_step - remain;
            }
            break;
            case Emulator::END:
                return;
            default:
                break;
            }
        }
    }
#if defined(_WIN32)
    __except (OS::exception_handler(_exception_info()))
#endif
    {
        if (Emulator::allow_continue)
        {
            Emulator::allow_continue = false;
        }
        else
        {
            Emulator::pending_restart = true;
            return;
        }
    }

    goto continue_execution;
}

void Emulator::initialize(const EmulatorConfig& config)
{
#if defined(NGP_BUILD_VAR)
    cores.resize(config.core_count);
    core_count = config.core_count;
    clock_cycles = config.clock_speed;

    for (i32 i = 0; i < core_count; i++)
    {
        cores[i].core = CPUCore::create_cpu(config.cpu_type);
    }
#endif

    OS::initialize();
    Time::initialize();

#if defined(NGP_BUILD_VAR)
    Bus::initialize(config.ram_size);
#else
    Bus::initialize(BuildConfig::RAMSize);
#endif
    if (!Bus::load_bios(bios_file))
    {
        printf("error: invalid bios file '%s'\n", bios_file);
        std::exit(-1);
    }

    start_cores();
    IO::initialize();

    Window::initialize(Window::DefaultWindowWidth, Window::DefaultWindowHeight);
#if defined(NGP_BUILD_VAR)
    GUDevice::initialize(GUDevice::VGU, config.vram_size);
#else
    GUDevice::initialize(GUDevice::VGU, BuildConfig::VRAMSize);
#endif

    auto thread_count = std::thread::hardware_concurrency();
}

void Emulator::shutdown()
{
    end_cores();
    print_cores();

#if defined(NGP_BUILD_VAR)
    cores.clear();
#endif

    GUDevice::shutdown();
    Window::shutdown();
    IO::shutdown();
    Bus::shutdown();
    OS::shutdown();
}

void Emulator::start_cores()
{
    for (u32 core = 0; core < core_count; core++)
    {
        cores[core].get_core().initialize();

        // All cores are disable by default, except for core 0
        CPUCore::ProgramStateRegister initial_psr =
        {
            .HALT = core == 0 ? false : true,
            .CURRENT_EL = CPUCore::MaxExceptionLevel,
        };
        cores[core].get_core().set_psr(initial_psr);
#if defined(NGP_BUILD_VAR)
        cores[core].clock_speed = clock_cycles;
#endif

        cores[core].get_core().set_pc(Bus::BIOS_START);

        cores[core].thread = std::thread(thread_core_callback, *reinterpret_cast<void**>(&core));
    }
}

void Emulator::end_cores()
{
    for (u32 core = 0; core < core_count; core++)
    {
        cores[core].thread.join();
        cores[core].get_core().shutdown();
    }
}

void Emulator::cores_restore_context()
{
    for (u32 core = 0; core < core_count; core++)
    {
        cores[core].thread.join();
        cores[core].get_core().shutdown();
    }

    start_cores();
}

void Emulator::print_cores()
{
    for (u32 core = 0; core < core_count; core++)
    {
        printf("Core: %d\n", core);
        cores[core].get_core().print_registers();
    }
}

void Emulator::signal_cores(Signal signal)
{
    for (u32 core = 0; core < core_count; core++)
    {
        if(!cores[core].get_core().get_psr().HALT)
        {
            cores[core].signal = signal;
        }
    }
}

void Emulator::loop()
{
    // The main thread use the main core
    i32 fps = 0;
    f64 elapsed = 0.0;
    f64 dt = 0.0;

    while (Window::is_open)
    {
        if (pending_restart)
        {
            cores_restore_context();
            pending_restart = false;
        }

        auto start = Time::get_time();

        Window::update();
        IO::dispatch();
        GUDevice::present(true);
        fps++;
        elapsed += Time::get_time() - start;

        if (elapsed >= 1.0)
        {
            printf("FPS: %d, Elapsed: %f\n", fps, elapsed);

            fps = 0;
            elapsed = 0.0;
            signal_cores(RUN);
        }
    }

    signal_cores(END);
}

void Emulator::run()
{
    loop();
}

void Emulator::handle_readwrite_interrupt(VirtualAddress, bool)
{
}

