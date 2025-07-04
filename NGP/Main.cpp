/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Memory/Bus.h"
#include "Emulator.h"
#include "BuildConfig.h"

#include <cstdio>
#include <string>

EmulatorConfig config = {};


void print_help()
{
    puts(
        "usage: ngpas [options]\n"
        "options:\n"
        "\t-help show this help\n"
        "\t-bios <path> set the bios file\n"
#if defined(NGP_BUILD_VAR)
        "\t-m <MB> size of the ram\n"
        "\t-vram <MB> set the size of vram\n"
        "\t-cores <N> number of cores of the machine\n"
        "\t-cycles <N> number of cycles per core\n"
#endif
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
#if defined(NGP_BUILD_VAR)
        else if (arg == "ram")
        {
            if (index == argc)
            {
                printf("error: -ram require a number\n");
                exit(1);
            }
            config.ram_size = MB(std::atoi(argv[index++]));
        }
        else if (arg == "cores")
        {
            if (index == argc)
            {
                printf("error: -cores require a number\n");
                exit(1);
            }
            config.core_count = std::atoi(argv[index++]);
        }
        else if (arg == "vram")
        {
            if (index == argc)
            {
                printf("error: -vram require a number\n");
                exit(1);
            }
            config.vram_size = MB(std::atoi(argv[index++]));
        }
        else if (arg == "cycles")
        {
            if (index == argc)
            {
                printf("error: -cycles require a number\n");
                exit(1);
            }

            config.vram_size = MB(std::atoi(argv[index++]));
            Emulator::clock_cycles = std::atoi(argv[index++]);
        }
#endif
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

