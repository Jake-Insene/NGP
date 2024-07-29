/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/GPU.h"

GPUDriver driver_impl = {};

void GPU::initialize(GPUDriver impl) {
    driver_impl = impl;

    driver_impl.initialize();
}

void GPU::shutdown() {
    driver_impl.shutdown();
}
