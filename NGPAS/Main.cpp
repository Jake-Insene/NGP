#include <Header.h>
#include "Assembler.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        return INVALID_ARGUMENTS;
    }

    assemble_file(argv[1]);

    return 0;
}
