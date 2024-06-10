#pragma once
#include "Header.h"

inline constexpr u32 RomSignature = 	0x0050474E;
inline constexpr u32 RamBaseAddress = 	0x00000000;
inline constexpr u32 BiosBaseAddress = 	0x08000000;
inline constexpr u32 RomBaseAddress = 	0x08200000;

struct RomHeader {
    u32 magic = RomSignature;
    // A multiply of 4
    u32 address_of_entry_point;

    u32 check_sum;
};
