/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/Pad/Pad.h"

#include "Memory/Bus.h"


IO::IODevice Pad::get_io_device()
{
    return IO::IODevice
    {
        .base_address = IO::PAD_BASE,

        .initialize = &initialize,
        .shutdown = &shutdown,
        .dispatch = []() {},

        .read_byte = [](VirtualAddress) -> u8 { return 0; },
        .read_half = [](VirtualAddress) -> u16 { return 0; },
        .read_word = [](VirtualAddress) -> Word { return 0; },
        .read_dword = [](VirtualAddress) -> DWord { return 0; },
        .read_qword = [](VirtualAddress) -> QWord { return QWord(); },

        .write_byte = [](VirtualAddress, u8) {},
        .write_half = [](VirtualAddress, u16) {},
        .write_word = &handle_write_word,
        .write_dword = [](VirtualAddress, DWord) {},
        .write_qword = [](VirtualAddress, QWord) {},
    };
}

void Pad::initialize()
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

void Pad::shutdown()
{}

void Pad::update(u32 port, PadButton button, bool down)
{
    PadInfo& pad = pads[port];
    if (down)
    {
        if (button < PAD_LEFT_AXIS_LEFT)
        {
            // Masking the button bit
            if(port == 0) 
            {
                get_main_pad().buttons |= (1 << button);
            }
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
            if (port == 0)
            {
                get_main_pad().buttons &= ~(1 << button);
            }
            pad.buttons &= ~(1 << button);
        }
    }
}


void Pad::handle_write_word(VirtualAddress local_address, Word value)
{
}

