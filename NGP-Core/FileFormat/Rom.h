#pragma once
#include "Core/Header.h"

inline constexpr u32 RomSignature = 	0x0050474E;

struct RomHeader {
    u32 magic = RomSignature;
    u32 check_sum;

    // A multiply of 4
    u32 address_of_entry_point;
};
