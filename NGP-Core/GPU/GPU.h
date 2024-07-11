#pragma once

struct GPUDriver {
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
};

struct GPU {
    static void initialize(GPUDriver* impl);

    static void shutdown();
};
