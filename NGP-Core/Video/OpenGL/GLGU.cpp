/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/OpenGL/GLGU.h"


GU::GUDriver GLGU::get_driver()
{
    return GU::GUDriver
    {
        .initialize = &GLGU::initialize,
        .shutdown = &GLGU::shutdown,

        .present = &GLGU::present,

        .display_set = &GLGU::display_set,

        .check_vram_address = &GLGU::check_vram_address,

        .create_framebuffer = &GLGU::create_framebuffer,
        .update_framebuffer = &GLGU::update_framebuffer,
    };
}

void GLGU::initialize() {}
void GLGU::shutdown() {}

void GLGU::present(bool vsync) {}

void GLGU::display_set(VirtualAddress vva, i32 width, i32 height, GU::GUDisplayFormat display_format) {}

Bus::CheckAddressResult GLGU::check_vram_address(VirtualAddress vva) { return Bus::ValidVirtualAddress; }

VirtualAddress GLGU::create_framebuffer() { return VirtualAddress(0); }
void GLGU::update_framebuffer(void* fbaddr) {}
