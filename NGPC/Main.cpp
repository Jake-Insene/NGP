/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Core/Header.h"
#include "Backend/CCompiler.h"

#include <cstring>
#include <chrono>


void print_help()
{
    puts(
        "usage: ngpc <source> [-o output]\n"
        "options:\n"
        "\t-o set output file\n"
        "\t-v show the compiler version"
    );
}

int main(int argc, char** argv)
{
    printf("NGP Compiler %s\n", NGP_VERSION);

    if (argc < 2)
    {
        puts(
            "error: not input file specified\n"
            "error: use -help for help"
        );
        return -1;
    }

    CCompiler cc{};
    i32 index = 1;
    std::string input_file = "";
    std::string output_file = {};

    while (index < argc)
    {
        std::string arg = argv[index++];

        if (arg[0] == '-')
        {
            // Options
            arg.erase(arg.begin());
            if (arg == "help")
            {
                print_help();
                return 0;
            }
            else if (arg == "o")
            {
                if (index == argc)
                {
                    printf("error: -o require a path");
                    return -1;
                }
                else
                {
                    output_file = argv[index++];
                }
            }
            else if (arg == "v")
            {
                printf("NGP compiler version: %s\n", NGP_VERSION);
                return 0;
            }
            else
            {
                printf("error: unknown option '%s'\n", arg.c_str());
                return -1;
            }
        }
        else
        {
            input_file = arg;
        }
    }


    if (output_file.empty())
    {
        output_file = input_file;
        output_file = output_file.substr(0, output_file.find_last_of('.'));
    }

    auto start = std::chrono::high_resolution_clock::now();
    bool result = cc.compile_file(input_file.c_str(), output_file.c_str());
    f64 duration = (std::chrono::high_resolution_clock::now() - start).count() / 1'000'000'000.0;

    printf("compilation took's %fs\n", duration);

    return result ? 0 : -1;
}
