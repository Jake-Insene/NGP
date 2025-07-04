/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "CPU/CPUCore.h"

#if defined(NGP_BUILD_V1)
#include "CPU/NGPV1/NGPV1.h"
struct BuildConfig
{
	using CoreType = NGPV1;
	static constexpr CPUCore::CPUType CPUType = CPUCore::CPUType::V1;
	static constexpr Word CoreCount = 1;
	static constexpr usize ClockSpeed = MHZ(50);

	static constexpr Word RAMSize = MB(32);
	static constexpr Word VRAMSize = MB(16);
};
#endif
