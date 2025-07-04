/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/JobQueue.h"
#include "Video/VGU/VGUQueue.h"

#include <mutex>
#include <vector>


struct VGU
{
    static constexpr PhysicalAddress VRamAddress = 0x3'0000'0000;
    static constexpr i32 InvalidFB = -1;

    struct TMU
    {
        VirtualAddress texture_address;
        PhysicalAddress cache_texture_address;

        u8 flags;
        u8 texture_format;
        u16 width;
        u16 height;
        u8 min_filter;
        u8 mag_filter;
    };

    struct VFramebuffer
    {
        i32 width;
        i32 height;
        Display::DisplayFormat display_format;
        PhysicalAddress framebuffer;
    };

    struct GUState
    {
        GUDevice::GUDriver internal_driver;

        Word vram_size;
        Word* vram;
        Word* display_address;

        std::vector<VFramebuffer> cached_framebuffers;
        i32 current_fb;

        VGUQueue queue;
        TMU texture_units[16];
        std::mutex sync_mutex;
        std::mutex queue_mutex;

        bool present_requested;
        bool irq_pending;
    };

    static inline GUState state;

    [[nodiscard]] static GUState& get_state() { return state; }
    [[nodiscard]] static VFramebuffer& get_current_framebuffer()
    {
        return state.cached_framebuffers[state.current_fb];
    }

    static GUDevice::GUDriver get_driver();

    static void initialize(Word requested_vram_size);
    static void shutdown();

    static void present(bool vsync);
    static void request_present();

    static void display_set_config(i32 width, i32 height, Display::DisplayFormat display_format);
    static void display_set_address(VirtualAddress vva);

    static void queue_execute(VirtualAddress cmd_list, Word cmd_len);
    static void queue_dispatch();

    static void dma_send(VirtualAddress dest, VirtualAddress src, Word len, Word flags);

    static Bus::CheckAddressResult check_vram_address(VirtualAddress vva);

    // Internal functions
    static PhysicalAddress get_physical_vram_address(VirtualAddress vaddress);
    static void set_present_requested(bool requested);
};
