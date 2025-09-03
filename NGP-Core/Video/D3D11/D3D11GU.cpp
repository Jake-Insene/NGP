/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/D3D11/D3D11GU.h"

#include "Video/D3D12/DX.h"
#include "Video/Window.h"

#define D3D11_LOGGER(...) { printf("Video/D3D11: "); printf(__VA_ARGS__); putchar('\n'); }


#define DEBUG_OUTPUT(str) OutputDebugStringA(str)

#define DX_ERROR(expr, ...)\
    if((expr) != S_OK)\
    {\
        D3D11_LOGGER(__VA_ARGS__);\
    }

#define DX_EXI_ON_ERROR(expr, ...)\
    if((expr) != S_OK)\
    {\
        D3D11_LOGGER(__VA_ARGS__);\
    }



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


GUDevice::GUDriver D3D11GU::get_driver()
{
    return GUDevice::GUDriver
    {
        .initialize = &D3D11GU::initialize,
        .shutdown = &D3D11GU::shutdown,

        .present_framebuffer = &D3D11GU::present_framebuffer,
        .present = &D3D11GU::present,

        .create_framebuffer = &D3D11GU::create_framebuffer,
        .update_framebuffer = &D3D11GU::update_framebuffer,
    };
}

void D3D11GU::initialize()
{
    state.clear_color[0] = 0.f;
    state.clear_color[1] = 0.f;
    state.clear_color[2] = 0.f;
    state.clear_color[3] = 1.f;

    // Device
    {
        UINT flags =
#if NDEBUG == 0
            D3D11_CREATE_DEVICE_DEBUG;
#else
            0;
#endif

        D3D_FEATURE_LEVEL requested_feature_level = D3D_FEATURE_LEVEL_11_0;
        D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, &requested_feature_level,
            1, D3D11_SDK_VERSION, &state.device, &state.feature_level, &state.immediate_context
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
            state.dxgi_factory->CreateSwapChainForHwnd(state.device, handle, &swap_chain_desc, nullptr, nullptr, &tmp_sc),
            "Couldn't create the display swap chain"
        );

        DX_ERROR(
            tmp_sc->QueryInterface(IID_PPV_ARGS(&state.swap_chain)),
            "Couldn't query IDXGISwapChain"
        );

        tmp_sc->Release();
    }

    // RTV
    {
        state.swap_chain->GetBuffer(0, IID_PPV_ARGS(&state.rt_buffer));
        state.device->CreateRenderTargetView(state.rt_buffer, nullptr, &state.rtv);
    }

    // Vertex buffer
    {
        CD3D11_BUFFER_DESC buffer_desc
        {
            sizeof(vertices), D3D11_BIND_VERTEX_BUFFER,
        };

        D3D11_SUBRESOURCE_DATA initial_data =
        {
            .pSysMem = vertices,
        };

        state.device->CreateBuffer(&buffer_desc, &initial_data, &state.vb);
    }

    // Pipeline
    {
        ID3DBlob* vs_code = {};
        ID3DBlob* ps_code = {};
        ID3DBlob* error_blob = {};
        HRESULT result = S_OK;

        DX_ERROR(
            result = D3DCompileFromFile(L"Assets/blit.vs.d3d11.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs_code, &error_blob),
            "Couldn't create the blit vertex shader\n"
        );
        if (FAILED(result))
        {
            DEBUG_OUTPUT((CHAR*)error_blob->GetBufferPointer());
            dx::release(error_blob);
        }

        DX_ERROR(
            result = D3DCompileFromFile(L"Assets/blit.ps.d3d11.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps_code, &error_blob),
            "Couldn't create the blit pixel shader\n"
        );
        if (FAILED(result))
        {
            DEBUG_OUTPUT((CHAR*)error_blob->GetBufferPointer());
            dx::release(error_blob);
        }

        D3D11_INPUT_ELEMENT_DESC input_elements[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        state.device->CreateInputLayout(
            input_elements, 2, vs_code->GetBufferPointer(), vs_code->GetBufferSize(), &state.input_layout
        );

        D3D11_SAMPLER_DESC sampler_desc =
        {
            .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
            .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
            .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
            .ComparisonFunc = D3D11_COMPARISON_NEVER,
            .MaxLOD = D3D11_FLOAT32_MAX,
        };

        state.device->CreateSamplerState(&sampler_desc, &state.sampler);

        state.device->CreateVertexShader(vs_code->GetBufferPointer(), vs_code->GetBufferSize(), nullptr, &state.vs);
        state.device->CreatePixelShader(ps_code->GetBufferPointer(), ps_code->GetBufferSize(), nullptr, &state.ps);

        dx::release(vs_code);
        dx::release(ps_code);
    }


}

void D3D11GU::shutdown()
{
    for (auto& [key, vfb] : state.vframebuffers)
    {
        dx::release(vfb.framebuffer);
        dx::release(vfb.srv);
    }

    dx::release(state.ps);
    dx::release(state.vs);
    dx::release(state.sampler);
    dx::release(state.input_layout);
    
    dx::release(state.vb);
    
    dx::release(state.rtv);
    dx::release(state.rt_buffer);

    dx::release(state.swap_chain);
    dx::release(state.dxgi_factory);

    dx::release(state.immediate_context);
    dx::release(state.device);
}

void D3D11GU::present_framebuffer(PhysicalAddress fb, bool vsync)
{
    auto it = state.vframebuffers.find(fb);
    if (it == state.vframebuffers.end())
    {
        DEBUG_OUTPUT("Invalid framebuffer address");
        return;
    }
    VFramebuffer& vfb = it->second;

    state.immediate_context->OMSetRenderTargets(1, &state.rtv, nullptr);

    D3D11_VIEWPORT viewport =
    {
        .Width = Window::DefaultWindowWidth,
        .Height = Window::DefaultWindowHeight,
    };
    state.immediate_context->RSSetViewports(1, &viewport);
    state.immediate_context->ClearRenderTargetView(state.rtv, state.clear_color);

    state.immediate_context->IASetInputLayout(state.input_layout);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    state.immediate_context->IASetVertexBuffers(0, 1, &state.vb, &stride, &offset);
    state.immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    state.immediate_context->PSSetShader(state.ps, nullptr, 0);
    state.immediate_context->VSSetShader(state.vs, nullptr, 0);

    state.immediate_context->PSSetSamplers(0, 1, &state.sampler);
    state.immediate_context->PSSetShaderResources(0, 1, &vfb.srv);
    state.immediate_context->Draw(6, 0);

    state.swap_chain->Present(vsync, 0);
}

bool D3D11GU::present(bool vsync)
{
    state.immediate_context->OMSetRenderTargets(1, &state.rtv, nullptr);

    D3D11_VIEWPORT viewport =
    {
        .Width = Window::DefaultWindowWidth,
        .Height = Window::DefaultWindowHeight,
    };
    state.immediate_context->RSSetViewports(1, &viewport);
    state.immediate_context->ClearRenderTargetView(state.rtv, state.clear_color);

    state.swap_chain->Present(vsync, 0);
    return true;
}

PhysicalAddress D3D11GU::create_framebuffer(i32 width, i32 height)
{
    VFramebuffer new_vfb = {};

    new_vfb.size.x = width;
    new_vfb.size.y = height;

    CD3D11_TEXTURE2D_DESC framebuffer_desc
    {
        DefaultFormat, (UINT)width, (UINT)width,
        1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE
    };
    
    DX_ERROR(
        state.device->CreateTexture2D(&framebuffer_desc, nullptr, &new_vfb.framebuffer),
        "Couldn't create framebuffer with size (W: %d, H: %d)", width, height
    );

    state.device->CreateShaderResourceView(new_vfb.framebuffer, nullptr, &new_vfb.srv);

    D3D11_LOGGER("Framebuffer created at address: 0x%016llX, W: %d, H: %d", (u64)new_vfb.framebuffer, width, height);
    
    state.vframebuffers.insert({ (PhysicalAddress)new_vfb.framebuffer, new_vfb });
    return PhysicalAddress(new_vfb.framebuffer);
}

void D3D11GU::update_framebuffer(PhysicalAddress fb, void* va)
{
    auto it = state.vframebuffers.find(fb);
    if (it == state.vframebuffers.end())
    {
        DEBUG_OUTPUT("Invalid framebuffer address");
        return;
    }
    VFramebuffer& vfb = it->second;

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    state.immediate_context->Map(vfb.framebuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, va, vfb.size.x * vfb.size.y * 4);
    state.immediate_context->Unmap(vfb.framebuffer, 0);
}
