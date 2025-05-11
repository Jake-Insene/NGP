/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/GU.h"

#include "Video/VGU/VGU.h"

#ifdef _WIN32
#include <Video/D3D12/D3D12GU.h>
#endif // _WIN32

void GU::initialize(GU::DriverApi api)
{
	switch (api)
	{
#ifdef _WIN32
	case DriverApi::D3D12:
		main_driver = D3D12GU::get_driver();
		break;
#endif // _WIN32
	case DriverApi::VGU:
		main_driver = VGPU::get_driver();
		break;
	default:
		break;
	}

	main_driver.initialize();
}

