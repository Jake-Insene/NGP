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

    struct GUQueueState
    {
        // Address In RAM
        VirtualAddress cmd_list;
        // In words
        Word cmd_len;
        GU::QueueState queue_state;

        // Implementation data
        QueueSignal signal;
    };

    static inline GUQueueState state;

    static void initialize();
    static void shutdown();

    static void set_state(VirtualAddress new_cmd_list, Word new_cmd_len, GU::QueueState new_state)
    {
        state.cmd_list = new_cmd_list;
        state.cmd_len = new_cmd_len;
        state.queue_state = new_state;
    }

    static GU::QueueState get_state() { return state.queue_state; }

    static void set_signal(QueueSignal new_signal)
    {
        state.signal = new_signal;
    }

    static void try_execute();
};