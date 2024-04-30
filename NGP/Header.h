#pragma once

using i8 = char;
using u8 = unsigned char;
using i16 = short;
using u16 = unsigned short;
using i32 = int;
using u32 = unsigned int;
using i64 = long long;
using u64 = unsigned long long;

using f32 = float;
using f64 = double;

using word = u32;

enum ErrorCodes : i32 {
    STATUS_OK = 0,
    INVALID_ARGUMENTS = -1,
    ERROR = -2,
};
