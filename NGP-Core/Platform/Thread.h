/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

using ThreadID = u64;

struct Thread {
    using ThreadFunc = void(*)(void*);

    static ThreadID create(ThreadFunc func, void* arg);

    static void start(ThreadID id);

    static void terminate(ThreadID id);
};
