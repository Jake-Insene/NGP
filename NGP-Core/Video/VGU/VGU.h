/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Video/GU.h"
#include "Video/Math.h"


struct VGPU
{
    struct GPUState
    {
        Word* vram = nullptr;
        bool present_requested = false;

        Word* display_address = nullptr;
        
        GU::GUDriver internal_driver = {};
    };

    static inline GPUState state = {};

    static GU::GUDriver get_driver();

    static void initialize();
    static void shutdown();

    static void present(bool vsync);

    static void display_set(VirtualAddress vva, i32 width, i32 height, GU::GUDisplayFormat display_format);

    static Bus::CheckAddressResult check_vram_address(VirtualAddress vva);

    static void set_pixel(i32 x, i32 y, Color color);

    static void draw_line(Vector2 start, Vector2 end, Color color);
    static void draw_triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color);
    static void draw_fill_triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color);
};
