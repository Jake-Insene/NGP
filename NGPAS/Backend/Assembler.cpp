#include "Assembler.h"
#include "Frontend/Parser.h"
#include "Backend/Function.h"

void Assembler::assemble_file(const char* file_path)
{
    program = parser.parse_file(file_path);
    assemble_program();
}

void Assembler::assemble_program()
{
}
