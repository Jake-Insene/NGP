// --------------------
// Rom.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "Core/Header.h"

inline constexpr u32 RomSignature = 	0x0050474E;

struct RomHeader {
    u32 magic;
    u32 check_sum;

    // A multiply of 4
    u32 target_address;
};
