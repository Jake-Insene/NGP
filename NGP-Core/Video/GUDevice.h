/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

#include "IO/Display/Display.h"
#include "IO/GU/GU.h"
#include "Memory/Bus.h"


struct GUDevice
{
    static constexpr i32 MaxDeviceScreenWidth = 256;
    static constexpr i32 MaxDeviceScreenHeight = 144;

    struct GUDriver
    {
        void(*initialize)();
        void(*shutdown)();

        void(*present_framebuffer)(PhysicalAddress, bool);
        void(*present)(bool);
        void(*request_present)();

        void(*display_set_config)(i32, i32, Display::DisplayFormat);
        void(*display_set_address)(VirtualAddress);

        void(*queue_execute)(u8, u8, VirtualAddress, Word);
        void(*queue_dispatch)();

        void(*dma_send)(VirtualAddress, VirtualAddress, Word, Word);

        Bus::CheckAddressResult(*check_vram_address)(VirtualAddress);

        PhysicalAddress(*create_framebuffer)(i32, i32);
        void(*update_framebuffer)(PhysicalAddress, void*);
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

    VTFUNCDEFARG3(display_set_config, i32, i32, Display::DisplayFormat);
    VTFUNCDEFARG1(display_set_address, VirtualAddress);

    VTFUNCDEF(queue_dispatch);
    VTFUNCDEFARG4(queue_execute, u8, u8, VirtualAddress, Word);

    VTFUNCDEFARG4(dma_send, VirtualAddress, VirtualAddress, Word, Word);

    VTFUNCDEFRETARG1(Bus::CheckAddressResult, check_vram_address, VirtualAddress);
};
