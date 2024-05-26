#include "Core/SystemCalls.h"
#include "Core/VirtualMachine.h"

void draw_text(VirtualMachine* vm) {
    vm->display.draw_text(0, 0, (char*)vm->data + vm->reg[0], vm->reg[1]);
}
