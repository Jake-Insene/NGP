#pragma once
#include "FileFormat/Room.h"
#include "Graphics/Display.h"

#define SP_INDEX 31

struct VirtualMachine {
    VirtualMachine();
    ~VirtualMachine();

    i32 load(const char* room_path);
    void start();

    union {
        u32 reg[32] = {};
        i32 ireg[32];
    };

    RoomHeader header;

    Display display;

    u8* sp;
    u8* data;
    u32 pc;
};
