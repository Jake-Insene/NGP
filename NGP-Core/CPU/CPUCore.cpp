/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/CPUCore.h"

#include "CPU/CPUInterpreter/CPUInterpreter.h"


CPUCore* CPUCore::create_cpu(ImplementationType type)
{
    switch (type)
    {
    case ImplementationType::Unknown:
        break;
    case ImplementationType::Interpreter:
        return new CPUInterpreter();
    default:
        break;
    }
    return nullptr;
}