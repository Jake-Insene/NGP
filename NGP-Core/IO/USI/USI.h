/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "IO/IO.h"
#include "Memory/Bus.h"


struct USI
{
	enum Register
	{
		// USI Interrupt Mask
		// [0] Read/Write Finished
		// [1] Device Inserted
		// [2] Device Remove
		USI_IRQ_MASK =	0x000,
		USI_IRQ_STATUS = 0x004,
		// USI Control Register
		// [0] Reset
		USI_CTR =		0x00C,
		USI_ID =		0x008,

		// USI Command
		USI_CMD =			0x010,
		USI_ADDR =			0x014,
		USI_BLOCK_ADDR =	0x018,
		USI_PORT_SELECT =	0x01C,
	};

	enum USIRQMask
	{
		USI_IRQ_MASK_READ_WRITE_FINISHED = 0x1,
		USI_IRQ_MASK_DEVICE_INSERT = 0x2,
		USI_IRQ_MASK_DEVICE_REMOVE = 0x4,
	};

	enum USIControlBit
	{
		USI_RESET,
	};

	enum USID
	{
		USI_1 = 0,
	};

	struct USIRegisters
	{
		// 0x000
		Word irq_mask;
		Word irq_status;
		Word ctr;
		Word id;

		// 0x010
		Word cmd;
		Word addr;
		Word block_addr;
		Word port_select;
	};

	static IO::IODevice get_io_device();
	static USIRegisters& get_registers()
	{
		return *(USIRegisters*)(Bus::MAPPED_BUS_ADDRESS_START + IO::USI_BASE);
	}

	static void initialize();
	static void shutdown();

	static void dispatch();

	static void handle_write_word(VirtualAddress local_address, Word value);

};