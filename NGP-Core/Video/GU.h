/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

static constexpr u32 DisplayWidth = 640;
static constexpr u32 DisplayHeight = 480;

struct GPUDriver
{
    void(*initialize)();
    void(*shutdown)();

    void(*present)();

    VirtualAddress(*create_framebuffer)();
    void(*update_framebuffer)(void*);
};

struct GU
{
    static constexpr i32 MaxDeviceScreenWidth = 256;
    static constexpr i32 MaxDeviceScreenHeight = 144;

    enum DriverApi
    {
        D3D12 = 0,
        VGPU = 1,
    };

    static void initialize(DriverApi api);
    static void shutdown();

    static void present();
};
