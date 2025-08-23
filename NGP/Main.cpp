/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Memory/Bus.h"
#include "Emulator.h"

#include <cstdio>
#include <string>

EmulatorConfig config =
{
    // Interpreter by default.
    .impl_type = CPUCore::ImplementationType::Interpreter,
};


void print_help()
{
    puts(
        "usage: ngpas [options]\n"
        "options:\n"
        "\t-help show this help\n"
        "\t-bios <path> set the bios file\n"
    );
}

void handle_arguments(int argc, char** argv)
{
    printf("NGP Emulator %s\n", NGP_VERSION);


    i32 index = 1;
    while (index < argc)
    {
        std::string arg = argv[index++];

        if (arg[0] != '-')
        {
            printf("error: invalid argument '%s'\n", arg.c_str());
            exit(1);
        }

        // Options
        arg.erase(arg.begin());
        if (arg == "help")
        {
            print_help();
            exit(0);
        }
        else if (arg == "bios")
        {
            if (index == argc)
            {
                printf("error: -bios require a path");
                exit(1);
            }
            Emulator::bios_file = argv[index++];
        }
        else if (arg == "jit")
        {
            config.impl_type = CPUCore::ImplementationType::JIT;
        }
        else
        {
            printf("error: unknown option '%s'\n", arg.c_str());
            exit(1);
        }
    }
}

int main(int argc, char** argv)
{
    handle_arguments(argc, argv);
    Emulator::initialize(config);
    Emulator::run();
    Emulator::shutdown();
    return 0;
}

