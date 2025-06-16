/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/IRQ/IRQ.h"

#include "Memory/Bus.h"


IO::IODevice IRQ::get_io_device()
{
	return IO::IODevice
	{
		.base_address = IO::IRQ_BASE,

		.initialize = &IRQ::initialize,
		.shutdown = &IRQ::shutdown,
		.dispatch = []() {},

		.read_byte = [](VirtualAddress) -> u8 { return 0; },
		.read_half = [](VirtualAddress) -> u16 { return 0; },
		.read_word = [](VirtualAddress) -> Word { return 0; },
		.read_dword = [](VirtualAddress) -> DWord { return 0; },
		.read_qword = [](VirtualAddress) -> QWord { return QWord(); },

		.write_byte = [](VirtualAddress, u8) {},
		.write_half = [](VirtualAddress, u16) {},
		.write_word = &IRQ::handle_write_word,
		.write_dword = [](VirtualAddress, DWord) {},
		.write_qword = [](VirtualAddress, QWord) {},
	};
}

void IRQ::initialize()
{}

void IRQ::shutdown()
{}

void IRQ::handle_write_word(VirtualAddress local_address, Word value)
{
	switch (local_address)
	{
	case IRQ_MASK:
		get_registers().irq_mask = value;
		break;
	case IRQ_STATUS:
		get_registers().irq_status = value;
		break;
	}
}

