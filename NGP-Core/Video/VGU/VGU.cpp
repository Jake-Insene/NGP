/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/VGU/VGU.h"

#include "Memory/Bus.h"
#include "IO/DMA/DMA.h"
#include "IO/GU/GU.h"
#include "Platform/OS.h"
#include "Video/OpenGL/GLGU.h"
#include "Video/VGU/VRasterizer.h"

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


GUDevice::GUDriver get_internal_driver(GUDevice::DriverApi api)
{
    switch (api)
    {
    case GUDevice::NONE:
        return {};
    case GUDevice::D3D11:
#if defined(_WIN32)
        return D3D11GU::get_driver();
#else
        return {};
#endif
    case GUDevice::D3D12:
#if defined(_WIN32)
        return D3D12GU::get_driver();
#else
        return {};
#endif
    case GUDevice::OPENGL:
        return GLGU::get_driver();
    case GUDevice::VGU:
        return {};
    }

    return {};
}


GUDevice::GUDriver VGU::get_driver()
{
    return GUDevice::GUDriver
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
    state.vram_size = Bus::VRAM_SIZE;
    state.vram = (Word*)OS::allocate_virtual_memory((void*)VRamAddress, Bus::VRAM_SIZE, OS::PAGE_READ_WRITE);

    state.present_requested = false;
    state.irq_pending = false;

    // Internal driver
    state.internal_driver = get_internal_driver(GUDevice::D3D11);
    state.internal_driver.initialize();

    VGUQueue::initialize();

    state.cached_framebuffers.clear();
    state.current_fb = -1;

    VRasterizer::initialize();
}

void VGU::shutdown()
{
    VRasterizer::shutdown();
    VGUQueue::shutdown();

    OS::deallocate_virtual_memory((void*)VRamAddress);

    state.internal_driver.shutdown();
}


bool VGU::present(bool vsync)
{
    state.sync_mutex.lock();
    bool present_requested = state.present_requested;
    state.sync_mutex.unlock();

    if (state.display_address == nullptr || state.current_fb == InvalidFB || !present_requested)
        return false;

    state.internal_driver.update_framebuffer(get_current_framebuffer().framebuffer, state.display_address);
    state.internal_driver.present_framebuffer(get_current_framebuffer().framebuffer, vsync);

    set_present_requested(false);
    return true;
}

void VGU::request_present()
{
    set_present_requested(true);
}

void VGU::display_set_config(i32 width, i32 height, Display::DisplayFormat display_format)
{
    // Internal Framebuffer
    for (i32 i = 0; i < state.cached_framebuffers.size(); i++)
    {
        VFramebuffer& fb = state.cached_framebuffers[i];
        if (fb.width == width && fb.height == height && fb.display_format == display_format)
        {
            state.current_fb = i;
            return;
        }
    }

    VFramebuffer fb_info;
    fb_info.framebuffer = state.internal_driver.create_framebuffer(width, height);
    fb_info.width = width;
    fb_info.height = height;
    fb_info.display_format = display_format;
    state.cached_framebuffers.emplace_back(fb_info);
    
    state.current_fb = state.cached_framebuffers.size() - 1;
}

void VGU::display_set_address(VirtualAddress vva)
{
    state.display_address = (Word*)get_physical_vram_address(vva);
}

void VGU::queue_execute(VirtualAddress cmd_list, Word cmd_len)
{
    if ((cmd_list & 0x3) != 0)
    {
        VGPU_LOGGER("Unaligned Command List Base Address");
        return;
    }

    if (cmd_len == 0)
        return;

    state.queue_mutex.lock();
    VGUQueue::set_state(cmd_list, cmd_len, VGUQueue::get_state());
    VGUQueue::set_signal(VGUQueue::QUEUE_SIGNAL_RUN);
    state.queue_mutex.unlock();
}

void VGU::queue_dispatch()
{
    state.queue_mutex.lock();
    VGUQueue::try_execute();
    state.queue_mutex.unlock();
}

void VGU::dma_send(VirtualAddress dest, VirtualAddress src, Word len, Word flags)
{
    PhysicalAddress dest_mem = 0;
    PhysicalAddress src_mem = 0;

    DMA::DMADirection dir = DMA::DMADirection((flags >> 3) & 0x3);
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
    return vva < state.vram_size ? Bus::ValidAddress : Bus::InvalidAddress;
}

// Implementation

PhysicalAddress VGU::get_physical_vram_address(VirtualAddress vaddress)
{
    return VRamAddress + vaddress;
}

// Internal functions
void VGU::set_present_requested(bool requested)
{
    state.sync_mutex.lock();
    state.present_requested = requested;
    state.sync_mutex.unlock();
}


