#include <Header.h>
#include <string>
#include "Backend/Assembler.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        return INVALID_ARGUMENTS;
    }

    Assembler as{};

    std::string output = { argv[1] };
    output = output.substr(0, output.find_last_of(".") + 1);
    output += "ngp";

    return as.assemble_file(argv[1], output.c_str());
}
