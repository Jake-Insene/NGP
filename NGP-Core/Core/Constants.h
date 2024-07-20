// --------------------
// Constants.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "Core/Header.h"

// BIOS	00000000 - 00400000 = 4 MB
// RAM	10000000 - 20000000 = 256 MB


static constexpr u32 BIOSStart  =   0x0000'0000;
static constexpr u32 BIOSEnd    =   0x003F'FFFF;
static constexpr u32 RAMStart   =   0x1000'0000;
static constexpr u32 RAMEnd     =   0x1FFF'FFFF;
static constexpr u32 ROMStart   =   0x2000'0000;

static constexpr u32 CyclesPerSecond = MHZ(300);
static constexpr u32 FramesPerSecond = 60;
static constexpr u32 CyclesPerFrame = CyclesPerSecond / FramesPerSecond;

#define NGP_VERSION_MAJOR "1"
#define NGP_VERSION_MINOR "0"
#define NGP_VERSION_PATCH "0"

#define NGP_VERSION NGP_VERSION_MAJOR "." NGP_VERSION_MINOR "." NGP_VERSION_PATCH




