#include "GPU/GPU.h"

GPUDriver* driver_impl = nullptr;

void GPU::initialize(GPUDriver* impl) {
    driver_impl = impl;

    driver_impl->initialize();
}

void GPU::shutdown() {
    driver_impl->shutdown();
}