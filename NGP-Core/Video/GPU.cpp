/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/GPU.h"


#ifdef _WIN32
#include <Video/D3D12/D3D12GPU.h>
#endif // _WIN32

GPUDriver driver_impl = {};

void GPU::initialize(DriverApi api) {
	switch (api) {
#ifdef _WIN32
	case DriverApi::D3D12:
		driver_impl = D3D12GPU::get();
#endif // _WIN32
		break;
	default:
		break;
	}

    driver_impl.initialize();
}

void GPU::shutdown() {
    driver_impl.shutdown();
}
