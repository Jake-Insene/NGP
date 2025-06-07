/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"


namespace JIT::X86
{

enum X86Register
{
	EAX = 0x0,
	EBX = 0x1,
	ECX = 0x2,
	EDX = 0x3,
	ESI = 0x4,
	EDI = 0x5,
	ESP = 0x6,
	EBP = 0x7,
	R8D = 0x9,
	R9D = 0x9,
	R10D = 0xA,
	R11D = 0xB,
	R12D = 0xC,
	R13D = 0xD,
	R14D = 0xE,
	R15D = 0xF,
};

u8 modrm(u8 mod, X86Register reg, X86Register rm)
{
	return mod | (reg << 2) | (rm << 5);
}

u8* mov_eax_imm32(u8* mem, u32 imm32)
{
	mem[0] = 0xB8; // mov eax, imm32
	mem[1] = imm32 & 0xFF;
	mem[2] = (imm32 >> 8) & 0xFF;
	mem[3] = (imm32 >> 16) & 0xFF;
	mem[4] = (imm32 >> 24) & 0xFF;

	return mem + 5;
}

u8* mov_rcx_mem32_imm32(u8* mem, u8 offset, u32 imm32)
{
	mem[0] = 0xC7; // mov dword ptr[rcx + offset], imm32
	mem[1] = 0x41;
	mem[2] = offset;
	mem[3] = imm32 & 0xFF;
	mem[4] = (imm32 >> 8) & 0xFF;
	mem[5] = (imm32 >> 16) & 0xFF;
	mem[6] = (imm32 >> 24) & 0xFF;

	return mem + 7;
}

u8* mov_reg_rcx_mem32(u8* mem, X86Register reg, u8 offset)
{
	mem[0] = 0x8B; // mov reg, dword ptr[rcx + offset]
	mem[1] = modrm(0x01, reg, ECX);
	mem[2] = offset;

	return mem + 3;
}

u8* sub_rcx_mem32_imm32(u8* mem, u8 offset, u32 imm32)
{
	mem[0] = 0x81; // sub dword ptr[rcx + offset], imm32
	mem[1] = 0x69;
	mem[2] = offset;
	mem[3] = imm32 & 0xFF;
	mem[4] = (imm32 >> 8) & 0xFF;
	mem[5] = (imm32 >> 16) & 0xFF;
	mem[6] = (imm32 >> 24) & 0xFF;

	return mem + 7;
}

u8* shl(u8* mem, X86Register reg, u8 bit_count)
{
	mem[0] = 0xC1; // shl reg, bit_count
	mem[1] = modrm(0, reg, EAX);
	mem[2] = bit_count;

	return mem + 3;
}

u8* ret(u8* mem)
{
	mem[0] = 0xC3; // ret
	return mem + 1;
}

}