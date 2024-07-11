#pragma once

#define KB(n) (1024*n)
#define MB(n) (KB(1024)*n)

#define MHZ(n) n * 1'000'000
#define GHZ(n) n * 1'000'000'000

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

using Word = u32;

enum ERROR_CODES : i32 {
    STATUS_OK = 0,
    INVALID_ARGUMENTS = -1,
    ERROR = -2,
    CORRUPT = -3,
};

[[nodiscard]] constexpr u32 align_up(u32 size, u16 alignment) {
    return (size + alignment - 1) & -(alignment);
}

[[nodiscard]] constexpr u32 align_down(u32 size, u16 alignment) {
    return size & ~(alignment - 1);
}

[[nodiscard]] constexpr u32 sign_ext(u32 imm, u8 imm_width, u8 extend_size) {
    u8 sign = (imm >> (imm_width - 1)) & 1;
    u32 extended = imm;

    if (sign) {
        u32 extension_mask = ((1 << (extend_size - imm_width)) - 1) << imm_width;
        extended = imm | extension_mask;
    }

    return extended;
}
