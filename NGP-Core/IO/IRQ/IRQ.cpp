/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/IRQ/IRQ.h"

#include "IO/IORegisters.h"

namespace IO
{


void irq_handle_write_word(CPUCore& core, VirtualAddress address, Word value)
{
	switch (address)
	{
	case IRQ_STATUS:
		get_io_registers().irq.irq_status = value;
		break;
	case IRQ_ENABLE_MASK:
		get_io_registers().irq.enable_mask = value;
	}
}

}
