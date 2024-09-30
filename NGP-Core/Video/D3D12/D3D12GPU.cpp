/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/D3D12/D3D12GPU.h"

#include <stdio.h>
#define DX_ERROR(expr, ...)\
    if((expr) != S_OK){\
        OutputDebugStringA(__VA_ARGS__);\
        exit(-1);\
    }

GPUDriver D3D12GPU::get() {
    return GPUDriver{
        .initialize = D3D12GPU::initialize,
        .shutdown = D3D12GPU::shutdown,
    };
}

void D3D12GPU::initialize() {
#if NDEBUG == 0
    ID3D12Debug3* debug = nullptr;
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
    {
        debug->EnableDebugLayer();
        debug->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    DX_ERROR(
        D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)),
        "Couln't initialize the D3D12 driver"
    );

#if NDEBUG == 0
    if (debug != nullptr) {
        ID3D12InfoQueue* info_queue = nullptr;
        device->QueryInterface(IID_PPV_ARGS(&info_queue));
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        info_queue->Release();
        debug->Release();
    }
#endif

    D3D12_COMMAND_QUEUE_DESC queue_desc = {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };
    DX_ERROR(
        device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&queue_graphics)),
        "Couln't create the graphics queue"
    );

    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    DX_ERROR(
        device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&queue_compute)),
        "Couln't create the compute queue"
    );

    DX_ERROR(
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmd_graphics_allocator)),
        "Couln't create the graphics command allocator"
    );

    DX_ERROR(
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&cmd_compute_allocator)),
        "Couln't create the compute command allocator"
    );

    DX_ERROR(
        device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmd_graphics_allocator, nullptr, IID_PPV_ARGS(&cmd_graphics)),
        "Couln't create the graphics command list"
    );
    cmd_graphics->Close();

    DX_ERROR(
        device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, cmd_compute_allocator, nullptr, IID_PPV_ARGS(&cmd_compute)),
        "Couln't create the compute command list"
    );
    cmd_compute->Close();

    DX_ERROR(
        device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&main_fence)),
        "Couldn't create the main fence"
    );

}

void D3D12GPU::shutdown() {
    main_fence->Release();

    cmd_graphics->Release();
    cmd_compute->Release();

    cmd_graphics_allocator->Release();
    cmd_compute_allocator->Release();

    queue_graphics->Release();
    queue_compute->Release();

    device->Release();

}
