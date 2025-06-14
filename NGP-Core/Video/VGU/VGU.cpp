/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/VGU/VGU.h"

#include "Memory/Bus.h"
#include "IO/DMA/DMA.h"
#include "IO/GUDevice/GUDevice.h"
#include "Platform/OS.h"
#include "Video/OpenGL/GLGU.h"


#if defined(_WIN32)
#include "Video/D3D11/D3D11GU.h"
#include "Video/D3D12/D3D12GU.h"
#endif

#include <cstdlib>
#include <algorithm>

#define VGPU_LOGGER(...) { printf("VGPU: "); printf(__VA_ARGS__); putchar('\n'); }

static inline void queue_rutine(VGU::Queue* queue)
{

}


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


GU::GUDriver VGU::get_driver()
{
    return GU::GUDriver
    {
        .initialize = &VGU::initialize,
        .shutdown = &VGU::shutdown,
        
        .present = &VGU::present,
        .request_present = &VGU::request_present,

        .display_set_config = &VGU::display_set_config,
        .display_set_address = &VGU::display_set_address,

        .queue_execute = &VGU::queue_execute,

        .dma_send = &VGU::dma_send,

        .check_vram_address = &VGU::check_vram_address,
    };
}

void VGU::initialize()
{
    // VRAM
    state.vram = (Word*)OS::allocate_virtual_memory((void*)VRamAddress, Bus::get_vram_size(), OS::PAGE_READ_WRITE);

    state.present_requested = false;
    state.irq_pending = false;
    state.queue_requested = false;

    // Internal driver
    state.internal_driver = get_internal_driver(GU::D3D12);
    state.internal_driver.initialize();

    for (u32 i = 0; i < GUDevice::GU_QUEUE_INDEX_MAX; i++)
    {
        state.queues[i].id = i;
        state.queues[i].priority = GUDevice::GU_QUEUE_PRIORITY_LOW;
        state.queues[i].cmd_list = VirtualAddress(0);
        state.queues[i].cmd_len = 0;
    }
}

void VGU::shutdown()
{
    OS::deallocate_virtual_memory((void*)VRamAddress);

    state.internal_driver.shutdown();
}

void VGU::present(bool vsync)
{
    state.sync_mutex.lock();
    bool present_requested = state.present_requested;
    bool queue_requested = state.queue_requested;
    state.sync_mutex.unlock();

    if (queue_requested)
    {
        for (auto& queue : state.queues)
        {
            queue_execute_cmd(&queue);
        }

        state.sync_mutex.lock();
        state.queue_requested = false;
        state.sync_mutex.unlock();
    }

    if (state.display_address == nullptr || state.width == 0 || state.height == 0 || !present_requested)
        return;

    state.internal_driver.update_framebuffer(state.fb, state.display_address);
    state.internal_driver.present_framebuffer(state.fb, vsync);

    state.sync_mutex.lock();
    state.present_requested = false;
    state.sync_mutex.unlock();

    VGPU_LOGGER("Present: FB=0x%016llX", state.fb);
}

void VGU::request_present()
{
    state.sync_mutex.lock();
    state.present_requested = true;
    state.sync_mutex.unlock();
}

void VGU::display_set_config(i32 width, i32 height, Display::DisplayFormat display_format)
{
    state.fb = state.internal_driver.create_framebuffer(width, height);

    state.width = width;
    state.height = height;
    state.display_format = display_format;
}

void VGU::display_set_address(VirtualAddress vva)
{
    state.display_address = (Word*)((u8*)state.vram + vva);
}

void VGU::queue_execute(u8 index, u8 priority, VirtualAddress cmd_list, Word cmd_len)
{
    Word* cmd_words = (Word*)Bus::get_physical_addr(cmd_list);

    if ((cmd_len & 0x3) != 0)
    {
        VGPU_LOGGER("Unaligned Command List Base Address");
        return;
    }

    if (cmd_len == 0)
        return;

    if (index >= GUDevice::GU_QUEUE_INDEX_MAX)
    {
        VGPU_LOGGER("Invalid Queue Index");
        return;
    }

    Queue* queue = &state.queues[index];
    queue->queue_mutex.lock();
    queue->priority = priority;
    queue->cmd_list = cmd_list;
    queue->cmd_len = cmd_len;
    queue->queue_mutex.unlock();

    // Non low priority start execution immediately
    queue_send_signal(queue, QUEUE_START);
}

void VGU::dma_send(VirtualAddress dest, VirtualAddress src, Word len, Word flags)
{
    PhysicalAddress dest_mem = Bus::get_physical_addr(dest);
    PhysicalAddress src_mem = Bus::get_physical_addr(src);

    switch ((flags >> 3) & 0x7F)
    {
    case DMA::GU_DMA_TEXTURE_R8:
        while (len--)
        {
            *((u8*)dest_mem) = *((u8*)src_mem);
            dest_mem++;
            src_mem++;
        }
        break;
    case DMA::GU_DMA_TEXTURE_RG8:
        while (len--)
        {
            *((u16*)dest_mem) = *((u16*)src_mem);
            dest_mem += 2;
            src_mem += 2;
        }
        break;
    case DMA::GU_DMA_TEXTURE_RGB8:
        while (len--)
        {
            struct alignas(1) RGB { u8 r, g, b; };
            *((RGB*)dest_mem) = *((RGB*)src_mem);
            *((RGB*)dest_mem) = *((RGB*)src_mem);
            dest_mem += 3;
            src_mem += 3;
        }
        break;
    case DMA::GU_DMA_TEXTURE_RGBA8:
        while (len--)
        {
            *((Word*)dest_mem) = *((Word*)src_mem);
            dest_mem += 4;
            src_mem += 4;
        }
        break;
    }
}

Bus::CheckAddressResult VGU::check_vram_address(VirtualAddress vva)
{
    return vva < Bus::get_vram_size() ? Bus::ValidAddress : Bus::InvalidAddress;
}

void VGU::set_pixel(i32 x, i32 y, Color color)
{
    if (x < 0 || x >= state.width || y < 0 || y >= state.height)
        return;

    state.display_address[x + y * state.width] = *(Word*)&color;
}

void VGU::queue_send_signal(Queue* queue, QueueSignal signal)
{
    queue->queue_mutex.lock();
    queue->signal = signal;
    queue->queue_mutex.unlock();

    state.sync_mutex.lock();
    state.queue_requested = true;
    state.sync_mutex.unlock();
}

void VGU::queue_execute_cmd(Queue* queue)
{
    queue->queue_mutex.lock();
    QueueSignal signal = queue->signal;
    queue->queue_mutex.unlock();

    if (signal == QUEUE_IDLE)
        return;

    Word* cmd_list = (Word*)Bus::get_physical_addr(queue->cmd_list);
    Word cmd_len = queue->cmd_len;

    while (cmd_len)
    {
        Word cmd_w = cmd_list[0];
        cmd_list++;
        cmd_len--;

        GUDevice::GUCommand cmd = GUDevice::GUCommand(cmd_w >> 24);

        switch (cmd)
        {
        case GUDevice::GU_COMMAND_END:
            cmd_len = 0;
            break;
        case GUDevice::GU_COMMAND_RECT:
        {
            if (!cmd_len) break;

            Color rgb = {};
            *((Word*)&rgb) = cmd_w & 0xFFFFFF;
            rgb.a = 0xFF;

            u16 x, y, w, h;
            cmd_w = cmd_list[0];
            cmd_list++;
            cmd_len--;
            if (!cmd_len) break;

            x = cmd_w & 0xFFFF;
            y = (cmd_w >> 16);

            cmd_w = cmd_list[0];
            cmd_list++;
            cmd_len--;

            w = (cmd_w) & 0xFFFF;
            h = (cmd_w >> 16);

            fill_rectangle(rgb, x, y, w, h);
        }
            break;
        }


    }
}

void VGU::fill_rectangle(Color color, i32 x, i32 y, i32 w, i32 h)
{
    for (i32 y1 = y; y1 < y + h; y1++)
    {
        for (i32 x1 = x; x1 < x + w; x1++)
        {
            state.display_address[(y1 * state.height) + x1] = color.rgba;
        }
    }
}

