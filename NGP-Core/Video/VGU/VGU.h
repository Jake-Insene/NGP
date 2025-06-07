/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Video/GU.h"
#include "Video/Math.h"

#include <mutex>


struct VGPU
{
    struct GPUState
    {
        Word* vram;
        bool present_requested;

        Word* display_address;

        GU::GUDriver internal_driver;

        i32 width;
        i32 height;
        IO::DisplayFormat display_format;
        PhysicalAddress fb;

        std::mutex sync_mutex;
    };

    static inline GPUState state;

    static GU::GUDriver get_driver();

    static void initialize();
    static void shutdown();

    static void present(bool vsync);
    static void request_present();

    static void display_set_config(i32 width, i32 height, IO::DisplayFormat display_format);
    static void display_set_address(VirtualAddress vva);

    static Bus::CheckAddressResult check_vram_address(VirtualAddress vva);

    static void set_pixel(i32 x, i32 y, Color color);

    static void draw_line(Vector2 start, Vector2 end, Color color);
    static void draw_triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color);
    static void draw_fill_triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color);
};
