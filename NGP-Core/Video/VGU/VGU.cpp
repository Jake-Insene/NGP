/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/VGU/VGU.h"

#include "Memory/Bus.h"
#include "Platform/OS.h"
#include "Video/Window.h"
#include "Video/OpenGL/GLGU.h"

#if defined(_WIN32)
#include "Video/D3D11/D3D11GU.h"
#include "Video/D3D12/D3D12GU.h"
#endif

#include <cstdlib>
#include <algorithm>

GU::GUDriver get_internal_driver(GU::DriverApi api)
{
    switch (api)
    {
    case GU::NONE:
        return {};
    case GU::D3D11:
#if defined(_WIN32)
        return D3D11GU::get_driver();
#else
        return {};
#endif
    case GU::D3D12:
#if defined(_WIN32)
        return D3D12GU::get_driver();
#else
        return {};
#endif
    case GU::OPENGL:
        return GLGU::get_driver();
    case GU::VGU:
        return {};
    }

    return {};
}

static constexpr usize VRamAddress = 0x2'0000'0000;


f32 signed_triangle_area(Vector2 p1, Vector2 p2, Vector2 p3)
{
    return 0.5f * ((p2.y - p1.y) * (p2.x + p1.x) + (p3.y - p2.y) * (p3.x + p2.x) + (p1.y - p3.y) * (p1.x + p3.x));
}

GU::GUDriver VGPU::get_driver()
{
    return GU::GUDriver
    {
        .initialize = &VGPU::initialize,
        .shutdown = &VGPU::shutdown,
        
        .present = &VGPU::present,

        .display_set = &VGPU::display_set,

        .check_vram_address = &VGPU::check_vram_address,
    };
}

void VGPU::initialize()
{
    // VRAM
    state.vram = (Word*)OS::allocate_virtual_memory((void*)VRamAddress, Bus::get_vram_size(), OS::PAGE_READ_WRITE);

    state.present_requested = false;

    // Internal driver
    state.internal_driver = get_internal_driver(GU::OPENGL);
    state.internal_driver.initialize();

    VirtualAddress addr = state.internal_driver.create_framebuffer();
}

void VGPU::shutdown()
{
    OS::deallocate_virtual_memory((void*)VRamAddress);

    state.internal_driver.shutdown();
}

void VGPU::present(bool vsync)
{
    if (state.present_requested)
    {
        state.internal_driver.present(vsync);
    }
}

void VGPU::display_set(VirtualAddress vva, i32 width, i32 height, GU::GUDisplayFormat display_format)
{
    state.display_address = (Word*)((u8*)state.vram + vva);
}

Bus::CheckAddressResult VGPU::check_vram_address(VirtualAddress vva)
{
    return vva < Bus::get_vram_size() ? Bus::ValidVirtualAddress : Bus::InvalidVirtualAddress;
}

void VGPU::set_pixel(i32 x, i32 y, Color color)
{
    state.display_address[x + y * GU::MaxDeviceScreenWidth] = *(u32*)&color;
}

void VGPU::draw_line(Vector2 start, Vector2 end, Color color)
{
    f32 x0 = start.x;
    f32 x1 = end.x;
    f32 y0 = start.y;
    f32 y1 = end.y;

    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    f32 dx = x1 - x0;
    f32 dy = y1 - y0;
    f32 derror2 = std::abs(dy) * 2;
    f32 error2 = 0;
    f32 y = y0;
    for (f32 x = x0; x <= x1; x++)
    {
        if (steep)
        {
            set_pixel((i32)y, (i32)x, color);
        }
        else
        {
            set_pixel((i32)x, (i32)y, color);
        }
        error2 += derror2;
        if (error2 > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void VGPU::draw_triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color)
{
    draw_line(p1, p2, color);
    draw_line(p2, p3, color);
    draw_line(p3, p1, color);
}

void VGPU::draw_fill_triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color)
{
    // bounding box for the triangle
    f32 bbminx = std::min(std::min(p1.x, p2.x), p3.x); 
    // defined by its top left and bottom right corners
    f32 bbminy = std::min(std::min(p1.y, p2.y), p3.y);
    f32 bbmaxx = std::max(std::max(p1.x, p2.x), p3.x);
    f32 bbmaxy = std::max(std::max(p1.y, p2.y), p3.y);
    f32 total_area = signed_triangle_area(p1, p2, p3);

    //#pragma omp parallel for
    for (f32 x = bbminx; x <= bbmaxx; x++)
    {
        for (f32 y = bbminy; y <= bbmaxy; y++)
        {
            f32 alpha = signed_triangle_area(Vector2(x, y), p2, p3) / total_area;
            f32 beta = signed_triangle_area(Vector2(x, y), p3, p1) / total_area;
            f32 gamma = signed_triangle_area(Vector2(x, y), p1, p2) / total_area;
            // negative barycentric coordinate => the pixel is outside the triangle
            if (alpha < 0 || beta < 0 || gamma < 0)
                continue;
            set_pixel((i32)x, (i32)y, color);
        }
    }
}
