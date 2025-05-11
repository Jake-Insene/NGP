/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/Thread.h"

#include <pthread.h>


ThreadID Thread::create(ThreadFunc func, void* arg)
{
    pthread_t pid;
    using PThreadFuncType = void*(*)(void*);
    pthread_create(&pid, nullptr, PThreadFuncType(func), arg);

    return ThreadID(pid);
}

void Thread::suspend(ThreadID id)
{
    pthread_cancel(id);
}
    
void Thread::resume(ThreadID id)
{
}

void Thread::terminate(ThreadID id)
{
    pthread_detach(id);
}
