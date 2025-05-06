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
    static inline u8* vram = nullptr;
    static inline GPUDriver internal_driver = {};

    static GPUDriver get_driver();

    static void initialize();
    static void shutdown();

    static void present();

    static void set_pixel(i32 x, i32 y, Color color);

    static void draw_line(Vector2 start, Vector2 end, Color color);
    static void draw_triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color);
};
