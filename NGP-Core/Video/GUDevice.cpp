/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/GUDevice.h"

#include "Video/OpenGL/GLGU.h"
#include "Video/VGU/VGU.h"

#ifdef _WIN32
#include <Video/D3D11/D3D11GU.h>
#include <Video/D3D12/D3D12GU.h>
#endif // _WIN32

void GUDevice::initialize(GUDevice::DriverApi api)
{
	switch (api)
	{
#ifdef _WIN32
	case GUDevice::D3D12:
		main_driver = D3D12GU::get_driver();
		break;
#endif // _WIN32
	case GUDevice::VGU:
		main_driver = VGU::get_driver();
		break;
	case GUDevice::OPENGL:
		main_driver = GLGU::get_driver();
		break;
	default:
		break;
	}

	main_driver.initialize();
}

