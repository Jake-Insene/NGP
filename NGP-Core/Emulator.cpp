/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Emulator.h"

#include "IO/IO.h"
#include "Memory/Bus.h"
#include "Platform/OS.h"
#include "Platform/Time.h"
#include "Platform/Header.h"
#include "Video/GU.h"
#include "Video/Window.h"
#include <cstdlib>
#include <cstdio>
#include <new>

thread_local Emulator::ThreadCore* local_core = nullptr;

void thread_core_callback(void* arg)
{
    u32 core_index = *reinterpret_cast<u32*>(&arg);

    Emulator::ThreadCore& thread = Emulator::cores[core_index];
    local_core = &thread;

    f64 start = Time::get_time();
    thread.elapsed += Time::get_time() - start;

    continue_execution:
#if defined(_WIN32)
    __try
#endif
    {
        while (true)
        {
            if (thread.core.psr.HALT)
            {
#if DEBUGGING
                printf(
                    "Core: %d, MIPS: %llu, CPS: %llu, Elapsed: %f\n",
                    core_index, thread.core.inst_counter / 1'000'000,
                    thread.core.cycles_in_second, thread.elapsed
                );
                thread.core.inst_counter = 0;
                thread.core.cycles_in_second = 0;
                thread.core.cycle_counter = 0;
#endif
                return;
            }

            if (thread.core.cycles_in_second >= thread.core.clock_speed)
            {
                thread.signal = Emulator::NONE;
            }

            if (thread.elapsed >= 1.0 || thread.core.cycles_in_second >= thread.core.clock_speed)
            {
#if DEBUGGING
                printf(
                    "Core: %d, MIPS: %llu, CPS: %llu, Elapsed: %f\n",
                    core_index, thread.core.inst_counter / 1'000'000,
                    thread.core.cycles_in_second, thread.elapsed
                );
      
                thread.core.inst_counter = 0;
#endif
                if (thread.core.cycles_in_second > thread.core.clock_speed)
                {
                    u64 cycles_passed = thread.core.cycles_in_second - thread.core.clock_speed;
                    u32 to_wait = u32((cycles_passed * 1000) / thread.core.clock_speed);
                    OS::sleep(to_wait);
                }

                thread.elapsed = 0.0;
                thread.last_cycle_counter = thread.core.cycle_counter;
                thread.core.cycles_in_second = 0;

                if (thread.core.cycles_in_second >= thread.core.clock_speed)
                {
                    thread.signal = Emulator::NONE;
                }
            }

            switch (thread.signal)
            {
            case Emulator::NONE:
                break;
            case Emulator::RUN:
            {
                f64 start = Time::get_time();
                u64 cycles_per_step = thread.core.clock_speed / 60;
                thread.core.dispatch(cycles_per_step);

                thread.elapsed += Time::get_time() - start;
                thread.core.cycle_counter += thread.core.cycles_in_second;
            }
            break;
            case Emulator::END:
            {
                return;
            }
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

void Emulator::initialize()
{
    OS::initialize();
    Time::initialize();

    Bus::initialize();
    if (!Bus::load_bios(bios_file))
    {
        printf("error: invalid bios file '%s'\n", bios_file);
        std::exit(-1);
    }

    start_cores();
    IO::initialize();

    Window::initialize(Window::DefaultWindowWidth, Window::DefaultWindowHeight);

    GU::initialize(GU::VGU);
}

void Emulator::shutdown()
{
    end_cores();
    print_cores();

    delete[] cores;

    GU::shutdown();
    Window::shutdown();
    IO::shutdown();
    Bus::shutdown();
    Time::shutdown();
    OS::shutdown();
}

void Emulator::start_cores()
{
    if (cores == nullptr)
    {
        cores = new ThreadCore[number_of_cores]{};
    }
    else
    {
        ::new(cores) ThreadCore[number_of_cores]{};
    }

    for (u32 core = 0; core < number_of_cores; core++)
    {
        cores[core].core.initialize();

        // All cores are disable by default, except for core 0
        cores[core].core.psr.HALT = core == 0 ? false : true;
        cores[core].core.psr.CURRENT_EL = CPUCore::MaxExceptionLevel;
        cores[core].core.clock_speed = clock_cycles;

        cores[core].core.pc = Bus::BIOS_START;

        cores[core].core.handle_pc_change();

        cores[core].threadid = Thread::create(thread_core_callback, *reinterpret_cast<void**>(&core));
    }
}

void Emulator::end_cores()
{
    for (u32 core = 0; core < number_of_cores; core++)
    {
        Thread::terminate(cores[core].threadid);
        cores[core].core.shutdown();
    }
}

void Emulator::cores_restore_context()
{
    for (u32 core = 0; core < number_of_cores; core++)
    {
        Thread::terminate(cores[core].threadid);
        cores[core].core.shutdown();
    }

    start_cores();
}

void Emulator::print_cores()
{
    for (u32 core = 0; core < number_of_cores; core++)
    {
        printf("Core: %d\n", core);
        cores[core].core.print_pegisters();
    }
}

void Emulator::signal_cores(Signal signal)
{
    for (u32 core = 0; core < number_of_cores; core++)
    {
        cores[core].signal = signal;
    }
}

void Emulator::loop()
{
    // The main thread use the main core
    ThreadCore& main_core = cores[0];

    u32 cycle_counter = 0;
    u32 fps = 0;

    f64 elapsed = 0.0;
    f64 dt = 0.0;

    while (Window::is_open)
    {
        if (pending_restart)
        {
            cores_restore_context();
            pending_restart = false;
        }

        f64 start = Time::get_time();
        Window::update();
        GU::present(true);
        dt = Time::get_time() - start;
        elapsed += dt;

        if (elapsed >= 1.0)
        {
            elapsed = 0.0;
            signal_cores(RUN);
        }
    }
}

void Emulator::run()
{
    loop();
}

void Emulator::handle_readwrite_interrupt(VirtualAddress address, bool read)
{
}

