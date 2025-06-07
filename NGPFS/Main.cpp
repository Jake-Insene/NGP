/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Core/Header.h"
#include "FileFormat/NGPFS.h"
#include <string>
#include <fstream>

void print_help()
{
    puts(
        "usage: ngpfs [options]\n"
        "options:\n"
        "\t-help show this help\n"
        "\t-disk <file> format file disk\n"
        "\t-size <MB> size in MB of disk to create/use\n"
    );
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		print_help();
		return -1;
	}

    printf("NGP File System Tool %s\n", NGP_VERSION);

    std::string disk_name = "";
    // In MB
    i32 disk_size = 0;

    i32 index = 1;
    while (index < argc)
    {
        std::string arg = argv[index++];

        // Options
        if (arg[0] == '-')
        {
            arg.erase(arg.begin());
            if (arg == "help")
            {
                print_help();
                exit(0);
            }
            else if (arg == "disk")
            {
                if (index == argc)
                {
                    printf("error: -disk require a name argument");
                    exit(1);
                }
                disk_name = argv[index++];
            }
            else if (arg == "size")
            {
                if (index == argc)
                {
                    printf("error: -size require a number argument");
                    exit(1);
                }
                disk_size = std::atoi(argv[index++]) * 1024 * 1024;
            }
            else
            {
                printf("error: unknown option '%s'\n", arg.c_str());
                exit(1);
            }
        }
        else
        {
            printf("error: invalid argument '%s'\n", arg.c_str());
            exit(1);
        }
    }

    std::fstream disk_file{ disk_name, std::ios::out | std::ios::binary };
    if(!disk_file.is_open())
    {
        printf("error: cannot create disk file '%s'\n", disk_name.c_str());
        exit(1);
	}

    usize i = 0;
    while (i < disk_size)
    {
        disk_file.write("\0", 1);
        i++;
    }

    disk_file.close();

	return 0;
}