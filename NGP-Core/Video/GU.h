/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

#include "IO/Display/Display.h"
#include "Memory/Bus.h"


struct GU
{
    static constexpr i32 MaxDeviceScreenWidth = 256;
    static constexpr i32 MaxDeviceScreenHeight = 144;

    enum GUPositionFormat
    {
        GU_VB_XYF32 = 0,
        GU_VB_XYZF32 = 1,
        GU_VB_XYI32 = 2,
        GU_VB_XYZI32 = 3,
    };

    enum GUUVFormat
    {
        GU_VB_UVF32 = 0,
        GU_VB_UVWF32 = 1,
        GU_VB_UVI32 = 2,
        GU_VB_UVWI32 = 3,
    };

    enum GUColorFormat
    {
        GU_VB_RGBF32 = 0,
        GU_VB_RGBAF32 = 1,
        GU_VB_RGB8 = 2,
        GU_VB_RGBA8 = 3,
    };

    // Each command is 16 bytes size
    enum GUCommand
    {
        GU_COMMAND_END = 0,
        // [32 - 34] Position FMT
        // [35 - 37] UV FMT
        // [38 - 40] Color FMT
        GU_COMMAND_SET_VS_FMT = 1,

        // [32 - 39] 8 bits for red component 
        // [40 - 47] 8 bits for green component 
        // [48 - 55] 8 bits for blue component 
        // [56 - 63] 8 bits for alpha component 
        GU_COMMAND_CLEAR = 2,
    };

    struct GUDriver
    {
        void(*initialize)();
        void(*shutdown)();

        void(*present)(bool);
        void(*request_present)();

        void(*display_set_config)(i32, i32, IO::DisplayFormat);
        void(*display_set_address)(VirtualAddress);

        Bus::CheckAddressResult(*check_vram_address)(VirtualAddress);

        VirtualAddress(*create_framebuffer)(i32, i32);
        void(*update_framebuffer)(VirtualAddress, void*);
    };

    static inline GUDriver main_driver = {};
#define VTFUNCDEF(name) static void name() { main_driver.name(); }
#define VTFUNCDEFARG1(name, t1) static void name(t1 arg1) { main_driver.name(arg1); }
#define VTFUNCDEFARG2(name, t1, t2) static void name(t1 arg1, t2 arg2) { main_driver.name(arg1, arg2); }
#define VTFUNCDEFARG3(name, t1, t2, t3) \
    static void name(t1 arg1, t2 arg2, t3 arg3) { main_driver.name(arg1, arg2, arg3); }
#define VTFUNCDEFARG4(name, t1, t2, t3, t4) \
    static void name(t1 arg1, t2 arg2, t3 arg3, t4 arg4) { main_driver.name(arg1, arg2, arg3, arg4); }
#define VTFUNCDEFRET(ret, name) static ret name() { return main_driver.name(); }
#define VTFUNCDEFRETARG1(ret, name, t1) static ret name(t1 arg1) { return main_driver.name(arg1); }

    enum DriverApi
    {
        NONE = 0,
        D3D12,
        D3D11,
        OPENGL,
        VGU,
    };

    static void initialize(DriverApi api);

    VTFUNCDEF(shutdown);
    VTFUNCDEFARG1(present, bool);
    VTFUNCDEF(request_present);

    VTFUNCDEFARG3(display_set_config, i32, i32, IO::DisplayFormat);
    VTFUNCDEFARG1(display_set_address, VirtualAddress);

    VTFUNCDEFRETARG1(Bus::CheckAddressResult, check_vram_address, VirtualAddress);
};
