#include "Platform/Time.h"
#include <Windows.h>

static i64 frequency = 1;
static i64 start = 0;
void Time::initialize() {
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    QueryPerformanceCounter((LARGE_INTEGER*)&start);
}

void Time::shutdown() {}

f64 Time::getTime() {
    return f64(getCounter() - start) / frequency;
}

i64 Time::getCounter() {
    i64 counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return counter;
}

i64 Time::getTimerFrequency() {
    return frequency;
}

