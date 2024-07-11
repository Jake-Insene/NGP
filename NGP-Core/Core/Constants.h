#pragma once
#include "Core/Header.h"

// RAM	0        - 07FFFFFF = 128 MB
// BIOS	08000000 - 081FFFFF = 2 MB
// ROM	08200000 - 0FFFFFFF = 3965 MB / 3.8 GB

static constexpr u32 RAMStart =     0x0000'0000;
static constexpr u32 RAMEnd =       0x07FF'FFFF;
static constexpr u32 BIOSStart =    0x0800'0000;
static constexpr u32 BIOSEnd =      0x081F'FFFF;
static constexpr u32 ROMStart =     0x0820'0000;
static constexpr u32 ROMEnd = 		0xEFFF'FFFF;

static constexpr u32 ClockSpeed = MHZ(300);
