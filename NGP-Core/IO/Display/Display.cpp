/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "IO/Display/Display.h"

#include "Video/GU.h"


namespace IO
{

DisplayRegisters& get_display_registers()
{
	return *(DisplayRegisters*)(Bus::MAPPED_BUS_ADDRESS_START + DISPLAY_BASE);
}


void IO::display_handle_write_word(CPUCore& core, VirtualAddress address, Word value)
{
	if (address != DISPLAY_CTR && (get_display_registers().ctr & DISPLAY_ENABLE) != 1)
		return;

	switch (address)
	{
	case DISPLAY_CTR:
		get_display_registers().ctr = value;
		break;
	case DISPLAY_BUFFER:
		GU::display_set_address(value);
		get_display_registers().buffer = value;
		break;
	case DISPLAY_FORMAT:
		GU::display_set_config(value & 0x3FFF, (value >> 14) & 0x3FFF, (IO::DisplayFormat)(value >> 28));
		get_display_registers().format = value;
		break;
	case DISPLAY_PRESENT:
		GU::request_present();
		break;
	default:
		break;
	}
}

}