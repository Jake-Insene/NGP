/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/CPUCore.h"

#include "CPU/NGPV1/NGPV1.h"


CPUCore* CPUCore::create_cpu(CPUType type)
{
    switch (type)
    {
    case CPUType::Unknown:
        break;
    case CPUType::V1:
        return new NGPV1();
    default:
        break;
    }
    return nullptr;
}