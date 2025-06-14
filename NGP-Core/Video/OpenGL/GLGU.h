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

    static void display_set_config(i32 width, i32 height, Display::DisplayFormat display_format);
    static void display_set_address(VirtualAddress vva);

    static Bus::CheckAddressResult check_vram_address(VirtualAddress vva);

    static PhysicalAddress create_framebuffer(i32, i32);
    static void update_framebuffer(PhysicalAddress, void* fbaddr);

};