#pragma once
#include "Header.h"

void initialize_display();

struct Display {
    Display(i32 width, i32 height);
    ~Display();

    void update();

    void clear(u32 rgba);
    void draw_text(i32 x, i32 y, char* text, u32 count);

    u32 width;
    u32 height;

    bool is_open;
    void* handle;
    void* native_graphics_context;
};
