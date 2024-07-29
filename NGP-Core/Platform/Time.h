/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

struct Time {

    static void initialize();
    static void shutdown();
    
    static f64 get_time();

    static i64 get_counter();

    static i64 get_timer_frequency();

};
