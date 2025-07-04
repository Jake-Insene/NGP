/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/JIT/X86/X86JIT.h"

#include "CPU/JIT/X86/X86JITHelper.h"
#include "CPU/CPUCore.h"

#include "FileFormat/ISA.h"
#include "Memory/Bus.h"
#include "Platform/OS.h"

namespace JIT
{

// In a win32 call:
//      rcx is the first argument
//      rdx is the second argument
//      r8 is the third argument

void X86JIT::initialize()
{

}

void X86JIT::shutdown()
{

}

void X86JIT::jit_block(CodeBlock& block, VirtualAddress pc)
{
    u8* memory = block.memory_executable;
    while (true)
    {
        Word inst = Bus::read_word(pc);

        u8 opc = inst & 0x3F;
        switch (opc)
        {
        case NGP_BL:
        {
            // Decoding disp
            const u32 disp_inst = inst >> 6;
            const u32 disp = inst & 0x8000000 ? (0xFC00'0000 | disp_inst) << 2 : disp_inst << 2;
            // now PC is the current instruction, but in execution it should point the next instruction
            Word new_pc = (pc + 4) + disp;

            memory = X86::mov_rcx_mem32_imm32(memory, CPUCore::LinkRegister * 4, pc + 4);
            memory = X86::mov_eax_imm32(memory, new_pc);
            memory = X86::ret(memory);
            goto end;
        }
            break;
        case NGP_B:
        {
            // Decoding disp
            const u32 disp_inst = inst >> 6;
            const u32 disp = inst & 0x8000000 ? (0xFC00'0000 | disp_inst) << 2 : disp_inst << 2;
            // now PC is the current instruction, but in execution it should point the next instruction
            Word new_pc = (pc + 4) + disp;

            memory = X86::mov_eax_imm32(memory, new_pc);
            memory = X86::ret(memory);
            goto end;
        }
            break;
        case NGP_ADR_PC:
        {
            u8 dest = (inst >> 6) & 0x1F;
            if (dest == CPUCore::ZeroRegister) break;
            const u32 disp_inst = inst >> 11;
            const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
            memory = X86::mov_rcx_mem32_imm32(memory, dest * 4, (pc + 4) + disp);
        }
            break;
        case NGP_3OP:
        {
            u8 las = (inst >> 6) & 0x3F;
            u8 dest = (inst >> 12) & 0x1F;
            u8 src1 = (inst >> 17) & 0x1F;
            u8 src2 = (inst >> 22) & 0x1F;
            u8 src3 = (inst >> 27) & 0x1F;

            switch (las)
            {
            case NGP_SUB:
                memory = X86::mov_reg_rcx_mem32(memory, X86::EAX, src1 * 4);
                memory = X86::mov_reg_rcx_mem32(memory, X86::EBX, src2 * 4);
                break;
            }
        }
            break;
        case NGP_NON_BINARY:
        {
      
        }
            break;
        case NGP_SUB_IMMEDIATE:
        {
            u8 dest = (inst >> 6) & 0x1F;
            u8 src1 = (inst >> 11) & 0x1F;
            u16 imm = inst >> 16;

            if (dest == ZeroRegister) break;

            if(dest == src1)
                memory = X86::sub_rcx_mem32_imm32(memory, dest * 4, imm);
        }
            break;
        }

        pc += 4;
    }

end:

	block.func = (X86JIT::JITFunc)block.memory_executable;
}

usize X86JIT::try_run_block(VirtualAddress pc, Word* wregisters, Word* psr)
{
    auto it = code_cache.find(pc);
    if (it != code_cache.end())
    {
        CodeBlock& block = it->second;
        if(block.func)
        {
            return it->second.func(wregisters, psr, pc);
        }
        else
        {
            jit_block(block, pc);
            return pc;
        }
    }

    auto& new_block = code_cache.insert({ pc, CodeBlock{} }).first->second;
    new_block.memory_executable = (u8*)OS::allocate_virtual_memory(nullptr, 4096, OS::PAGE_READ_WRITE_EXECUTE);
    new_block.memory_size = 4096;

    jit_block(new_block, pc);

    return new_block.func(wregisters, psr, pc);
}

}