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

GPUDriver D3D12GU::get_driver()
{
    return GPUDriver{
        .initialize = D3D12GU::initialize,
        .shutdown = D3D12GU::shutdown,

        .present = D3D12GU::present,
        .create_framebuffer = &D3D12GU::create_framebuffer,
        .update_framebuffer = &D3D12GU::update_framebuffer,
    };
}

void D3D12GU::initialize()
{
    current_frame = 0;

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
            D3D12CreateDevice(0, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)),
            "Couldn't initialize the D3D12 driver"
        );

#if NDEBUG == 0
        ID3D12InfoQueue* info_queue = nullptr;
        device->QueryInterface(IID_PPV_ARGS(&info_queue));
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
            device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&queue_graphics)),
            "Couldn't create the graphics queue"
        );

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        DX_ERROR(
            device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&queue_compute)),
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
            CreateDXGIFactory2(flags, IID_PPV_ARGS(&dxgi_factory)),
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
            dxgi_factory->CreateSwapChainForHwnd(queue_graphics, handle, &swap_chain_desc, nullptr, nullptr, &tmp_sc),
            "Couldn't create the display swap chain"
        );

        DX_ERROR(
            tmp_sc->QueryInterface(IID_PPV_ARGS(&swap_chain)),
            "Can't query IDXGISwapChain"
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

    device->CreateDescriptorHeap(
        &descriptor_heap_desc,
        IID_PPV_ARGS(&rtv_heap)
    );

    // SRV Heap
    descriptor_heap_desc =
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = 1,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0
    };

    device->CreateDescriptorHeap(
        &descriptor_heap_desc,
        IID_PPV_ARGS(&srv_heap)
    );

    // Synchronization && RTV && Command Lists
    for (u32 i = 0; i < DefaultBufferCount; i++)
    {
        Framebuffer& frame = framebuffers[i];
        swap_chain->GetBuffer(i, IID_PPV_ARGS(&frame.buffer));

        D3D12_CPU_DESCRIPTOR_HANDLE heap_begin = rtv_heap->GetCPUDescriptorHandleForHeapStart();
        frame.handle.ptr = heap_begin.ptr + (device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * i);

        DX_ERROR(
            device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frame.fence)),
            "Couldn't create the main fence"
        );
        frame.fence_value = 0;
        frame.fence_event = CreateEventA(nullptr, FALSE, FALSE, nullptr);

        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc =
        {
            .Format = DefaultFormat,
            .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
            .Texture2D =
            {
                .MipSlice = 0,
                .PlaneSlice = 0,
            }
        };

        device->CreateRenderTargetView(frame.buffer, &rtv_desc, frame.handle);

        DX_ERROR(
            device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.cmd_graphics_allocator)),
            "Couldn't create the graphics command allocator"
        );

        DX_ERROR(
            device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frame.cmd_graphics_allocator, nullptr, IID_PPV_ARGS(&frame.cmd_graphics)),
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
            result = D3DCompileFromFile(L"Assets/blit.vs.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs_code, &error_blob),
            "Couldn't create the blit vertex shader"
        );
        if (FAILED(result))
        {
            DEBUG_OUTPUT((CHAR*)error_blob->GetBufferPointer());
            dx::release(error_blob);
        }

        DX_ERROR(
            result = D3DCompileFromFile(L"Assets/blit.ps.hlsl", nullptr, nullptr, "PSMain", "ps_5_1", 0, 0, &ps_code, &error_blob),
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
            device->CreateRootSignature(0, root_blob->GetBufferPointer(), root_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature)),
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
        pipeline_desc.pRootSignature = root_signature;
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

        device->CreateGraphicsPipelineState(&pipeline_desc, IID_PPV_ARGS(&blit_pipeline));
        dx::release(vs_code);
        dx::release(ps_code);
    }

    // Vertex Buffer
    {
        D3D12_RESOURCE_DESC vb_desc = dx::buffer_desc(sizeof(vertices));

        D3D12_HEAP_PROPERTIES heap_properties = dx::heap_properties(D3D12_HEAP_TYPE_DEFAULT);
        device->CreateCommittedResource(
            &heap_properties, D3D12_HEAP_FLAG_NONE, &vb_desc, 
            D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&vb)
        );

        ID3D12Resource* vb_upload = nullptr;
        heap_properties = dx::heap_properties(D3D12_HEAP_TYPE_UPLOAD);
        device->CreateCommittedResource(
            &heap_properties, D3D12_HEAP_FLAG_NONE, &vb_desc,
            D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&vb_upload)
        );

        void* data;
        vb_upload->Map(0, nullptr, &data);
        memcpy(data, vertices, sizeof(vertices));
        vb_upload->Unmap(0, nullptr);


        Framebuffer& frame = framebuffers[current_frame];

        // Uploading buffer
        frame.cmd_graphics_allocator->Reset();
        ID3D12GraphicsCommandList4* cmd_graphics = frame.cmd_graphics;
        cmd_graphics->Reset(frame.cmd_graphics_allocator, nullptr);
        
        dx::transition(cmd_graphics, vb, 0, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        dx::transition(cmd_graphics, vb_upload, 0, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE);
        cmd_graphics->CopyResource(vb, vb_upload);
        dx::transition(cmd_graphics, vb, 0, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        cmd_graphics->Close();
        queue_graphics->ExecuteCommandLists(1, (ID3D12CommandList**)&cmd_graphics);

        // Signal fence
        queue_graphics->Signal(frame.fence, ++frame.fence_value);
        queue_graphics->Wait(frame.fence, frame.fence_value);

        dx::release(vb_upload);
    }
}

void D3D12GU::shutdown()
{
    dx::release(blit_pipeline);
    dx::release(root_signature);
    dx::release(vb);

    dx::release(framebuffer);
    dx::release(upload_framebuffer);

    dx::release(swap_chain);
    dx::release(dxgi_factory);

    dx::release(rtv_heap);

    for (u32 i = 0; i < DefaultBufferCount; i++)
    {
        dx::release(framebuffers[i].buffer);
        dx::release(framebuffers[i].fence);
        dx::release(framebuffers[i].cmd_graphics_allocator);
        dx::release(framebuffers[i].cmd_graphics);
        CloseHandle(framebuffers[i].fence_event);
    }

    dx::release(queue_graphics);
    dx::release(queue_compute);

#if NDEBUG == 0
    ID3D12DebugDevice* debug_device = nullptr;
    device->QueryInterface(IID_PPV_ARGS(&debug_device));
    debug_device->ReportLiveDeviceObjects(D3D12_RLDO_NONE);
    debug_device->Release();
#endif

    dx::release(device);
}

void D3D12GU::present()
{
    Framebuffer& frame = framebuffers[current_frame];
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

    dx::transition(cmd_graphics, frame.buffer, 0, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    //cmd_graphics->ClearRenderTargetView(frame.handle, clear_color, 0, nullptr);

    cmd_graphics->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_graphics->SetPipelineState(blit_pipeline);
    cmd_graphics->SetGraphicsRootSignature(root_signature);
    cmd_graphics->SetDescriptorHeaps(1, &srv_heap);
    cmd_graphics->SetGraphicsRootDescriptorTable(0, framebuffer_srv);
    D3D12_VERTEX_BUFFER_VIEW vb_view =
    {
        .BufferLocation = vb->GetGPUVirtualAddress(),
        .SizeInBytes = sizeof(vertices),
        .StrideInBytes = sizeof(Vertex)
    };
    cmd_graphics->IASetVertexBuffers(0, 1, &vb_view);

    cmd_graphics->DrawInstanced(6, 1, 0, 0);

    dx::transition(cmd_graphics, frame.buffer, 0, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    cmd_graphics->Close();

    ID3D12CommandList* cmd_list[] = { cmd_graphics };
    queue_graphics->ExecuteCommandLists(1, (ID3D12CommandList**)&cmd_graphics);

    swap_chain->Present(1, 0);
    current_frame = (current_frame + 1) % DefaultBufferCount;

    // Signal fence
    queue_graphics->Signal(frame.fence, ++frame.fence_value);
}

VirtualAddress D3D12GU::create_framebuffer()
{
    D3D12_HEAP_PROPERTIES heap_properties = dx::heap_properties(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC resource_desc =
    {
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
        .Width = GU::MaxDeviceScreenWidth,
        .Height = GU::MaxDeviceScreenHeight,
        .DepthOrArraySize = 1,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc = {.Count = 1},
    };

    (void)device->CreateCommittedResource(
        &heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&framebuffer)
    );

    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc =
    {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
        .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        .Texture2D = {.MipLevels = 1}
    };

    device->CreateShaderResourceView(framebuffer, &view_desc, srv_heap->GetCPUDescriptorHandleForHeapStart());
    framebuffer_srv = srv_heap->GetGPUDescriptorHandleForHeapStart();

    heap_properties = dx::heap_properties(D3D12_HEAP_TYPE_UPLOAD);
    resource_desc = dx::buffer_desc(GU::MaxDeviceScreenWidth * GU::MaxDeviceScreenHeight * 4);

    (void)device->CreateCommittedResource(
        &heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc,
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&upload_framebuffer)
    );

    return VirtualAddress();
}

void D3D12GU::update_framebuffer(void* pixels)
{
    Framebuffer& frame = framebuffers[current_frame];
    if (frame.fence_value != frame.fence->GetCompletedValue())
    {
        frame.fence->SetEventOnCompletion(frame.fence_value, frame.fence_event);
        WaitForSingleObject(frame.fence_event, INFINITE);
    }

    ID3D12GraphicsCommandList4* cmd_graphics = frame.cmd_graphics;
    frame.cmd_graphics_allocator->Reset();
    cmd_graphics->Reset(frame.cmd_graphics_allocator, nullptr);

    void* data;
    upload_framebuffer->Map(0, nullptr, &data);
    memcpy(data, pixels, GU::MaxDeviceScreenWidth * GU::MaxDeviceScreenHeight* 4);
    upload_framebuffer->Unmap(0, nullptr);

    dx::transition(cmd_graphics, upload_framebuffer, 0, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE);
    dx::transition(cmd_graphics, framebuffer, 0, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

    D3D12_TEXTURE_COPY_LOCATION dest =
    {
        .pResource = framebuffer,
        .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = 0,
    };

    D3D12_TEXTURE_COPY_LOCATION src =
    {
        .pResource = upload_framebuffer,
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
    };

    src.PlacedFootprint.Footprint =
    {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .Width = GU::MaxDeviceScreenWidth,
        .Height = GU::MaxDeviceScreenHeight,
        .Depth = 1,
        .RowPitch = GU::MaxDeviceScreenWidth*4
    };

    D3D12_BOX box =
    {
        .left = 0,
        .top = 0,
        .front = 0,
        .right = GU::MaxDeviceScreenWidth,
        .bottom = GU::MaxDeviceScreenHeight,
        .back = 1,
    };

    cmd_graphics->CopyTextureRegion(&dest, 0, 0, 0, &src, &box);
    dx::transition(cmd_graphics, upload_framebuffer, 0, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);
    dx::transition(cmd_graphics, framebuffer, 0, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    cmd_graphics->Close();

    ID3D12CommandList* cmd_list[] = { cmd_graphics };
    queue_graphics->ExecuteCommandLists(1, cmd_list);

    // Signal fence
    HRESULT result = queue_graphics->Signal(frame.fence, ++frame.fence_value);
    queue_graphics->Wait(frame.fence, frame.fence_value);
}

