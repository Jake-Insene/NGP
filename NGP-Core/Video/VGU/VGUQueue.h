/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Video/GUDevice.h"
#include "Video/Math.h"

struct VGUQueue
{
    enum QueueSignal
    {
        QUEUE_SIGNAL_IDLE = 0,
        QUEUE_SIGNAL_RUN = 1,
    };
    
    VirtualAddress cmd_list;
    // In words
    Word cmd_len;
    GU::QueueState state;

    // Implementation data
    QueueSignal signal;

    void set_signal(QueueSignal new_signal)
    {
        signal = new_signal;
    }

    void try_execute();
};