/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

static constexpr u32 DisplayWidth = 640;
static constexpr u32 DisplayHeight = 480;

enum class DriverApi {
    D3D12 = 0,
};

struct GPUDriver {
    void(*initialize)();
    void(*shutdown)();
};

struct GPU { 
    static void initialize(DriverApi api);

    static void shutdown();
};
