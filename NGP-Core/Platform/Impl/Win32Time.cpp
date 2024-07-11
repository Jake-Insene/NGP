#include "Platform/Time.h"
#include <Windows.h>

static i64 frequency = 1;
static i64 start = 0;
void Time::initialize() {
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    QueryPerformanceCounter((LARGE_INTEGER*)&start);
}

void Time::shutdown() {}

f64 Time::get_time() {
    return f64(get_counter() - start) / frequency;
}

i64 Time::get_counter() {
    i64 counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return counter;
}

i64 Time::get_timer_frequency() {
    return frequency;
}

