/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/Thread.h"
#include "Platform/Header.h"

ThreadID Thread::create(Thread::ThreadFunc func, void* arg) {
    HANDLE id = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)func, arg, CREATE_SUSPENDED, 0);
    if (id) {
        return reinterpret_cast<ThreadID>(id);
    }

    return ThreadID(-1);
}

void Thread::suspend(ThreadID id) {
    SuspendThread(HANDLE(id));
}

void Thread::resume(ThreadID id) {
    ResumeThread(HANDLE(id));
}

void Thread::terminate(ThreadID id) {
    TerminateThread(HANDLE(id), DWORD(-1));
}
