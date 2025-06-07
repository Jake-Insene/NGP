/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

#include <unordered_map>

struct CPUCore;

namespace JIT
{

struct X86JIT
{
	using JITFunc = usize(*)(Word*, Word*, Word);

	struct CodeBlock
	{
		JITFunc func;
		u8* memory_executable;
		usize memory_size;
	};

	// Function pointer to the JIT compiled code
	std::unordered_map<VirtualAddress, CodeBlock> code_cache;
	
	void initialize();
	void shutdown();

	void jit_block(CodeBlock& block, VirtualAddress pc);

	usize try_run_block(VirtualAddress pc, Word* wregisters, Word* psr);
};

}
