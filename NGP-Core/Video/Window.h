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
    static constexpr i32 DefaultWindowWidth = 1080;
    static constexpr i32 DefaultWindowHeight = 720;

    static void initialize(i32 width, i32 height);
    static void shutdown();

    static void update();

    static inline i32 current_width = 0;
    static inline i32 current_height = 0;

    static inline bool is_open = false;
    static inline void* handle = nullptr;
};
