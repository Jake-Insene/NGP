/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/GU/GU.h"

#include "CPU/CPUCore.h"
#include "Memory/Bus.h"
#include "Video/GU.h"


namespace IO
{

static void gu_irq_mask(VirtualAddress value)
{
}

static void gu_set_status(VirtualAddress value)
{
}

static void gu_set_ctr(VirtualAddress value)
{
}

GURegisters& get_gu_registers()
{
    return *(GURegisters*)(Bus::MAPPED_BUS_ADDRESS_START + GU_BASE);
}

void gu_handle_write_word(VirtualAddress address, Word value)
{
    switch (address)
    {
    case GU_IRQ_MASK:
        gu_irq_mask(value);
        break;
    case GU_IRQ_STATUS:
        gu_set_status(value);
        break;
    case GU_CTR:
        gu_set_ctr(value);
        break;
    case GU_ID:
        // Ignored
        break;
    case GU_TRANSFER_INADDR:
    case GU_TRANSFER_INFMT:
    case GU_TRANSFER_OUTADDR:
    case GU_TRANSFER_OUTFMT:
    case GU_TRANSFER_CTR:
    case GU_TRANSFER_IRQ_MASK:
        break;
    case GU_TRANSFER_ID:
        // Ignored
        break;
    case GU_QUEUE_CMD:
    case GU_QUEUE_ADDR:
    case GU_QUEUE_CTR:
    case GU_QUEUE_STATE:
        break;
    }
}


}