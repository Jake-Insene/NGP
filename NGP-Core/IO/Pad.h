/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

namespace IO {

static constexpr u32 MaxPadPort = 4;

enum PadButton {
    PAD_X = 0,
    PAD_Y = 1,
    PAD_Z = 2,
    PAD_W = 3,

    PAD_LEFT = 4,
    PAD_RIGHT = 5,
    PAD_UP = 6,
    PAD_DOWN = 7,

    PAD_SELECT = 8,
    PAD_START = 9,
    PAD_LB = 10,
    PAD_RB = 11,

    PAD_LEFT_AXIS_LEFT = 12,
    PAD_LEFT_AXIS_RIGHT = 13,
    PAD_LEFT_AXIS_UP = 14,
    PAD_LEFT_AXIS_DOWN = 15,

    PAD_RIGHT_AXIS_LEFT = 16,
    PAD_RIGHT_AXIS_RIGHT = 17,
    PAD_RIGHT_AXIS_UP = 18,
    PAD_RIGHT_AXIS_DOWN = 19,
};

void pad_reset();

void pad_update(u32 port, PadButton button, bool down);

}
