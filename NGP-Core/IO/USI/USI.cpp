/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/USI/USI.h"

IO::IODevice USI::get_io_device()
{
	return IO::IODevice
	{
		.base_address = IO::DMA_BASE,

		.initialize = &USI::initialize,
		.shutdown = &USI::shutdown,
		.dispatch = &USI::dispatch,

        .read_byte = [](VirtualAddress) -> u8 { return 0; },
        .read_half = [](VirtualAddress) -> u16 { return 0; },
        .read_word = [](VirtualAddress) -> Word { return 0; },
        .read_dword = [](VirtualAddress) -> DWord { return 0; },
        .read_qword = [](VirtualAddress) -> QWord { return QWord(); },

        .write_byte = [](VirtualAddress, u8) {},
        .write_half = [](VirtualAddress, u16) {},
        .write_word = &USI::handle_write_word,
        .write_dword = [](VirtualAddress, DWord) {},
        .write_qword = [](VirtualAddress, QWord) {},
	};
}

void USI::initialize() {}
void USI::shutdown() {}
void USI::dispatch() {}

void USI::handle_write_word(VirtualAddress local_addr, Word value)
{
    switch (local_addr)
    {
    case USI_IRQ_MASK:
        get_registers().irq_mask = value;
        break;
    case USI_IRQ_STATUS:
        break;
    case USI_CTR:
        get_registers().ctr = value;
        break;
    case USI_ID:
        break;
    case USI_CMD:
        get_registers().cmd = value;
        break;
    case USI_ADDR:
        get_registers().addr = value;
        break;
    case USI_BLOCK_ADDR:
        get_registers().block_addr = value;
        break;
    case USI_PORT_SELECT:
        get_registers().port_select = value;
        break;
    }
}
