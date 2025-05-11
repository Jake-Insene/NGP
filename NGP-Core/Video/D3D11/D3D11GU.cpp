/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/D3D11/D3D11GU.h"

#include "Video/D3D12/DX.h"
#include "Video/Window.h"


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


GU::GUDriver D3D11GU::get_driver()
{
    return GU::GUDriver
    {
        .initialize = &D3D11GU::initialize,
        .shutdown = &D3D11GU::shutdown,

        .present = &D3D11GU::present,
        .create_framebuffer = &D3D11GU::create_framebuffer,
        .update_framebuffer = &D3D11GU::update_framebuffer,
    };
}

void D3D11GU::initialize()
{
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
            1, D3D11_SDK_VERSION, &device, &feature_level, &immediate_context
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
            dxgi_factory->CreateSwapChainForHwnd(device, handle, &swap_chain_desc, nullptr, nullptr, &tmp_sc),
            "Couldn't create the display swap chain"
        );

        DX_ERROR(
            tmp_sc->QueryInterface(IID_PPV_ARGS(&swap_chain)),
            "Couldn't query IDXGISwapChain"
        );

        tmp_sc->Release();
    }

    // RTV
    {
        swap_chain->GetBuffer(0, IID_PPV_ARGS(&rt_buffer));
        device->CreateRenderTargetView(rt_buffer, nullptr, &rtv);
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

        device->CreateBuffer(&buffer_desc, &initial_data, &vb);
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

        device->CreateInputLayout(
            input_elements, 2, vs_code->GetBufferPointer(), vs_code->GetBufferSize(), &input_layout
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

        device->CreateSamplerState(&sampler_desc, &sampler);

        device->CreateVertexShader(vs_code->GetBufferPointer(), vs_code->GetBufferSize(), nullptr, &vs);
        device->CreatePixelShader(ps_code->GetBufferPointer(), ps_code->GetBufferSize(), nullptr, &ps);

        dx::release(vs_code);
        dx::release(ps_code);
    }


}

void D3D11GU::shutdown()
{
    dx::release(framebuffer_srv);
    dx::release(framebuffer);

    dx::release(ps);
    dx::release(vs);
    dx::release(sampler);
    dx::release(input_layout);
    
    dx::release(vb);
    
    dx::release(rtv);
    dx::release(rt_buffer);

    dx::release(swap_chain);
    dx::release(dxgi_factory);

    dx::release(immediate_context);
    dx::release(device);
}

void D3D11GU::present(bool vsync)
{
    immediate_context->OMSetRenderTargets(1, &rtv, nullptr);

    D3D11_VIEWPORT viewport =
    {
        .Width = Window::DefaultWindowWidth,
        .Height = Window::DefaultWindowHeight,
    };
    immediate_context->RSSetViewports(1, &viewport);
    FLOAT clear_color[] = {1.f, 1.f, 0.f, 1.f};
    immediate_context->ClearRenderTargetView(rtv, clear_color);

    immediate_context->IASetInputLayout(input_layout);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    immediate_context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->PSSetShader(ps, nullptr, 0);
    immediate_context->VSSetShader(vs, nullptr, 0);

    immediate_context->PSSetSamplers(0, 1, &sampler);
    immediate_context->PSSetShaderResources(0, 1, &framebuffer_srv);
    immediate_context->Draw(6, 0);

    swap_chain->Present(vsync, 0);
}

VirtualAddress D3D11GU::create_framebuffer()
{
    CD3D11_TEXTURE2D_DESC framebuffer_desc
    {
        DefaultFormat, GU::MaxDeviceScreenWidth, GU::MaxDeviceScreenHeight, 
        1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE
    };
    device->CreateTexture2D(&framebuffer_desc, nullptr, &framebuffer);

    device->CreateShaderResourceView(framebuffer, nullptr, &framebuffer_srv);

    return VirtualAddress();
}

void D3D11GU::update_framebuffer(void* pixels)
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    immediate_context->Map(framebuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, pixels, GU::MaxDeviceScreenWidth * GU::MaxDeviceScreenHeight * 4);
    immediate_context->Unmap(framebuffer, 0);
}
