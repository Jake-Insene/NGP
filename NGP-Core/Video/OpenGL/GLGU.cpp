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

        .display_set_config = &GLGU::display_set_config,
        .display_set_address = &GLGU::display_set_address,

        .check_vram_address = &GLGU::check_vram_address,

        .create_framebuffer = &GLGU::create_framebuffer,
        .update_framebuffer = &GLGU::update_framebuffer,
    };
}

void GLGU::initialize() {}
void GLGU::shutdown() {}

void GLGU::present(bool vsync) {}

void GLGU::display_set_config(i32 width, i32 height, IO::DisplayFormat display_format) {}
void GLGU::display_set_address(VirtualAddress vva) {}

Bus::CheckAddressResult GLGU::check_vram_address(VirtualAddress vva) { return Bus::ValidAddress; }

PhysicalAddress GLGU::create_framebuffer(i32, i32) { return PhysicalAddress(0); }
void GLGU::update_framebuffer(PhysicalAddress, void*) {}
