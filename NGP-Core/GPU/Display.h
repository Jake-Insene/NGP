#pragma once
#include "Core/Header.h"

struct Display {
    static void initialize(u32 width, u32 height);
    static void shutdown();

    static void update();

    static inline u32 current_width = 0;
    static inline u32 current_height = 0;

    static inline bool is_open = false;
    static inline void* handle = nullptr;
};
