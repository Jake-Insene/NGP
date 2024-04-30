#include <Header.h>
#include "Backend/Assembler.h"

int main(int argc, char** argv) {
    /*if (argc != 2) {
        return INVALID_ARGUMENTS;
    }*/

    Assembler as{};
    as.assemble_file("Examples/main.ngps", "Examples/main.ngpe");

    return 0;
}
