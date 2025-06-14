/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Platform/Header.h"
#include "Video/GU.h"
#include "Video/Math.h"

#include <unordered_map>


struct D3D11GU
{
    static constexpr u32 DefaultBufferCount = 2;
    static constexpr DXGI_FORMAT DefaultFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    struct VFramebuffer
    {
        ID3D11Texture2D* framebuffer;
        ID3D11ShaderResourceView* srv;
        Vector2I size;
    };

    struct GPUState
    {
        IDXGIFactory4* dxgi_factory;
        IDXGISwapChain4* swap_chain;

        D3D_FEATURE_LEVEL feature_level;
        ID3D11Device* device;
        ID3D11DeviceContext* immediate_context;

        ID3D11Resource* rt_buffer;
        ID3D11RenderTargetView* rtv;

        ID3D11InputLayout* input_layout;
        ID3D11VertexShader* vs;
        ID3D11PixelShader* ps;
        ID3D11Buffer* vb;
        ID3D11SamplerState* sampler;

        std::unordered_map<PhysicalAddress, VFramebuffer> vframebuffers;

        u64 current_frame;
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
};

