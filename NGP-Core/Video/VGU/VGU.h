/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/JobQueue.h"
#include "Video/GU.h"
#include "Video/Math.h"

#include <mutex>


struct VGU
{
    static constexpr PhysicalAddress VRamAddress = 0x2'0000'0000;

    enum QueueSignal
    {
        QUEUE_SIGNAL_IDLE = 0,
        QUEUE_SIGNAL_RUN = 1,
    };

    struct Queue
    {
        u8 id;
        u8 priority;
        VirtualAddress cmd_list;
        // In words
        Word cmd_len;

        // Implementation data
        QueueSignal signal;
    };

    struct TMU
    {
        VirtualAddress texture_address;
        u8 min_filter;
        u8 mag_filter;

        PhysicalAddress cache_texture_address;
    };

    struct GUState
    {
        GU::GUDriver internal_driver;

        Word* vram;
        Word* display_address;

        i32 width;
        i32 height;
        Display::DisplayFormat display_format;
        PhysicalAddress fb;

        Queue queues[GUDevice::GU_QUEUE_INDEX_MAX];
        std::mutex sync_mutex;
        std::mutex queue_mutex;

        bool present_requested;
        bool irq_pending;
    };

    static inline GUState state;

    static GU::GUDriver get_driver();

    static void initialize();
    static void shutdown();

    static void present(bool vsync);
    static void request_present();

    static void display_set_config(i32 width, i32 height, Display::DisplayFormat display_format);
    static void display_set_address(VirtualAddress vva);

    static void queue_execute(u8 index, u8 priority, VirtualAddress cmd_list, Word cmd_len);
    static void queue_dispatch();

    static void dma_send(VirtualAddress dest, VirtualAddress src, Word len, Word flags);

    static Bus::CheckAddressResult check_vram_address(VirtualAddress vva);

    // Internal functions
    static void set_present_requested(bool requested);

    static void set_pixel(i32 x, i32 y, Color color);

    // DMA

    // Queue
    static void queue_execute_cmd(Queue* queue);

    // Drawing
    static void fill_rectangle(Color color, i32 x, i32 y, i32 w, i32 h);
};
