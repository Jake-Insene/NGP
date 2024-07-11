#pragma once
#include "Core/Header.h"

inline constexpr u32 RomSignature = 	0x0050474E;
inline constexpr u32 BiosBaseAddress = 	0x00000000;
inline constexpr u32 RamBaseAddress = 	0x01000000;
inline constexpr u32 RomBaseAddress = 	0x08200000;

struct RomHeader {
    u32 magic = RomSignature;
    u32 check_sum;

    // A multiply of 4
    u32 address_of_entry_point;
};
