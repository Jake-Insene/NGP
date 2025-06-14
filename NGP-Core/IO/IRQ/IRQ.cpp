/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/IRQ/IRQ.h"

#include "Memory/Bus.h"

namespace IRQ
{

IO::IODevice irq_get_io_device()
{
	return IO::IODevice
	{
		.base_address = IO::IRQ_BASE,

		.read_byte = [](VirtualAddress) -> u8 { return 0; },
		.read_half = [](VirtualAddress) -> u16 { return 0; },
		.read_word = [](VirtualAddress) -> Word { return 0; },
		.read_dword = [](VirtualAddress) -> DWord { return 0; },
		.read_qword = [](VirtualAddress) -> QWord { return QWord(); },

		.write_byte = [](VirtualAddress, u8) {},
		.write_half = [](VirtualAddress, u16) {},
		.write_word = &irq_handle_write_word,
		.write_dword = [](VirtualAddress, DWord) {},
		.write_qword = [](VirtualAddress, QWord) {},
	};
}

IRQRegisters& irq_get_registers()
{
	return *(IRQRegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IO::IRQ_BASE);
}

void irq_handle_write_word(VirtualAddress address, Word value)
{
	switch (address)
	{
	case IRQ_MASK:
		irq_get_registers().irq_mask = value;
		break;
	case IRQ_STATUS:
		irq_get_registers().irq_status = value;
		break;
	}
}

}
