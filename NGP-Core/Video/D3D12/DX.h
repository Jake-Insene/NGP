#pragma once
#include "Platform/Header.h"
#include <stdio.h>

#define DEBUG_OUTPUT(str) OutputDebugStringA(str)

#define DX_ERROR(expr, ...)\
    if((expr) != S_OK)\
    {\
        char buffer[256]{};\
        snprintf(buffer, sizeof(buffer), __VA_ARGS__);\
        OutputDebugStringA(buffer);\
    }

#define DX_EXI_ON_ERROR(expr, ...)\
    if((expr) != S_OK)\
    {\
        char buffer[256]{};\
        snprintf(buffer, sizeof(buffer), __VA_ARGS__);\
        OutputDebugStringA(buffer);\
        exit(-1);\
    }


namespace dx
{

    template<typename T>
    inline void release(T*& resource)
    {
        if (resource)
        {
            resource->Release();
            resource = nullptr;
        }
    }

    inline D3D12_RESOURCE_BARRIER transition_barrier(ID3D12Resource* resource, UINT subresource,
        D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
    {
        D3D12_RESOURCE_BARRIER barrier =
        {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition =
            {
                .pResource = resource,
                .Subresource = subresource,
                .StateBefore = before,
                .StateAfter = after
            }
        };

        return barrier;
    }


    inline D3D12_RESOURCE_DESC buffer_desc(usize len)
    {
        return D3D12_RESOURCE_DESC
        {
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
            .Width = len,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .SampleDesc = {.Count = 1},
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR
        };
    }

    inline D3D12_HEAP_PROPERTIES heap_properties(D3D12_HEAP_TYPE type)
    {
        return D3D12_HEAP_PROPERTIES
        {
            .Type = type,
        };
    }
}