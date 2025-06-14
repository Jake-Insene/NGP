/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/Display/Display.h"

#include "Video/GU.h"


namespace Display
{

IO::IODevice display_get_io_device()
{
	return IO::IODevice
	{
		.base_address = IO::DISPLAY_BASE,

		.read_byte = [](VirtualAddress) -> u8 { return 0; },
		.read_half = [](VirtualAddress) -> u16 { return 0; },
		.read_word = [](VirtualAddress) -> Word { return 0; },
		.read_dword = [](VirtualAddress) -> DWord { return 0; },
		.read_qword = [](VirtualAddress) -> QWord { return QWord(); },

		.write_byte = [](VirtualAddress, u8) {},
		.write_half = [](VirtualAddress, u16) {},
		.write_word = &display_handle_write_word,
		.write_dword = [](VirtualAddress, DWord) {},
		.write_qword = [](VirtualAddress, QWord) {},
	};
}

DisplayRegisters& display_get_registers()
{
	return *(DisplayRegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IO::DISPLAY_BASE);
}


void display_handle_write_word(VirtualAddress address, Word value)
{
	switch (address)
	{
	case DISPLAY_CTR:
		display_get_registers().ctr = value;
		break;
	case DISPLAY_BUFFER_ADDR:
		if (display_get_registers().ctr & DISPLAY_ENABLE)
		{
			GU::display_set_address(value);
			display_get_registers().buffer_addr = value;
		}
		break;
	case DISPLAY_FORMAT:
		if (display_get_registers().ctr & DISPLAY_ENABLE)
		{
			GU::display_set_config(
				value & 0x3FFF, (value >> 14) & 0x3FFF, 
				(Display::DisplayFormat)(value >> 28)
			);
			display_get_registers().format = value;
		}
		break;
	case DISPLAY_PRESENT:
		if (display_get_registers().ctr & DISPLAY_ENABLE)
		{
			GU::request_present();
		}
		break;
	default:
		break;
	}
}

}