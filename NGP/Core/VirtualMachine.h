#pragma once
#include "FileFormat/Rom.h"
#include "Graphics/Display.h"
#include <vector>

#define SP_INDEX 31

#define DISPLAY_WIDTH 620
#define DISPLAY_HEIGHT 480

struct VirtualMachine {
    VirtualMachine();
    ~VirtualMachine();

    i32 load(const char* room_path);
    void start();

    void push(u32 word);
    u32 pop();

    RomHeader* header;

    Display display;

    u32 pc;
    u32 inst;
    bool is_halt;
};
