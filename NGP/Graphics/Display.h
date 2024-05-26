#pragma once
#include "Header.h"

void initialize_display();

struct Display {
    Display(i32 width, i32 height);
    ~Display();

    void update();

    void draw_text(i32 x, i32 y, char* text, u32 count);

    bool is_open;
    void* handle;
    void* native_graphics_context;
};
