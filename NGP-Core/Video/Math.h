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
