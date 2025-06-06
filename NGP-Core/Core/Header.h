/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once

#define NGP_VERSION_MAJOR "1"
#define NGP_VERSION_MINOR "0"
#define NGP_VERSION_PATCH "0"

#define NGP_VERSION NGP_VERSION_MAJOR "." NGP_VERSION_MINOR "." NGP_VERSION_PATCH

#define KB(n) (1024U*n)
#define MB(n) (KB(1024U)*n)

#define MHZ(n) n * 1'000'000U

using i8 = char;
using u8 = unsigned char;
using i16 = short;
using u16 = unsigned short;
using i32 = int;
using u32 = unsigned int;
using i64 = long long;
using u64 = unsigned long long;

using isize = i64;
using usize = u64;

using f32 = float;
using f64 = double;

using Word = u32;
using DWord = u64;

using VirtualAddress = u32;
using PhysicalAddress = u64;

#pragma warning(disable : 4201)

union QWord
{
    Word w[4];
    u64 dw[2];
    struct
    {
        u64 lo;
        u64 hi;
    };
};

#if defined(_MSVC_LANG)
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__)
#define FORCE_INLINE __attribute__((always_inline))
#endif // _MSVC_LANG

[[nodiscard]] constexpr u32 align_up(u32 size, u16 alignment)
{
    return (size + alignment - 1) & -(alignment);
}

[[nodiscard]] constexpr u32 align_down(u32 size, u16 alignment)
{
    return size & ~(alignment - 1);
}

[[nodiscard]] constexpr usize bits_of(usize val)
{
    usize counter = 0;

    while ((val & 1))
    {
        counter++;
        val >>= 1;
    }

    return counter;
}

