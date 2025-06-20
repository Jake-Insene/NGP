/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/VGU/VRasterizer.h"
#include "Video/VGU/VGU.h"


void VRasterizer::initialize()
{
    state.draw_buffer.address = 0;
    state.draw_buffer.size = Vector2I();
    state.draw_buffer.offset = Vector2I();

    state.put_pixel = PP_NONE;
}

void VRasterizer::shutdown() {}

void VRasterizer::reset_state()
{
    state.draw_buffer.address = 0;
    state.draw_buffer.size = Vector2I();
    state.draw_buffer.offset = Vector2I();

    state.put_pixel = PP_NONE;
}

void VRasterizer::pixel(Vector2I position, Color rgb)
{
    switch (state.put_pixel)
    {
    case PP_RGBA8:
    {
        if (position.x < 0 || position.x >= state.draw_buffer.size.x
            || position.x < 0 || position.x >= state.draw_buffer.size.y)
            return;

        Word* wpixels = (Word*)state.draw_buffer.address;
        wpixels[(position.y * state.draw_buffer.size.y) + position.x] = rgb.rgba;
    }
    break;
    default:
        break;
    }
}

void VRasterizer::set_draw_buffer(PhysicalAddress address, Vector2I size, 
    Vector2I offset, GU::TextureFormat format)
{
    state.draw_buffer.address = address;
    state.draw_buffer.size = size;
    state.draw_buffer.offset = offset;
    state.draw_buffer.format = format;

    state.put_pixel = PP_RGBA8;
}

void VRasterizer::fill_rect(Vector2 position, Vector2 size, Color color)
{
    VGU::VFramebuffer& fb = VGU::get_current_framebuffer();

    for (i32 y1 = position.y; y1 < position.y + size.y; y1++)
    {
        for (i32 x1 = position.x; x1 < position.x + size.x; x1++)
        {
            pixel(Vector2I(x1, y1), color);
        }
    }
}

void VRasterizer::fill_triangle(VertexColor v0, VertexColor v1, VertexColor v2)
{
}
