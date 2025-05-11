/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Core/Header.h"
#include "Platform/Time.h"
#include "Backend/Assembler.h"
#include <cstring>

void print_help()
{
    puts(
        "usage: ngpas <source> [-o output]\n"
        "options:\n"
        "\t-o set output file\n"
        "\t-v show the assembler version"
    );
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        puts(
            "error: not input file specified\n"
            "error: use -help for help"
        );
        return -1;
    }

    Assembler as{};
    i32 index = 1;
    const char* input_file = "";
    std::string output_file = {};

    while (index < argc)
    {
        char* arg = argv[index++];

        if (arg[0] == '-')
        {
            // Options
            arg++;
            u32 arg_len = u32(std::strlen(arg));

            if (arg_len == 4)
            {
                if (std::memcmp(arg, "help", 4) == 0)
                {
                    print_help();
                    return 0;
                }
                else
                {
                    printf("error: unknown option '%s'\n", arg--);
                    return -1;
                }
            }
            else if (arg_len == 1)
            {
                if (arg[0] == 'o')
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
                else if (arg[0] == 'v')
                {
                    printf("NGP assebmler version: %s\n", NGP_VERSION);
                    return 0;
                }
            }
        }
        else
        {
            input_file = arg;
        }
    }

    Time::initialize();

    if (output_file.empty())
    {
        output_file = input_file;
        output_file = output_file.substr(0, output_file.find_last_of('.'));
    }

    auto start = Time::get_time();
    bool result = as.assemble_file(input_file, output_file.c_str());
    f64 duration = Time::get_time() - start;

    printf("assembly tooks %fs\n", duration);

    Time::shutdown();
    return result ? 0 : -1;
}
