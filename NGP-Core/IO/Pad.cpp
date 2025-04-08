/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/Pad.h"

namespace IO
{

struct PadInfo
{
    u32 buttons;
    i8 la_x;
    i8 la_y;
    i8 ra_x;
    i8 ra_y;

    u32 status;
    u32 port;
    // RAM address of sync buffer
    u32 sync_buffer;
} pads[MaxPadPort];


void pad_reset()
{
    for (u32 i = 0; i < MaxPadPort; i++)
    {
        pads[i].port = i;
        pads[i].buttons = 0;
        pads[i].la_x = 0;
        pads[i].la_y = 0;
        pads[i].ra_x = 0;
        pads[i].ra_y = 0;
        pads[i].status = 0;
    }
}

void pad_update(u32 port, PadButton button, bool down)
{
    PadInfo& pad = pads[port];
    if (down)
    {
        if (button < PAD_LEFT_AXIS_LEFT)
        {
            // Masking the button bit
            pad.buttons |= (1 << button);
        }
        else
        {
            // set axis
        }
    }
    else
    {
        if (button < PAD_LEFT_AXIS_LEFT)
        {
            // Unmasking the button bit
            pad.buttons &= ~(1 << button);
        }
    }
}

}
