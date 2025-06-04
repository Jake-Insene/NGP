/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/Time.h"
#include "Platform/Header.h"

static i64 start = 0;
static i64 frequency = 0;

void Time::initialize()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&start);
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
}


f64 Time::get_time()
{
	i64 end;
	QueryPerformanceCounter((LARGE_INTEGER*)&end);
	return f64(end - start) / frequency;
}
