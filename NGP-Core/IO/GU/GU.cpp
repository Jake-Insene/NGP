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

static void gu_irq_mask(CPUCore& core, VirtualAddress value)
{

}

static void gu_set_status(CPUCore& core, VirtualAddress value)
{

}

static void gu_set_ctr(CPUCore& core, VirtualAddress value)
{

}

GURegisters& get_gu_registers()
{
    return *(GURegisters*)(Bus::MAPPED_BUS_ADDRESS_START + GU_BASE);
}

void gu_handle_write_word(CPUCore& core, VirtualAddress address, Word value)
{
    switch (address)
    {
    case GU_IRQ_MASK:
        gu_irq_mask(core, value);
        break;
    case GU_STATUS:
        gu_set_status(core, value);
        break;
    case GU_CTR:
        gu_set_ctr(core, value);
        break;
    case GU_ID:
        // Ignored
        break;
    case GU_DISPLAYADR:
        if (GU::check_vram_address(value) == Bus::ValidVirtualAddress)
        {
            GU::display_set(value, 0, 0, GU::GU_DISPLAY_FMT_NONE);
            get_gu_registers().display_address = value;
        }
        break;
    case GU_DISPLAYFMT:
        GU::display_set(value, value & 0xFFF, (value >> 12) & 0xFFF, (GU::GUDisplayFormat)(value >> 24));
        get_gu_registers().display_format = value;
        break;
    case GU_TRANSFER_INADDR:
    case GU_TRANSFER_INFMT:
    case GU_TRANSFER_OUTADDR:
    case GU_TRANSFER_OUTFMT:
    case GU_TRANSFER_COUNT:
    case GU_TRANSFER_CTR:
    case GU_TRANSFER_IRQ_MASK:
        break;
    case GU_TRANSFER_ID:
        // Ignored
        break;
    }
}


}