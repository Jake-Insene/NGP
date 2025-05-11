/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Platform/Header.h"
#include "Video/GU.h"

struct D3D11GU
{
    static constexpr u32 DefaultBufferCount = 2;
    static constexpr DXGI_FORMAT DefaultFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    static inline IDXGIFactory4* dxgi_factory = nullptr;
    static inline IDXGISwapChain4* swap_chain = nullptr;

    static inline D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_1_0_CORE;
    static inline ID3D11Device* device = nullptr;
    static inline ID3D11DeviceContext* immediate_context = nullptr;

    static inline ID3D11Resource* rt_buffer = nullptr;
    static inline ID3D11RenderTargetView* rtv = nullptr;

    static inline ID3D11InputLayout* input_layout = nullptr;
    static inline ID3D11VertexShader* vs = nullptr;
    static inline ID3D11PixelShader* ps = nullptr;
    static inline ID3D11Buffer* vb = nullptr;
    static inline ID3D11SamplerState* sampler = nullptr;
    static inline ID3D11Texture2D* framebuffer = nullptr;
    static inline ID3D11ShaderResourceView* framebuffer_srv = {};

    static inline u64 current_frame = 0;

    static inline FLOAT clear_color[4] =
    {
        0.f, 0.f, 0.f, 1.f
    };

    static GU::GUDriver get_driver();

    static void initialize();
    static void shutdown();

    static void present(bool vsync);

    static VirtualAddress create_framebuffer();
    static void update_framebuffer(void* pixels);
};

