#include <Core/Header.h>
#include <Platform/Time.h>
#include "Backend/Assembler.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        return INVALID_ARGUMENTS;
    }

    Assembler as{};
    Time::initialize();

    std::string output = { argv[1] };
    output = output.substr(0, output.find_last_of('.') + 1);
    output += "ngp";
    
    auto start = Time::get_time();
    i32 result = as.assemble_file(argv[1], output.c_str(), 0);
    f64 duration = Time::get_time() - start;

    printf("Assembly '%s' tooks %fs\n", argv[1], duration);

    Time::shutdown();
    return result;
}
