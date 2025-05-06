/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/GU.h"

#include "Video/vGPU/VGPU.h"

#ifdef _WIN32
#include <Video/D3D12/D3D12GU.h>
#endif // _WIN32

GPUDriver driver_impl = {};

void GU::initialize(GU::DriverApi api)
{
	switch (api)
	{
#ifdef _WIN32
	case DriverApi::D3D12:
		driver_impl = D3D12GU::get_driver();
#endif // _WIN32
		break;
	case DriverApi::VGPU:
		driver_impl = VGPU::get_driver();
	default:
		break;
	}

	driver_impl.initialize();
}

void GU::shutdown()
{
	driver_impl.shutdown();
}

void GU::present()
{
	driver_impl.present();
}
