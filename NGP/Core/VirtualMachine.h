#pragma once
#include "FileFormat/Room.h"
#include "Graphics/Display.h"

#define SP_INDEX 31

#define DISPLAY_WIDTH 620
#define DISPLAY_HEIGHT 512

struct VirtualMachine {
    VirtualMachine();
    ~VirtualMachine();

    i32 load(const char* room_path);
    void start();

    void push(u32 word);
    u32 pop();

    union {
        u32* ureg;
        i32* ireg;
    };
    union {
        f32* dreg;
        f32* sreg;
    };

    RoomHeader header;

    Display display;

    u8* ram;
    u32 pc;
};
