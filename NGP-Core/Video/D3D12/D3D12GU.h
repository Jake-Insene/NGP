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
    static constexpr u32 DefaultBufferCount = 3;
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

    static inline Framebuffer framebuffers[DefaultBufferCount] = {};

    static inline IDXGIFactory4* dxgi_factory = nullptr;
    static inline IDXGISwapChain4* swap_chain = nullptr;

    static inline ID3D12Device10* device = nullptr;
    static inline ID3D12CommandQueue* queue_graphics = nullptr;
    static inline ID3D12CommandQueue* queue_compute = nullptr;
    static inline ID3D12DescriptorHeap* rtv_heap = nullptr;
    static inline ID3D12DescriptorHeap* srv_heap = nullptr;
    static inline ID3D12PipelineState* blit_pipeline = nullptr;
    static inline ID3D12RootSignature* root_signature = nullptr;

    static inline ID3D12Resource* vb = nullptr;
    static inline ID3D12Resource* upload_framebuffer = nullptr;
    static inline ID3D12Resource* framebuffer = nullptr;
    static inline D3D12_GPU_DESCRIPTOR_HANDLE framebuffer_srv = {};

    static inline u32 current_frame = 0;

    static inline FLOAT clear_color[4] =
    {
        0.f, 0.f, 0.f, 1.f
    };

    static GPUDriver get_driver();

    static void initialize();
    static void shutdown();

    static void present();
    static VirtualAddress create_framebuffer();
    static void update_framebuffer(void* pixels);
};
