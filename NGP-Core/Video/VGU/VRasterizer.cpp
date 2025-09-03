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
        if (position.x < 0 || position.x > state.draw_buffer.size.x
            || position.y < 0 || position.y > state.draw_buffer.size.y)
            return;

        Word* pixels = (Word*)state.draw_buffer.address;
        pixels[(position.y * state.draw_buffer.size.x) + position.x] = rgb.rgba;
    }
    break;
    default:
        break;
    }
}

void VRasterizer::pixel_line(Vector2I position, i32 width, Color rgb)
{
    switch (state.put_pixel)
    {
    case PP_RGBA8:
    {
        if (position.x < 0 || position.x + width > state.draw_buffer.size.x
            || position.y < 0 || position.y >= state.draw_buffer.size.y)
            return;

        Word* pixels = ((Word*)state.draw_buffer.address) + state.draw_buffer.size.x * position.y;
        for (i32 x = position.x; x < position.x + width; x++)
        {
            pixels[x] = rgb.rgba;
        }
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

Vector2 move_linear(Vector2 current, Vector2 target, float speed, float dt)
{
    Vector2 direction = target - current;
    f32 distance = direction.lenght();

    if (distance < 1e-6f)
        return target;

    direction /= distance;
    float step = speed * dt;

    if (step >= distance)
        return target;

    return current + direction * step;
}

float approach(float current, float target, float delta)
{
    if (current < target)
    {
        current += delta;
        if (current > target) current = target;
    }
    else if (current > target)
    {
        current -= delta;
        if (current < target) current = target;
    }
    return current;
}

void VRasterizer::line(VertexColor p0, VertexColor p1)
{
    i32 x0 = (i32)std::round(p0.position.x);
    i32 y0 = (i32)std::round(p0.position.y);
    i32 x1 = (i32)std::round(p1.position.x);
    i32 y1 = (i32)std::round(p1.position.y);

    i32 dx = std::abs(x1 - x0);
    i32 dy = -std::abs(y1 - y0);
    i32 sx = (x0 < x1) ? 1 : -1;
    i32 sy = (y0 < y1) ? 1 : -1;
    i32 err = dx + dy;

    while (true)
    {
        pixel(Vector2I(x0, y0), p0.color);

        if (x0 == x1 && y0 == y1) break;
        i32 e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void VRasterizer::rect(Vector2 position, Vector2 size, Color color)
{
    VertexColor v0 = VertexColor(position, color);
    VertexColor v1 = VertexColor(position + Vector2(size.x, 0), color);
    VertexColor v2 = VertexColor(position + Vector2(0, size.y), color);
    VertexColor v3 = VertexColor(position + size, color);

    line(v0, v1);
    line(v1, v2);
    line(v2, v3);
    line(v3, v1);
}

void VRasterizer::fill_rect(Vector2 position, Vector2 size, Color color)
{
    VGU::VFramebuffer& fb = VGU::get_current_framebuffer();

    for (i32 y1 = position.y; y1 < position.y + size.y; y1++)
    {
        pixel_line(Vector2I(position.x, y1), size.x, color);
    }
}

void VRasterizer::triangle(VertexColor v0, VertexColor v1, VertexColor v2)
{
    line(v0, v1);
    line(v1, v2);
    line(v2, v0);
}

Color shade(VRasterizer::VertexColor v0, VRasterizer::VertexColor v1, 
    VRasterizer::VertexColor v2, int w0, int w1, int w2, int denom)
{
    Color output = {};
    output.r = (int(v0.color.r) * w0 + int(v1.color.r) * w1 + int(v2.color.r) * w2) / denom;
    output.g = (int(v0.color.g) * w0 + int(v1.color.g) * w1 + int(v2.color.g) * w2) / denom;
    output.b = (int(v0.color.b) * w0 + int(v1.color.b) * w1 + int(v2.color.b) * w2) / denom;
    output.a = 255;
    return output;
}

void VRasterizer::fill_triangle(VertexColor v0, VertexColor v1, VertexColor v2)
{
    int x0 = (int)std::round(v0.position.x), y0 = (int)std::round(v0.position.y);
    int x1 = (int)std::round(v1.position.x), y1 = (int)std::round(v1.position.y);
    int x2 = (int)std::round(v2.position.x), y2 = (int)std::round(v2.position.y);

    // Bounding box (clipped to draw buffer)
    int minX = std::max(0, std::min({ x0, x1, x2 }));
    int maxX = std::min(state.draw_buffer.size.x - 1, std::max({ x0, x1, x2 }));
    int minY = std::max(0, std::min({ y0, y1, y2 }));
    int maxY = std::min(state.draw_buffer.size.y - 1, std::max({ y0, y1, y2 }));
    if (minX > maxX || minY > maxY) return;

    // Edge setup
    int A01 = y0 - y1, B01 = x1 - x0;
    int A12 = y1 - y2, B12 = x2 - x1;
    int A20 = y2 - y0, B20 = x0 - x2;

    // Triangle area (twice the area). If 0 -> degenerate.
    int denom = A12 * (x0 - x2) + B12 * (y0 - y2);
    if (denom == 0) return;

    // Normalize orientation so denom > 0 (flip everything if needed)
    if (denom < 0)
    {
        denom = -denom;
        A01 = -A01; B01 = -B01;
        A12 = -A12; B12 = -B12;
        A20 = -A20; B20 = -B20;
    }

    // Edge function values at (minX, minY)
    int w0_row = A12 * (minX - x2) + B12 * (minY - y2);
    int w1_row = A20 * (minX - x0) + B20 * (minY - y0);
    int w2_row = A01 * (minX - x1) + B01 * (minY - y1);

    // Rasterize (>= 0 works now that denom is positive)
    for (int y = minY; y <= maxY; ++y)
    {
        int w0 = w0_row, w1 = w1_row, w2 = w2_row;

        for (int x = minX; x <= maxX; ++x)
        {
            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                pixel(Vector2I(x, y), shade(v0, v1, v2, w0, w1, w2, denom));
            }
            w0 += A12;  // step E12 by +A12 when x++
            w1 += A20;  // step E20 by +A20
            w2 += A01;  // step E01 by +A01
        }

        w0_row += B12; // step by +B12 when y++
        w1_row += B20;
        w2_row += B01;
    }
}

