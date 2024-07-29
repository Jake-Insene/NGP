/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Video/GPU.h"

#include <d3d12.h>

struct GPUD12 {
    static GPUDriver get();

    static void initialize();
    static void shutdown();

    static inline ID3D12Device4* device = nullptr;
    static inline ID3D12CommandQueue* queue_graphics = nullptr;
    static inline ID3D12CommandQueue* queue_compute = nullptr;

    static inline ID3D12CommandAllocator* cmd_graphics_allocator = nullptr;
    static inline ID3D12CommandAllocator* cmd_compute_allocator = nullptr;

    static inline ID3D12GraphicsCommandList4* cmd_graphics = nullptr;
    static inline ID3D12GraphicsCommandList4* cmd_compute = nullptr;

    static inline ID3D12Fence* main_fence = nullptr;
    static inline u64 fence_value = 0;

    static inline D3D12_VIEWPORT VIEWPORT = {
        0.0f, 0.0f,
        0.f, 0.f,
        0.f, 1.f,
    };
};
