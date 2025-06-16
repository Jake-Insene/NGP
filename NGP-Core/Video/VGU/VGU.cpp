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

template<typename T>
static void dma_transfer_with_step(PhysicalAddress dest_mem, PhysicalAddress src_mem, Word len)
{
    while (len--)
    {
        *((T*)dest_mem) = *((T*)src_mem);
        dest_mem += sizeof(T);
        src_mem += sizeof(T);
    }
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
        .queue_dispatch = &VGU::queue_dispatch,

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
    state.sync_mutex.unlock();
    
    if (state.display_address == nullptr || state.width == 0 || state.height == 0 || !present_requested)
        return;

    state.internal_driver.update_framebuffer(state.fb, state.display_address);
    state.internal_driver.present_framebuffer(state.fb, vsync);

    set_present_requested(false);

    VGPU_LOGGER("Present: FB=0x%016llX", state.fb);
}

void VGU::request_present()
{
    set_present_requested(true);
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
    if ((cmd_list & 0x3) != 0)
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

    state.queue_mutex.lock();
    Queue* queue = &state.queues[index];
    queue->priority = priority;
    queue->cmd_list = cmd_list;
    queue->cmd_len = cmd_len;
    queue->signal = QUEUE_SIGNAL_RUN;
    state.queue_mutex.unlock();
}

void VGU::queue_dispatch()
{
    state.queue_mutex.lock();
    for (auto& queue : state.queues)
    {
        queue_execute_cmd(&queue);
    }
    state.queue_mutex.unlock();
}

void VGU::dma_send(VirtualAddress dest, VirtualAddress src, Word len, Word flags)
{
    PhysicalAddress dest_mem = 0;
    PhysicalAddress src_mem = 0;

    DMA::DMADirection dir = DMA::DMADirection((flags >> 4) & 0x7);
    DMA::DMAStep step = DMA::DMAStep((flags >> 5) & 0x7);

    if (dir == DMA::DMA_RAM_TO_DEVICE)
    {
        dest_mem = VRamAddress + dest;
        src_mem = Bus::get_physical_addr(src);
    }
    else if (dir == DMA::DMA_DEVICE_TO_RAM)
    {
        dest_mem = Bus::get_physical_addr(dest);
        src_mem = VRamAddress + src;
    }
    else if (dir == DMA::DMA_DEVICE_TO_DEVICE)
    {
        dest_mem = VRamAddress + dest;
        src_mem = VRamAddress + src;
    }
    else
    {
        return;
    }

    switch (step)
    {
    case DMA::DMA_WORD:
        dma_transfer_with_step<Word>(dest_mem, src_mem, len);
        break;
    case DMA::DMA_HALF:
        dma_transfer_with_step<u16>(dest_mem, src_mem, len);
        break;
    case DMA::DMA_BYTE:
        dma_transfer_with_step<u8>(dest_mem, src_mem, len);
        break;
    case DMA::DMA_DWORD:
        dma_transfer_with_step<DWord>(dest_mem, src_mem, len);
        break;
    case DMA::DMA_QWORD:
        dma_transfer_with_step<QWord>(dest_mem, src_mem, len);
        break;
    }

    DMA::get_registers().channels[DMA::DMA_CHANNEL_GU].ctr = flags & DMA::DMA_IRQ;
}

Bus::CheckAddressResult VGU::check_vram_address(VirtualAddress vva)
{
    return vva < Bus::get_vram_size() ? Bus::ValidAddress : Bus::InvalidAddress;
}

// Implementation

// Internal functions
void VGU::set_present_requested(bool requested)
{
    state.sync_mutex.lock();
    state.present_requested = requested;
    state.sync_mutex.unlock();
}

void VGU::set_pixel(i32 x, i32 y, Color color)
{
    if (x < 0 || x >= state.width || y < 0 || y >= state.height)
        return;

    state.display_address[(y * state.width) + x] = color.rgba;
}

// DMA


// Queue

void VGU::queue_execute_cmd(Queue* queue)
{
    if (queue->signal == QUEUE_SIGNAL_IDLE)
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
            *((Word*)&rgb) = cmd_w & 0xFF'FFFF;
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
        case GUDevice::GU_COMMAND_TEXTURE_SET:
        {
            if (!cmd_len) break;
            VirtualAddress tex_address = cmd_w & 0xFF'FFFF;

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

            PhysicalAddress texture = VRamAddress + tex_address;

            for (i32 y1 = y, ty = 0; y1 < y + h; y1++, ty++)
            {
                for (i32 x1 = x, tx = 0; x1 < x + w; x1++, tx++)
                {
                    PhysicalAddress offset = ((ty * w) + tx) * 4;
                    Color c;
                    c.rgba = *(Word*)(texture + offset);
                    set_pixel(x1, y1, c);
                }
            }
        }
            break;
        default:
            break;
        }
    }

    queue->signal = QUEUE_SIGNAL_IDLE;
}

// Drawing
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

