/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/Time.h"

#include <time.h>


void Time::initialize()
{

}

void Time::shutdown()
{

}


f64 Time::get_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

i64 Time::get_counter()
{
    return 0;
}

i64 Time::get_timer_frequency()
{
    return 0;
}
