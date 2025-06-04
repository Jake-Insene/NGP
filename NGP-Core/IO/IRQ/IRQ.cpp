/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/IRQ/IRQ.h"

#include "Memory/Bus.h"

namespace IO
{
IRQRegisters& get_irq_registers()
{
	return *(IRQRegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IRQ_BASE);
}

void irq_handle_write_word(CPUCore& core, VirtualAddress address, Word value)
{
	switch (address)
	{
	case IRQ_MASK:
		get_irq_registers().irq_mask = value;
		break;
	case IRQ_STATUS:
		get_irq_registers().irq_status = value;
		break;
	}
}

}
