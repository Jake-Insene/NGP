/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

struct Window
{

    static constexpr u32 DeviceWindowWidth = 960;
    static constexpr u32 DeviceWindowHeight = 540;

    static void initialize(u32 width, u32 height);
    static void shutdown();

    static void update();

    static inline u32 current_width = 0;
    static inline u32 current_height = 0;

    static inline bool is_open = false;
    static inline void* handle = nullptr;
};
