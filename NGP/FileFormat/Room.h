#pragma once
#include "Header.h"

inline constexpr u32 DEFAULT_STACK_SIZE = 0xFFFF;

inline constexpr u32 VERSION_MAJOR = 1;
inline constexpr u32 VERSION_MINOR = 0;

struct RoomFunction {
    u8 locals;
    u8 arguments;

    u32 size_of_code;
};

struct RoomHeader {
    u32 magic = 'NGP\0';

    u16 major_version = VERSION_MAJOR;
    u16 minor_version = VERSION_MINOR;

    u32 stack_reserve = DEFAULT_STACK_SIZE;
    u16 data_reserve;
    u16 raw_data;

    u32 functions;
    u32 entry_point;
};
