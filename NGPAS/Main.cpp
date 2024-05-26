#include <Header.h>
#include "Backend/Assembler.h"
#include <string>

int main(int argc, char** argv) {
    if (argc != 2) {
        return INVALID_ARGUMENTS;
    }

    Assembler as{};

    std::string output = {argv[1]};
    output = output.substr(0, output.find_last_of("s"));
    output += "ngp";
    as.assemble_file(argv[1], output.c_str());

    return 0;
}
