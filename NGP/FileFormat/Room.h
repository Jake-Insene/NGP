#pragma once
#include "Header.h"

inline constexpr u16 DEFAULT_STACK_SIZE = 0xFF;
inline constexpr u32 MAGIC_NUMBER = 0x0050474E;

struct RoomHeader {
    u32 magic = MAGIC_NUMBER;

    u32 size_of_raw_data;
    u32 address_of_entry_point;
};
