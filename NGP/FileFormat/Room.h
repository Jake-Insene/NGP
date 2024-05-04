#pragma once
#include "Header.h"

inline constexpr u16 DEFAULT_STACK_SIZE = 0xFF;

struct RoomHeader {
    u32 magic = 'NGP\0';

    u16 stack_reserve = DEFAULT_STACK_SIZE;
    u16 sp = DEFAULT_STACK_SIZE;

    u32 size_of_raw_data;
    u32 address_of_entry_point;
};
