/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/vGPU/VGPU.h"

#include "Memory/Bus.h"
#include "Platform/OS.h"
#include "Video/Window.h"

#if defined(_WIN32)
#include "Video/D3D12/D3D12GU.h"
#endif

GPUDriver get_internal_driver()
{
#if defined(_WIN32)
    return D3D12GU::get_driver();
#endif
}

static constexpr usize VRamAddress = 0x2'0000'0000;

GPUDriver VGPU::get_driver()
{
    return GPUDriver
    {
        .initialize = &VGPU::initialize,
        .shutdown = &VGPU::shutdown,
        
        .present = &VGPU::present,
    };
}

void VGPU::initialize()
{
    // VRAM
    vram = (u8*)OS::allocate_virtual_memory((void*)VRamAddress, Bus::get_vram_size(), OS::PAGE_READ_WRITE);

    // Internal driver
    internal_driver = get_internal_driver();
    internal_driver.initialize();

    VirtualAddress addr = internal_driver.create_framebuffer();
}

void VGPU::shutdown()
{
    OS::deallocate_virtual_memory((void*)VRamAddress);

    internal_driver.shutdown();
}

void VGPU::present()
{
    for (u32 x = 0; x < GU::MaxDeviceScreenWidth; x++)
    {
        for (u32 y = 0; y < GU::MaxDeviceScreenHeight; y++)
        {
            ((u32*)vram)[x + y * GU::MaxDeviceScreenWidth] = 0xFF00'00FF;
        }
    }

    draw_line({ 100, 100 }, { 50, 50 }, Color(255, 255, 0, 255));
    draw_triangle({ 0, 0 }, { 20, 0}, {20, 20}, Color(0, 0, 0, 255));

    //internal_driver.update_framebuffer(vram);
    internal_driver.present();
}

void VGPU::set_pixel(i32 x, i32 y, Color color)
{
    ((u32*)vram)[x + y * GU::MaxDeviceScreenWidth] = *(u32*)&color;
}

void VGPU::draw_line(Vector2 start, Vector2 end, Color color)
{
    for (f32 t = 0.f; t < 1.f; t += 0.01f)
    {
        f32 x = start.x + (end.x - start.x) * t;
        f32 y = start.y + (end.y - start.y) * t;

        set_pixel((i32)x, (i32)y, color);
    }
}

void VGPU::draw_triangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color)
{
    draw_line(p1, p2, color);
    draw_line(p2, p3, color);
    draw_line(p3, p1, color);
}
