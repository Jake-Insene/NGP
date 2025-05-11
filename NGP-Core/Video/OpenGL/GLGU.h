/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Video/GU.h"


struct GLGU
{

    static GU::GUDriver get_driver();

    static void initialize();
    static void shutdown();

    static void present(bool vsync);

    static void display_set(VirtualAddress vva, i32 width, i32 height, GU::GUDisplayFormat display_format);

    static Bus::CheckAddressResult check_vram_address(VirtualAddress vva);

    static VirtualAddress create_framebuffer();
    static void update_framebuffer(void* fbaddr);

};