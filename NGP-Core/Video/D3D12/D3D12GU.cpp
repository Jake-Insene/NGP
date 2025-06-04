/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/D3D12/D3D12GU.h"

#include "Video/D3D12/DX.h"
#include "Video/Window.h"
#include "Memory/Bus.h"
#include "Platform/OS.h"


struct Vertex
{
    float x, y;
    float u, v;
};


const Vertex vertices[] =
{
    Vertex(-1.f, 1.f, 0, 0),
    Vertex(1.f, 1.f, 1, 0),
    Vertex(1.f, -1.f, 1, 1),
    Vertex(1.f, -1.f, 1, 1),
    Vertex(-1.f, -1.f, 0, 1),
    Vertex(-1.f, 1.f, 0, 0),
};

static constexpr usize VRamAddress = 0x2'0000'0000;

GU::GUDriver D3D12GU::get_driver()
{
    return GU::GUDriver{
        .initialize = D3D12GU::initialize,
        .shutdown = D3D12GU::shutdown,

        .present = D3D12GU::present,
        .create_framebuffer = &D3D12GU::create_framebuffer,
        .update_framebuffer = &D3D12GU::update_framebuffer,
    };
}

void D3D12GU::initialize()
{
    state.current_frame = 0;

    // Debug
    {
#if NDEBUG == 0
        ID3D12Debug3* debug = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
        {
            debug->EnableDebugLayer();
            debug->SetEnableGPUBasedValidation(TRUE);
        }
#endif

        DX_ERROR(
            D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&state.device)),
            "Couldn't initialize the D3D12 driver"
        );

#if NDEBUG == 0
        ID3D12InfoQueue* info_queue = nullptr;
        state.device->QueryInterface(IID_PPV_ARGS(&info_queue));
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        info_queue->Release();
        debug->Release();
#endif
    }

    // Command Queue
    {
        D3D12_COMMAND_QUEUE_DESC queue_desc =
        {
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0,
        };
        DX_ERROR(
            state.device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&state.queue_graphics)),
            "Couldn't create the graphics queue"
        );

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        DX_ERROR(
            state.device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&state.queue_compute)),
            "Couldn't create the compute queue"
        );
    }

    // Swap Chain
    {
        UINT flags =
#if NDEBUG == 0
            DXGI_CREATE_FACTORY_DEBUG;
#else
            0;
#endif

        DX_ERROR(
            CreateDXGIFactory2(flags, IID_PPV_ARGS(&state.dxgi_factory)),
            "Couldn't create the DXGI factory"
        );

        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc =
        {
                .Width = Window::DefaultWindowWidth,
                .Height = Window::DefaultWindowHeight,
                .Format = DefaultFormat,
                .SampleDesc = { 1, 0 },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = DefaultBufferCount,
                .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                .AlphaMode = DXGI_ALPHA_MODE_IGNORE,
                .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
        };

        IDXGISwapChain1* tmp_sc = nullptr;
        HWND handle = (HWND)Window::handle;

        DX_ERROR(
            state.dxgi_factory->CreateSwapChainForHwnd(
                state.queue_graphics, handle, &swap_chain_desc, nullptr, nullptr, &tmp_sc
            ),
            "Couldn't create the display swap chain"
        );

        DX_ERROR(
            tmp_sc->QueryInterface(IID_PPV_ARGS(&state.swap_chain)),
            "Couldn't query IDXGISwapChain"
        );

        tmp_sc->Release();
    }

    // RTV Heap
    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc =
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = DefaultBufferCount,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };

    state.device->CreateDescriptorHeap(
        &descriptor_heap_desc,
        IID_PPV_ARGS(&state.rtv_heap)
    );

    // SRV Heap
    descriptor_heap_desc =
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = MaxFramebufferCount,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0
    };

    state.device->CreateDescriptorHeap(
        &descriptor_heap_desc,
        IID_PPV_ARGS(&state.srv_heap)
    );

    // Synchronization && RTV && Command Lists
    state.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&state.cmd_compute_allocator));
    state.device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_COMPUTE, state.cmd_compute_allocator, 
        nullptr, IID_PPV_ARGS(&state.cmd_compute)
    );
    state.cmd_compute->Close();
    state.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&state.compute_fence));
    state.compute_fence_event = CreateEventA(nullptr, 0, 0, nullptr);

    for (u32 i = 0; i < DefaultBufferCount; i++)
    {
        Framebuffer& frame = state.framebuffers[i];
        state.swap_chain->GetBuffer(i, IID_PPV_ARGS(&frame.buffer));

        D3D12_CPU_DESCRIPTOR_HANDLE heap_begin = state.rtv_heap->GetCPUDescriptorHandleForHeapStart();
        frame.handle.ptr = heap_begin.ptr
            + (state.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * i);

        state.device->CreateRenderTargetView(frame.buffer, nullptr, frame.handle);

        DX_ERROR(
            state.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frame.fence)),
            "Couldn't create the main fence"
        );
        frame.fence_value = 0;
        frame.fence_event = CreateEventA(nullptr, FALSE, FALSE, nullptr);

        DX_ERROR(
            state.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.cmd_graphics_allocator)),
            "Couldn't create the graphics command allocator"
        );

        DX_ERROR(
            state.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frame.cmd_graphics_allocator, nullptr, IID_PPV_ARGS(&frame.cmd_graphics)),
            "Couldn't create the graphics command list of frame: %d", i
        );
        frame.cmd_graphics->Close();
    }

    // Pipeline
    {
        ID3DBlob* vs_code = {};
        ID3DBlob* ps_code = {};
        ID3DBlob* error_blob = {};
        HRESULT result = S_OK;

        DX_ERROR(
            result = D3DCompileFromFile(L"Assets/blit.vs.d3d12.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs_code, &error_blob),
            "Couldn't create the blit vertex shader"
        );
        if (FAILED(result))
        {
            DEBUG_OUTPUT((CHAR*)error_blob->GetBufferPointer());
            dx::release(error_blob);
        }

        DX_ERROR(
            result = D3DCompileFromFile(L"Assets/blit.ps.d3d12.hlsl", nullptr, nullptr, "PSMain", "ps_5_1", 0, 0, &ps_code, &error_blob),
            "Couldn't create the blit pixel shader"
        );
        if (FAILED(result))
        {
            DEBUG_OUTPUT((CHAR*)error_blob->GetBufferPointer());
            dx::release(error_blob);
        }

        ID3DBlob* root_blob;

        D3D12_DESCRIPTOR_RANGE descriptor_range1 = {};
        descriptor_range1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptor_range1.NumDescriptors = 1;
        descriptor_range1.BaseShaderRegister = 0;
        descriptor_range1.RegisterSpace = 0;

        D3D12_ROOT_PARAMETER param1 = {};
        param1.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param1.DescriptorTable.NumDescriptorRanges = 1;
        param1.DescriptorTable.pDescriptorRanges = &descriptor_range1;
        param1.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_STATIC_SAMPLER_DESC static_sampler = {};
        static_sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        static_sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        static_sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        static_sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        static_sampler.ShaderRegister = 0;
        static_sampler.RegisterSpace = 1;
        static_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        D3D12_ROOT_PARAMETER params[] = { param1 };

        D3D12_ROOT_SIGNATURE_DESC root_desc = {};
        root_desc.NumParameters = 1;
        root_desc.pParameters = params;
        root_desc.NumStaticSamplers = 1;
        root_desc.pStaticSamplers = &static_sampler;
        root_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        result = D3D12SerializeRootSignature(&root_desc, D3D_ROOT_SIGNATURE_VERSION_1, &root_blob, &error_blob);
        if (FAILED(result))
        {
            DEBUG_OUTPUT((CHAR*)error_blob->GetBufferPointer());
            dx::release(error_blob);
        }

        DX_ERROR(
            state.device->CreateRootSignature(
                0, root_blob->GetBufferPointer(), root_blob->GetBufferSize(), 
                IID_PPV_ARGS(&state.root_signature)
            ),
            "Couldn't create the root signature"
        );

        D3D12_INPUT_ELEMENT_DESC elements[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };

        D3D12_INPUT_LAYOUT_DESC input_layout =
        {
            .pInputElementDescs = elements,
            .NumElements = 2
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_desc = {};
        pipeline_desc.pRootSignature = state.root_signature;
        pipeline_desc.VS = D3D12_SHADER_BYTECODE
        {
            .pShaderBytecode = vs_code->GetBufferPointer(),
            .BytecodeLength = vs_code->GetBufferSize()
        };
        pipeline_desc.PS = D3D12_SHADER_BYTECODE
        {
            .pShaderBytecode = ps_code->GetBufferPointer(),
            .BytecodeLength = ps_code->GetBufferSize()
        };
        pipeline_desc.BlendState = D3D12_BLEND_DESC
        {
            .AlphaToCoverageEnable = false,
            .IndependentBlendEnable = false,
        };
        pipeline_desc.BlendState.RenderTarget[0] = D3D12_RENDER_TARGET_BLEND_DESC
        {
            .BlendEnable = false,
            .LogicOpEnable = false,
            .SrcBlend = D3D12_BLEND_ONE,
            .DestBlend = D3D12_BLEND_ZERO,
            .BlendOp = D3D12_BLEND_OP_ADD,
            .SrcBlendAlpha = D3D12_BLEND_ONE,
            .DestBlendAlpha = D3D12_BLEND_ZERO,
            .BlendOpAlpha = D3D12_BLEND_OP_ADD,
            .LogicOp = D3D12_LOGIC_OP_NOOP,
            .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
        };
        pipeline_desc.SampleMask = UINT_MAX;
        pipeline_desc.RasterizerState = D3D12_RASTERIZER_DESC
        {
            .FillMode = D3D12_FILL_MODE_SOLID,
            .CullMode = D3D12_CULL_MODE_BACK,
            .FrontCounterClockwise = FALSE,
            .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
            .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
            .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
            .DepthClipEnable = TRUE,
            .MultisampleEnable = FALSE,
            .AntialiasedLineEnable = FALSE,
            .ForcedSampleCount = 0,
            .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,
        };
        pipeline_desc.DepthStencilState.DepthEnable = false;
        pipeline_desc.DepthStencilState.StencilEnable = false;
        pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pipeline_desc.NumRenderTargets = 1;
        pipeline_desc.RTVFormats[0] = DefaultFormat;
        pipeline_desc.SampleDesc.Count = 1;
        pipeline_desc.InputLayout = input_layout;

        state.device->CreateGraphicsPipelineState(&pipeline_desc, IID_PPV_ARGS(&state.blit_pipeline));
        dx::release(vs_code);
        dx::release(ps_code);
    }

    // Vertex Buffer
    {
        D3D12_RESOURCE_DESC vb_desc = dx::buffer_desc(sizeof(vertices));

        D3D12_HEAP_PROPERTIES heap_properties = dx::heap_properties(D3D12_HEAP_TYPE_DEFAULT);
        state.device->CreateCommittedResource(
            &heap_properties, D3D12_HEAP_FLAG_NONE, &vb_desc, 
            D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&state.vb)
        );

        ID3D12Resource* vb_upload = nullptr;
        heap_properties = dx::heap_properties(D3D12_HEAP_TYPE_UPLOAD);
        state.device->CreateCommittedResource(
            &heap_properties, D3D12_HEAP_FLAG_NONE, &vb_desc,
            D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&vb_upload)
        );

        void* data;
        vb_upload->Map(0, nullptr, &data);
        memcpy(data, vertices, sizeof(vertices));
        vb_upload->Unmap(0, nullptr);

        // Upload vertex buffer
        send_compute([](ID3D12Resource* vb_upload)
            {
                D3D12_RESOURCE_BARRIER b1 = dx::transition_barrier(
                    state.vb, 0, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST
                );
                state.cmd_compute->ResourceBarrier(1, &b1);

                b1 = dx::transition_barrier(
                    vb_upload, 0, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE
                );
                state.cmd_compute->ResourceBarrier(1, &b1);

                state.cmd_compute->CopyResource(state.vb, vb_upload);

                b1 = dx::transition_barrier(
                    state.vb, 0, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
                );
                state.cmd_compute->ResourceBarrier(1, &b1);
            }, vb_upload);

        dx::release(vb_upload);
    }
}

void D3D12GU::shutdown()
{
    dx::release(state.blit_pipeline);
    dx::release(state.root_signature);
    dx::release(state.vb);

    dx::release(state.framebuffer);
    dx::release(state.upload_framebuffer);

    dx::release(state.swap_chain);
    dx::release(state.dxgi_factory);

    dx::release(state.rtv_heap);

    for (u32 i = 0; i < DefaultBufferCount; i++)
    {
        dx::release(state.framebuffers[i].buffer);
        dx::release(state.framebuffers[i].fence);
        dx::release(state.framebuffers[i].cmd_graphics_allocator);
        dx::release(state.framebuffers[i].cmd_graphics);
        CloseHandle(state.framebuffers[i].fence_event);
    }

    dx::release(state.compute_fence);
    dx::release(state.cmd_compute);
    dx::release(state.cmd_compute_allocator);
    dx::release(state.queue_compute);
    dx::release(state.queue_graphics);

#if NDEBUG == 0
    ID3D12DebugDevice* debug_device = nullptr;
    state.device->QueryInterface(IID_PPV_ARGS(&debug_device));
    debug_device->ReportLiveDeviceObjects(D3D12_RLDO_NONE);
    debug_device->Release();
#endif

    dx::release(state.device);
}

void D3D12GU::present(bool vsync)
{
    Framebuffer& frame = state.framebuffers[state.current_frame];
    if (frame.fence_value != frame.fence->GetCompletedValue())
    {
        frame.fence->SetEventOnCompletion(frame.fence_value, frame.fence_event);
        WaitForSingleObject(frame.fence_event, INFINITE);
    }

    ID3D12GraphicsCommandList4* cmd_graphics = frame.cmd_graphics;
    frame.cmd_graphics_allocator->Reset();
    cmd_graphics->Reset(frame.cmd_graphics_allocator, nullptr);

    cmd_graphics->OMSetRenderTargets(1, &frame.handle, false, nullptr);

    D3D12_VIEWPORT viewport = {};
    viewport.Width = Window::DefaultWindowWidth;
    viewport.Height = Window::DefaultWindowHeight;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    cmd_graphics->RSSetViewports(1, &viewport);
    D3D12_RECT rect = {0, 0, Window::DefaultWindowWidth, Window::DefaultWindowHeight };
    cmd_graphics->RSSetScissorRects(1, &rect);

    D3D12_RESOURCE_BARRIER b1 = dx::transition_barrier(
        frame.buffer, 0, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    cmd_graphics->ResourceBarrier(1, &b1);

    cmd_graphics->ClearRenderTargetView(frame.handle, state.clear_color, 0, nullptr);

    cmd_graphics->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_graphics->SetPipelineState(state.blit_pipeline);
    cmd_graphics->SetGraphicsRootSignature(state.root_signature);
    cmd_graphics->SetDescriptorHeaps(1, &state.srv_heap);
    cmd_graphics->SetGraphicsRootDescriptorTable(0, state.framebuffer_srv);
    D3D12_VERTEX_BUFFER_VIEW vb_view =
    {
        .BufferLocation = state.vb->GetGPUVirtualAddress(),
        .SizeInBytes = sizeof(vertices),
        .StrideInBytes = sizeof(Vertex)
    };
    cmd_graphics->IASetVertexBuffers(0, 1, &vb_view);

    cmd_graphics->DrawInstanced(6, 1, 0, 0);

    b1 = dx::transition_barrier(
        frame.buffer, 0, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
    );
    cmd_graphics->ResourceBarrier(1, &b1);

    cmd_graphics->Close();

    ID3D12CommandList* cmd_list[] = { cmd_graphics };
    state.queue_graphics->ExecuteCommandLists(1, cmd_list);

    state.swap_chain->Present(vsync, 0);
    state.current_frame = (state.current_frame + 1) % DefaultBufferCount;

    // Signal fence
    state.queue_graphics->Signal(frame.fence, ++frame.fence_value);
}

VirtualAddress D3D12GU::create_framebuffer(i32 width, i32 height)
{
    state.framebuffer_width = width;
    state.framebuffer_height = height;

    D3D12_HEAP_PROPERTIES heap_properties = dx::heap_properties(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC resource_desc =
    {
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
        .Width = (UINT)state.framebuffer_width,
        .Height = (UINT)state.framebuffer_height,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc = {.Count = 1},
    };

    (void)state.device->CreateCommittedResource(
        &heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&state.framebuffer)
    );

    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc =
    {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        .Texture2D = {.MipLevels = 1}
    };

    state.device->CreateShaderResourceView(state.framebuffer, &view_desc, state.srv_heap->GetCPUDescriptorHandleForHeapStart());
    state.framebuffer_srv = state.srv_heap->GetGPUDescriptorHandleForHeapStart();

    heap_properties = dx::heap_properties(D3D12_HEAP_TYPE_UPLOAD);
    resource_desc = dx::buffer_desc(state.framebuffer_width * state.framebuffer_height * 4);

    (void)state.device->CreateCommittedResource(
        &heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc,
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&state.upload_framebuffer)
    );

    return (VirtualAddress)0;
}

void D3D12GU::update_framebuffer(VirtualAddress fb, void* va)
{
    void* data;
    state.upload_framebuffer->Map(0, nullptr, &data);
    memcpy(data, va, state.framebuffer_width * state.framebuffer_height * 4);
    state.upload_framebuffer->Unmap(0, nullptr);

    Framebuffer& frame = state.framebuffers[state.current_frame];
    if (frame.fence_value != frame.fence->GetCompletedValue())
    {
        frame.fence->SetEventOnCompletion(frame.fence_value, frame.fence_event);
        WaitForSingleObject(frame.fence_event, INFINITE);
    }

    ID3D12GraphicsCommandList4* cmd_graphics = frame.cmd_graphics;
    frame.cmd_graphics_allocator->Reset();
    cmd_graphics->Reset(frame.cmd_graphics_allocator, nullptr);

    D3D12_RESOURCE_BARRIER t3 = dx::transition_barrier(
        state.upload_framebuffer, 0, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE
    );

    D3D12_RESOURCE_BARRIER t4 = dx::transition_barrier(
        state.framebuffer, 0, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST
    );

    D3D12_RESOURCE_BARRIER translations[] = { t3, t4};
    cmd_graphics->ResourceBarrier(2, translations);

    D3D12_TEXTURE_COPY_LOCATION dest =
    {
        .pResource = state.framebuffer,
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = 0,
    };

    D3D12_TEXTURE_COPY_LOCATION src =
    {
        .pResource = state.upload_framebuffer,
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
    };

    src.PlacedFootprint.Footprint =
    {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .Width = (UINT)state.framebuffer_width,
        .Height = (UINT)state.framebuffer_height,
        .Depth = 1,
        .RowPitch = (UINT)state.framebuffer_width*4
    };

    D3D12_BOX box =
    {
        .left = 0,
        .top = 0,
        .front = 0,
        .right = (UINT)state.framebuffer_width,
        .bottom = (UINT)state.framebuffer_height,
        .back = 1,
    };

    cmd_graphics->CopyTextureRegion(&dest, 0, 0, 0, &src, &box);

    D3D12_RESOURCE_BARRIER translation1 = dx::transition_barrier(
        state.upload_framebuffer, 0, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON
    );
    cmd_graphics->ResourceBarrier(1, &translation1);

    D3D12_RESOURCE_BARRIER translation2 = dx::transition_barrier(
        state.framebuffer, 0, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );
    cmd_graphics->ResourceBarrier(1, &translation2);

    cmd_graphics->Close();

    ID3D12CommandList* cmd_list[] = { cmd_graphics };
    state.queue_graphics->ExecuteCommandLists(1, cmd_list);

    // Signal fence
    HRESULT result = state.queue_graphics->Signal(frame.fence, ++frame.fence_value);
    state.queue_graphics->Wait(frame.fence, frame.fence_value);
}

