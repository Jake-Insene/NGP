/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Platform/Header.h"
#include "Video/GU.h"

struct D3D12GU
{
    static constexpr u32 DefaultBufferCount = 4;
    static constexpr u32 MaxFramebufferCount = 4;
    static constexpr DXGI_FORMAT DefaultFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    struct Framebuffer
    {
        ID3D12Resource* buffer;
        D3D12_CPU_DESCRIPTOR_HANDLE handle;
        ID3D12Fence* fence;
        HANDLE fence_event;
        u64 fence_value;

        ID3D12CommandAllocator* cmd_graphics_allocator;
        ID3D12GraphicsCommandList4* cmd_graphics;
    };

    struct GPUState
    {
        ID3D12Fence* compute_fence;
        u64 compute_fence_value;
        HANDLE compute_fence_event;
        ID3D12CommandAllocator* cmd_compute_allocator;
        ID3D12GraphicsCommandList4* cmd_compute;

        Framebuffer framebuffers[DefaultBufferCount];

        IDXGIFactory4* dxgi_factory;
        IDXGISwapChain4* swap_chain;

        ID3D12Device10* device;
        ID3D12CommandQueue* queue_graphics;
        ID3D12CommandQueue* queue_compute;
        ID3D12DescriptorHeap* rtv_heap;
        ID3D12DescriptorHeap* srv_heap;
        ID3D12PipelineState* blit_pipeline;
        ID3D12RootSignature* root_signature;

        // Upload buffers
        ID3D12Resource* vb;
        ID3D12Resource* upload_framebuffer;

        // Virtual Framebuffer
        ID3D12Resource* framebuffer;
        D3D12_GPU_DESCRIPTOR_HANDLE framebuffer_srv;
        i32 framebuffer_width;
        i32 framebuffer_height;

        u32 current_frame;

        FLOAT clear_color[4];
    };

    static inline GPUState state;

    static GU::GUDriver get_driver();

    static void initialize();
    static void shutdown();

    static void present_framebuffer(PhysicalAddress fb, bool vsync);
    static void present(bool vsync);

    static PhysicalAddress create_framebuffer(i32 width, i32 height);
    static void update_framebuffer(PhysicalAddress fb, void* va);

    template<typename Fn, typename... TArgs>
    static void send_compute(Fn fn, TArgs&&... args)
    {
        state.cmd_compute_allocator->Reset();
        state.cmd_compute->Reset(state.cmd_compute_allocator, nullptr);

        fn(args...);

        state.cmd_compute->Close();
        state.queue_compute->ExecuteCommandLists(1, (ID3D12CommandList**)&state.cmd_compute);

        // Signal fence
        state.queue_compute->Signal(state.compute_fence, ++state.compute_fence_value);
        state.queue_compute->Wait(state.compute_fence, state.compute_fence_value);
        state.compute_fence->SetEventOnCompletion(state.compute_fence_value, state.compute_fence_event);
        WaitForSingleObject(state.compute_fence_event, INFINITE);
    }
};
