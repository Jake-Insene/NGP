/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"
#include <cmath>

struct Vector2
{
    f32 x, y;

    [[nodiscard]] constexpr Vector2 operator+(const Vector2& v) const
    {
        return Vector2(x + v.x, y + v.y);
    }
    
    [[nodiscard]] constexpr Vector2 operator-(const Vector2& v) const 
    { 
        return Vector2(x - v.x, y - v.y); 
    }

    [[nodiscard]] constexpr Vector2 operator*(const Vector2& v) const
    {
        return Vector2(x * v.x, y * v.y);
    }

    [[nodiscard]] constexpr Vector2 operator*(const f32 v) const
    {
        return Vector2(x * v, y * v);
    }

    [[nodiscard]] constexpr Vector2 operator/(const Vector2& v) const
    {
        return Vector2(x / v.x, y / v.y);
    }

    [[nodiscard]] constexpr Vector2 operator/(const f32 v) const
    {
        return Vector2(x / v, y / v);
    }

    [[nodiscard]] constexpr void operator/=(const f32 v)
    {
        x /= v;
        y /= v;
    }

    [[nodiscard]] const f32 lenght() const { return std::sqrt(x*x + y*y); }
};

struct Vector2I
{
    i32 x, y;
};

struct Vector2I16
{
    i16 x, y;
};

struct Vector2I8
{
    i8 x, y;
};

struct Vector3
{
    f32 x, y, z;
};

union Color
{
    struct
    {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };

    Word rgba;
};


template<typename T>
T min(T x, T y)
{
    return x > y ? y : x;
}

template<typename T>
T max(T x, T y)
{
    return x > y ? x : y;
}