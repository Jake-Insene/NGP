#include "Core/SystemCalls.h"
#include "Core/VirtualMachine.h"

void draw_text(VirtualMachine* vm) {
    vm->display.draw_text(vm->ireg[0], vm->ireg[1], (char*)vm->ram + vm->ureg[2], vm->ureg[3]);
}
