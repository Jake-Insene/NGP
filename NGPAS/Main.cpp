// --------------------
// Main.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include <Core/Header.h>
#include <Platform/Time.h>
#include "Backend/Assembler.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("error: not input file specified");
        return -1;
    }

    Assembler as{};
    Time::initialize();

    std::string output = { argv[1] };
    output = output.substr(0, output.find_last_of('.') + 1);
    output += "ngp";
    
    auto start = Time::getTime();
    as.assembleFile(argv[1], output.c_str(), 0);
    f64 duration = Time::getTime() - start;

    printf("Assembly '%s' tooks %fs\n", argv[1], duration);

    Time::shutdown();
    return 0;
}
