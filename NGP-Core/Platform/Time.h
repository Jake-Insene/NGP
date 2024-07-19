// --------------------
// Time.h
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#pragma once
#include "Core/Header.h"

struct Time {

    static void initialize();
    static void shutdown();
    
    static f64 getTime();

    static i64 getCounter();

    static i64 getTimerFrequency();

};
