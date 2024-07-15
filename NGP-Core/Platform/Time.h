#pragma once
#include "Core/Header.h"

struct Time {

    static void initialize();
    static void shutdown();
    
    static f64 getTime();

    static i64 getCounter();

    static i64 getTimerFrequency();

};
