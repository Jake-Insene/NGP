/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/Display/Display.h"

#include "Video/GUDevice.h"


IO::IODevice Display::get_io_device()
{
	return IO::IODevice
	{
		.base_address = IO::DISPLAY_BASE,

		.initialize = &Display::initialize,
		.shutdown = &Display::shutdown,
		.dispatch = []() {},

		.read_byte = [](VirtualAddress) -> u8 { return 0; },
		.read_half = [](VirtualAddress) -> u16 { return 0; },
		.read_word = [](VirtualAddress) -> Word { return 0; },
		.read_dword = [](VirtualAddress) -> DWord { return 0; },
		.read_qword = [](VirtualAddress) -> QWord { return QWord(); },

		.write_byte = [](VirtualAddress, u8) {},
		.write_half = [](VirtualAddress, u16) {},
		.write_word = &Display::handle_write_word,
		.write_dword = [](VirtualAddress, DWord) {},
		.write_qword = [](VirtualAddress, QWord) {},
	};
}

void Display::initialize()
{
	get_registers().id = DISPLAY_1;
}

void Display::shutdown()
{}


void Display::handle_write_word(VirtualAddress local_address, Word value)
{
	switch (local_address)
	{
	case DISPLAY_CTR:
	{
		get_registers().ctr = value;

		if (value & PRESENT)
		{
			GUDevice::request_present();
		}
	}
		break;
	case DISPLAY_BUFFER_ADDR:
		if (get_registers().ctr & ENABLE)
		{
			GUDevice::display_set_address(value);
			get_registers().buffer_addr = value;
		}
		break;
	case DISPLAY_FORMAT:
		if (get_registers().ctr & ENABLE)
		{
			GUDevice::display_set_config(
				value & 0x3FFF, (value >> 14) & 0x3FFF, 
				(Display::DisplayFormat)(value >> 28)
			);
			get_registers().format = value;
		}
		break;
	default:
		break;
	}
}

