// --------------------
// GPUD12.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "Core/Header.h"
#include "GPU/GPU.h"

#include <d3d12.h>

struct GPUD12 : GPUDriver {
    virtual void initialize() override;
    virtual void shutdown() override;

    ID3D12Device4* device;
    ID3D12CommandQueue* queue_graphics;
    ID3D12CommandQueue* queue_compute;

    ID3D12CommandAllocator* cmd_graphics_allocator;
    ID3D12CommandAllocator* cmd_compute_allocator;

    ID3D12GraphicsCommandList4* cmd_graphics;
    ID3D12GraphicsCommandList4* cmd_compute;

    ID3D12Fence* main_fence;
    u64 fence_value = 0;
};
