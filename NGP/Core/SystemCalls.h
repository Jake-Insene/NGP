#pragma once
#include "Header.h"

struct VirtualMachine;

using SystemCallFn = void(*)(VirtualMachine* vm);

void draw_text(VirtualMachine* vm);

inline SystemCallFn system_calls[] = {
    &draw_text,
};
