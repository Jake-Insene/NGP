/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/CPU.h"
#include "FileFormat/ISA.h"
#include "Memory/Bus.h"
#include <stdio.h>

// Logical Add sub
FORCE_INLINE void setFlags(CPU::CPUCore* core, u32 src1, u32 src2, u64 res) {
    // Set Z flag (zero flag)
    core->psr.z = res == 0;

    // Set C flag (carry flag)
    core->psr.c = src1 >= src2;

    // Set N flag (negative flag)
    core->psr.n = (res & 0x8000'0000) != 0;

    // Set V flag (overflow flag)
    core->psr.v = ((~((u64)src1 ^ (u64)src2) & ((u64)src1 ^ res)) & 0x8000'0000) != 0;
}

// Comparision
FORCE_INLINE u32 add_with_carry_setting_flags(CPU::CPUCore* core, u32 src1, u32 src2, u32 carry) {
    u64 res = u64(src1) + u64(src2) + carry;
    setFlags(core, src1, src2, res);

    return u32(res);
}

// Comparision
FORCE_INLINE u32 add_with_carry(u32 src1, u32 src2, u32 carry) {
    u64 res = u64(src1) + u64(src2) + carry;
    return u32(res);
}

FORCE_INLINE u32 and_setting_flags(CPU::CPUCore* core, u32 src1, u32 src2) {
    u64 res = (u64)src1 & (u64)src2;
    core->psr.n = (res & 0x8000'0000) != 0;
    core->psr.z = res == 0;

    return u32(res);
}

#define MAKE_SIMPLE_LAS(name, op) \
    void name(CPU::CPUCore* core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        core->list[dest] = dest == 31 ? op : 0;\
    }

#define MAKE_SIMPLE_OP(name, op) \
    void name(CPU::CPUCore* core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        core->list[dest] = dest == 31 ? op : 0;\
    }

#define MAKE_SETTING_FLAGS(name, op) \
    void name(CPU::CPUCore* core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        u32 result = op;\
        core->list[dest] = dest == 31 ? result : 0;\
    }

MAKE_SIMPLE_LAS(add_shl, core->list[src1] + (core->list[src2] << src3))
MAKE_SIMPLE_LAS(add_shr, core->list[src1] + (core->list[src2] >> src3))
MAKE_SIMPLE_LAS(add_asr, core->list[src1] + (core->ilist[src2] >> src3))
MAKE_SIMPLE_LAS(adc, add_with_carry(core->list[src1], core->list[src2], core->psr.c));

MAKE_SIMPLE_LAS(sub_shl, core->list[src1] - (core->list[src2] << src3))
MAKE_SIMPLE_LAS(sub_shr, core->list[src1] - (core->list[src2] >> src3))
MAKE_SIMPLE_LAS(sub_asr, core->list[src1] - (core->ilist[src2] >> src3))
MAKE_SIMPLE_LAS(sbc, add_with_carry(core->list[src1], ~core->list[src2], core->psr.c));

MAKE_SIMPLE_LAS(and_shl, core->list[src1] & (core->list[src2] << src3))
MAKE_SIMPLE_LAS(and_shr, core->list[src1] & (core->list[src2] >> src3))
MAKE_SIMPLE_LAS(and_asr, core->list[src1] & (core->ilist[src2] >> src3))
// TODO: implement ROR
MAKE_SIMPLE_LAS(and_ror, core->list[src1] & (core->ilist[src2]))

MAKE_SIMPLE_LAS(or_shl, core->list[src1] | (core->list[src2] << src3))
MAKE_SIMPLE_LAS(or_shr, core->list[src1] | (core->list[src2] >> src3))
MAKE_SIMPLE_LAS(or_asr, core->list[src1] | (core->ilist[src2] >> src3))
// TODO: implement ROR
MAKE_SIMPLE_LAS(or_ror, core->list[src1] | (core->ilist[src2]))

MAKE_SIMPLE_LAS(orn_shl, core->list[src1] | !(core->list[src2] << src3))
MAKE_SIMPLE_LAS(orn_shr, core->list[src1] | !(core->list[src2] >> src3))
MAKE_SIMPLE_LAS(orn_asr, core->list[src1] | !(core->ilist[src2] >> src3))
// TODO: implement ROR
MAKE_SIMPLE_LAS(orn_ror, core->list[src1] | !(core->ilist[src2]))

MAKE_SIMPLE_LAS(eor_shl, core->list[src1] ^ (core->list[src2] << src3))
MAKE_SIMPLE_LAS(eor_shr, core->list[src1] ^ (core->list[src2] >> src3))
MAKE_SIMPLE_LAS(eor_asr, core->list[src1] ^ (core->ilist[src2] >> src3))
// TODO: implement ROR
MAKE_SIMPLE_LAS(eor_ror, core->list[src1] ^ (core->ilist[src2]))

MAKE_SETTING_FLAGS(adds_shl, add_with_carry_setting_flags(core, core->list[src1], core->list[src2] << src3, 0));
MAKE_SETTING_FLAGS(adds_shr, add_with_carry_setting_flags(core, core->list[src1], core->list[src2] >> src3, 0));
MAKE_SETTING_FLAGS(adds_asr, add_with_carry_setting_flags(core, core->list[src1], core->ilist[src2] >> src3, 0));
// TODO: implement ROR
MAKE_SETTING_FLAGS(adds_ror, add_with_carry_setting_flags(core, core->list[src1], core->ilist[src2], 0));

MAKE_SETTING_FLAGS(subs_shl, add_with_carry_setting_flags(core, core->list[src1], ~(core->list[src2] << src3), 1));
MAKE_SETTING_FLAGS(subs_shr, add_with_carry_setting_flags(core, core->list[src1], ~(core->list[src2] >> src3), 1));
MAKE_SETTING_FLAGS(subs_asr, add_with_carry_setting_flags(core, core->list[src1], ~(core->ilist[src2] >> src3), 1));
// TODO: implement ROR
MAKE_SETTING_FLAGS(subs_ror, add_with_carry_setting_flags(core, core->list[src1], core->ilist[src2], 1));

MAKE_SETTING_FLAGS(ands_shl, and_setting_flags(core, core->list[src1], core->list[src2] << src3));
MAKE_SETTING_FLAGS(ands_shr, and_setting_flags(core, core->list[src1], core->list[src2] >> src3));
MAKE_SETTING_FLAGS(ands_asr, and_setting_flags(core, core->list[src1], core->ilist[src2] >> src3));
// TODO: implement ROR
MAKE_SETTING_FLAGS(ands_ror, and_setting_flags(core, core->list[src1], core->list[src2]));

MAKE_SIMPLE_LAS(bic_shl, core->list[src1] & !(core->list[src2] << src3));
MAKE_SIMPLE_LAS(bic_shr, core->list[src1] & !(core->list[src2] >> src3));
MAKE_SIMPLE_LAS(bic_asr, core->list[src1] & !(core->ilist[src2] >> src3));
// TODO: implement ROR
MAKE_SIMPLE_LAS(bic_ror, core->list[src1] & !(core->ilist[src2]));

MAKE_SETTING_FLAGS(bics_shl, and_setting_flags(core, core->list[src1], !(core->list[src2] << src3)));
MAKE_SETTING_FLAGS(bics_shr, and_setting_flags(core, core->list[src1], !(core->list[src2] >> src3)));
MAKE_SETTING_FLAGS(bics_asr, and_setting_flags(core, core->list[src1], !(core->ilist[src2] >> src3)));
// TODO: implement ROR
MAKE_SETTING_FLAGS(bics_ror, and_setting_flags(core, core->list[src1], core->list[src2]));

MAKE_SIMPLE_LAS(adcs, add_with_carry_setting_flags(core, core->list[src1], core->list[src2], core->psr.c));
MAKE_SIMPLE_LAS(sbcs, add_with_carry_setting_flags(core, core->list[src1], ~core->list[src2], core->psr.c));

// Memory
FORCE_INLINE void memory_read_byte(CPU::CPUCore* core, u32 dest, u32 address) {
    if (dest != 31) {
        // write back, fetch
        core->cycle_counter += 2;
        core->list[dest] = Bus::read_byte(core, address);
    }
}

FORCE_INLINE void memory_read_half(CPU::CPUCore* core, u32 dest, u32 address) {
    if (dest != 31) {
        // write back, fetch
        core->cycle_counter += 2;
        core->list[dest] = Bus::read_half(core, address);
    }
}

FORCE_INLINE void memory_read_word(CPU::CPUCore* core, u32 dest, u32 address) {
    if (dest != 31) {
        // write back, fetch
        core->cycle_counter += 2;
        core->list[dest] = Bus::read_word(core, address);
    }
}

FORCE_INLINE void memory_read_ihalf(CPU::CPUCore* core, u32 dest, u32 address) {
    if (dest != 31) {
        // write back, fetch
        core->cycle_counter += 2;
        core->ilist[dest] = static_cast<i16>(Bus::read_half(core, address));
    }
}

FORCE_INLINE void memory_read_ibyte(CPU::CPUCore* core, u32 dest, u32 address) {
    if (dest != 31) {
        // write back, fetch
        core->cycle_counter += 2;
        core->ilist[dest] = static_cast<i8>(Bus::read_byte(core, address));
    }
}

FORCE_INLINE void memory_read_single(CPU::CPUCore* core, u32 dest, u32 address) {
    if (dest != 31) {
        // write back, fetch
        core->cycle_counter += 2;
        core->simd[dest].w = Bus::read_word(core, address);
    }
}

FORCE_INLINE void memory_read_double(CPU::CPUCore* core, u32 dest, u32 address) {
    if (dest != 31) {
        // write back, fetch
        core->cycle_counter += 2;
        core->simd[dest].dw = Bus::read_dword(core, address);
    }
}

FORCE_INLINE void memory_read_qword(CPU::CPUCore* core, u32 dest, u32 address) {
    if (dest != 31) {
        // write back, fetch
        core->cycle_counter += 2;
        core->simd[dest].qw = Bus::read_qword(core, address);
    }
}

FORCE_INLINE void memory_write_byte(CPU::CPUCore* core, u32 src, u32 address) {
    // fetch
    core->cycle_counter++;
    Bus::write_byte(core, address, static_cast<u8>(core->list[src]));
}

FORCE_INLINE void memory_write_half(CPU::CPUCore* core, u32 src, u32 address) {
    // fetch
    core->cycle_counter++;
    Bus::write_half(core, address, static_cast<u16>(core->list[src]));
}

FORCE_INLINE void memory_write_word(CPU::CPUCore* core, u32 src, u32 address) {
    // fetch
    core->cycle_counter++;
    Bus::write_word(core, address, core->list[src]);
}

FORCE_INLINE void memory_write_single(CPU::CPUCore* core, u32 src, u32 address) {
    // fetch
    core->cycle_counter++;
    Bus::write_word(core, address, core->simd[src].w);
}

FORCE_INLINE void memory_write_double(CPU::CPUCore* core, u32 src, u32 address) {
    // fetch
    core->cycle_counter++;
    Bus::write_dword(core, address, core->simd[src].dw);
}

FORCE_INLINE void memory_write_qword(CPU::CPUCore* core, u32 src, u32 address) {
    // fetch
    core->cycle_counter++;
    Bus::write_qword(core, address, core->simd[src].qw);
}

FORCE_INLINE void memory_pair_read_word(CPU::CPUCore* core, u32 dest1, u32 dest2, u32 address) {
    // write back, fetch
    if (dest1 != 31) {
        core->cycle_counter++;
        core->list[dest1] = Bus::read_word(core, address);
    }

    if (dest2 != 31) {
        core->cycle_counter++;
        core->list[dest2] = Bus::read_word(core, address + 4);
    }
}

FORCE_INLINE void memory_pair_read_single(CPU::CPUCore* core, u32 dest1, u32 dest2, u32 address) {
    // write back, fetch
    if (dest1 != 31) {
        core->cycle_counter++;
        core->simd[dest1].w = Bus::read_word(core, address);
    }

    if (dest2 != 31) {
        core->cycle_counter++;
        core->simd[dest2].w = Bus::read_word(core, address + 4);
    }
}

FORCE_INLINE void memory_pair_read_double(CPU::CPUCore* core, u32 dest1, u32 dest2, u32 address) {
    // write back, fetch
    if (dest1 != 31) {
        core->cycle_counter++;
        core->simd[dest1].dw = Bus::read_dword(core, address);
    }

    if (dest2 != 31) {
        core->cycle_counter++;
        core->simd[dest2].dw = Bus::read_dword(core, address + 8);
    }
}

FORCE_INLINE void memory_pair_read_qword(CPU::CPUCore* core, u32 dest1, u32 dest2, u32 address) {
    // write back, fetch
    if (dest1 != 31) {
        core->cycle_counter++;
        core->simd[dest1].qw = Bus::read_qword(core, address);
    }

    if (dest2 != 31) {
        core->cycle_counter++;
        core->simd[dest2].qw = Bus::read_qword(core, address + 16);
    }
}

FORCE_INLINE void memory_pair_write_word(CPU::CPUCore* core, u32 src1, u32 src2, u32 address) {
    // fetch
    core->cycle_counter += 2;
    Bus::write_word(core, address, core->list[src1]);
    Bus::write_word(core, address + 4, core->list[src2]);
}

FORCE_INLINE void memory_pair_write_single(CPU::CPUCore* core, u32 src1, u32 src2, u32 address) {
    // fetch
    core->cycle_counter += 2;
    Bus::write_word(core, address, core->simd[src1].w);
    Bus::write_word(core, address + 4, core->simd[src2].w);
}

FORCE_INLINE void memory_pair_write_double(CPU::CPUCore* core, u32 src1, u32 src2, u32 address) {
    // fetch
    core->cycle_counter += 2;
    Bus::write_dword(core, address, core->simd[src1].dw);
    Bus::write_dword(core, address + 8, core->simd[src2].dw);
}

FORCE_INLINE void memory_pair_write_qword(CPU::CPUCore* core, u32 src1, u32 src2, u32 address) {
    // fetch
    core->cycle_counter += 2;
    Bus::write_qword(core, address, core->simd[src1].qw);
    Bus::write_qword(core, address + 16, core->simd[src2].qw);
}

// Non Binary
void ret(CPU::CPUCore* core, u8, u8 src1, u8) {
    core->pc = core->list[src1];

}

void blr(CPU::CPUCore* core, u8, u8 src1, u8) {
    core->gpr.lr = core->pc;
    core->pc = core->list[src1];
}

void br(CPU::CPUCore* core, u8, u8 src1, u8) {
    core->pc = core->list[src1];
}

void eret(CPU::CPUCore* core, u8, u8, u8) {}
void brk(CPU::CPUCore* core, u8, u8, u8) {}
void hlt(CPU::CPUCore* core, u8, u8, u8) {
    core->psr.halt = true;
}
void sit(CPU::CPUCore* core, u8, u8, u8) {}
void msr(CPU::CPUCore* core, u8, u8, u8) {}
void mrs(CPU::CPUCore* core, u8, u8, u8) {}
void nop(CPU::CPUCore* core, u8, u8, u8) {}

// Main opcodes
FORCE_INLINE void bl(CPU::CPUCore* core, u32 inst) {
    i32 disp = inst >> 6;
    disp = disp & 0x2000000 ? 0xFC00'0000 | disp : disp;
    disp <<= 2;
    core->gpr.lr = core->pc;
    core->pc += disp;
}

FORCE_INLINE void b(CPU::CPUCore* core, u32 inst) {
    i32 disp = inst >> 6;
    disp = disp & 0x2000000 ? 0xFC00'0000 | disp : disp;
    disp <<= 2;
    core->pc += disp;
}

FORCE_INLINE void bcond(CPU::CPUCore* core, u32 inst) {
    u8 cond = (inst >> 6) & 0xF;
    i32 disp = (inst >> 10);
    disp = disp & 0x200000 ? 0xFFC0'0000 | disp : disp;
    disp <<= 2;

    switch (cond) {
    case NGP_BEQ:
        if (core->psr.z) {
            core->pc += disp;
        }
        break;
    case NGP_BNE:
        if (!core->psr.z) {
            core->pc += disp;
        }
        break;
    case NGP_BLT:
        if (core->psr.n ^ core->psr.v) {
            core->pc += disp;
        }
        break;
    case NGP_BLE:
        if (core->psr.z || core->psr.n ^ core->psr.v) {
            core->pc += disp;
        }
        break;
    case NGP_BGT:
        if (!core->psr.z && core->psr.n == core->psr.v) {
            core->pc += disp;
        }
        break;
    case NGP_BGE:
        if (core->psr.n == core->psr.v) {
            core->pc += disp;
        }
        break;
    case NGP_BCS:
        if (core->psr.c) {
            core->pc += disp;
        }
        break;
    case NGP_BCC:
        if (!core->psr.c) {
            core->pc += disp;
        }
        break;
    case NGP_BMI:
        if (core->psr.n) {
            core->pc += disp;
        }
        break;
    case NGP_BPL:
        if (!core->psr.n) {
            core->pc += disp;
        }
        break;
    case NGP_BVS:
        if (core->psr.v) {
            core->pc += disp;
        }
        break;
    case NGP_BVC:
        if (!core->psr.v) {
            core->pc += disp;
        }
        break;
    case NGP_BHI:
        break;
    case NGP_BLS:
        break;
    case NGP_BAL:
        core->pc += disp;
        break;
    default:
        // TODO: invalid instruction exception
        break;
    }
}

FORCE_INLINE void logical_add_sub(CPU::CPUCore* core, u32 inst) {
    u8 las = (inst >> 6) & 0x3F;
    u8 dest = (inst >> 12) & 0x1F;
    u8 src1 = (inst >> 17) & 0x1F;
    u8 src2 = (inst >> 22) & 0x1F;
    u8 src3 = (inst >> 27) & 0x1F;

#define CASE(op, name) case op: name(core, dest, src1, src2, src3); break
    switch (las) {
    CASE(NGP_ADD_SHL, add_shl);
    CASE(NGP_ADD_SHR, add_shr);
    CASE(NGP_ADD_ASR, add_asr);
    CASE(NGP_ADC, adc);
    CASE(NGP_SUB_SHL, sub_shl);
    CASE(NGP_SUB_SHR, sub_shr);
    CASE(NGP_SUB_ASR, sub_asr);
    CASE(NGP_SBC, sbc);
    CASE(NGP_AND_SHL, and_shl);
    CASE(NGP_AND_SHR, and_shr);
    CASE(NGP_AND_ASR, and_asr);
    CASE(NGP_AND_ROR, and_ror);
    CASE(NGP_OR_SHL, or_shl);
    CASE(NGP_OR_SHR, or_shr);
    CASE(NGP_OR_ASR, or_asr);
    CASE(NGP_OR_ROR, or_ror);
    CASE(NGP_ORN_SHL, orn_shl);
    CASE(NGP_ORN_SHR, orn_shr);
    CASE(NGP_ORN_ASR, orn_asr);
    CASE(NGP_ORN_ROR, orn_ror);
    CASE(NGP_EOR_SHL, eor_shl);
    CASE(NGP_EOR_SHR, eor_shr);
    CASE(NGP_EOR_ASR, eor_asr);
    CASE(NGP_EOR_ROR, eor_ror);
    CASE(NGP_ADDS_SHL, adds_shl);
    CASE(NGP_ADDS_SHR, adds_shr);
    CASE(NGP_ADDS_ASR, adds_asr);
    CASE(NGP_ADDS_ROR, adds_ror);
    CASE(NGP_SUBS_SHL, subs_shl);
    CASE(NGP_SUBS_SHR, subs_shr);
    CASE(NGP_SUBS_ASR, subs_asr);
    CASE(NGP_SUBS_ROR, subs_ror);
    CASE(NGP_ANDS_SHL, ands_shl);
    CASE(NGP_ANDS_SHR, ands_shr);
    CASE(NGP_ANDS_ASR, ands_asr);
    CASE(NGP_ANDS_ROR, ands_ror);
    CASE(NGP_BIC_SHL, bic_shl);
    CASE(NGP_BIC_SHR, bic_shr);
    CASE(NGP_BIC_ASR, bic_asr);
    CASE(NGP_BIC_ROR, bic_ror);
    CASE(NGP_BICS_SHL, bics_shl);
    CASE(NGP_BICS_SHR, bics_shr);
    CASE(NGP_BICS_ASR, bics_asr);
    CASE(NGP_BICS_ROR, bics_ror);
    CASE(NGP_ADCS, adcs);
    CASE(NGP_SBCS, sbcs);
    default:
        // TODO: invalid instruction
        break;
    }
#undef CASE
}

FORCE_INLINE void fp_op(CPU::CPUCore* core, u32 inst) {}

FORCE_INLINE void load_store_immediate(CPU::CPUCore* core, u32 inst) {
    u8 memopc = (inst>>6) & 0x7;

    u8 dest = (inst >> 9) & 0x1F;
    u8 base = (inst >> 14) & 0x1F;
    i16 imm = (inst >> 20);
    imm = inst & 0x7FF ? -imm : imm;

#define CASE(op, name) case op: name(core, dest, core->list[base] + imm); break
    switch (memopc) {
        CASE(NGP_LD_IMMEDIATE, memory_read_word);
        CASE(NGP_LDSH_IMMEDIATE, memory_read_ihalf);
        CASE(NGP_LDH_IMMEDIATE, memory_read_half);
        CASE(NGP_LDSB_IMMEDIATE, memory_read_ibyte);
        CASE(NGP_LDB_IMMEDIATE, memory_read_byte);
        CASE(NGP_ST_IMMEDIATE, memory_write_word);
        CASE(NGP_STH_IMMEDIATE, memory_write_half);
        CASE(NGP_STB_IMMEDIATE, memory_write_byte);
    default:
        break;
    }
#undef CASE
}

FORCE_INLINE void load_store_fp_immediate(CPU::CPUCore* core, u32 inst) {}

FORCE_INLINE void load_store_register(CPU::CPUCore* core, u32 inst) {}

FORCE_INLINE void load_store_pair(CPU::CPUCore* core, u32 inst) {}

FORCE_INLINE void additional(CPU::CPUCore* core, u32 inst) {}

FORCE_INLINE void non_binary(CPU::CPUCore* core, u32 inst) {
    u16 nonopc = (inst >> 6) & 0x3FF;
    u8 op = (inst >> 16) & 0x3F;
    u8 src1 = (inst >> 22) & 0x1F;
    u8 src2 = (inst >> 27) & 0x1F;

#define CASE(_op, name) case _op: name(core, op, src1, src2); break
    switch (nonopc) {
    CASE(NGP_RET, ret);
    CASE(NGP_BLR, blr);
    CASE(NGP_BR, br);
    CASE(NGP_ERET, eret);
    CASE(NGP_BRK, brk);
    CASE(NGP_HLT, hlt);
    CASE(NGP_SIT, sit);
    CASE(NGP_MSR, msr);
    CASE(NGP_MRS, mrs);
    CASE(NGP_NOP, nop);
    default:
        break;
    }
#undef CASE
}

FORCE_INLINE void ld_pc(CPU::CPUCore* core, u32 inst) {}
FORCE_INLINE void ld_s_pc(CPU::CPUCore* core, u32 inst) {}
FORCE_INLINE void ld_d_pc(CPU::CPUCore* core, u32 inst) {}
FORCE_INLINE void ld_q_pc(CPU::CPUCore* core, u32 inst) {}
FORCE_INLINE void adr_pc(CPU::CPUCore* core, u32 inst) {}

FORCE_INLINE void immediate(CPU::CPUCore* core, u32 inst) {
    u16 imm = inst >> 16;
    u8 immopc = (inst >> 6) & 0x1F;
    u8 reg = (inst >> 11) & 0x1F;

    if (immopc == NGP_MOVT_IMMEDIATE) {
        core->list[reg] |= (imm << 16);
    }
}

FORCE_INLINE void add_immediate(CPU::CPUCore* core, u32 inst) {
    u8 dest = (inst >> 6) & 0x1F;
    if (dest != 31) {
        u8 src = (inst >> 11) & 0x1F;
        u16 imm = inst >> 16;
        core->list[dest] = core->list[src] + imm;
    }
}

FORCE_INLINE void adds_immediate(CPU::CPUCore* core, u32 inst) {
    u8 dest = (inst >> 6) & 0x1F;
    u8 src = (inst >> 11) & 0x1F;
    u16 imm = inst >> 16;
    u32 result = add_with_carry_setting_flags(core, core->list[src], imm, 0);
    core->list[dest] = dest == 31 ? result : 0;
}

FORCE_INLINE void sub_immediate(CPU::CPUCore* core, u32 inst) {
    u8 dest = (inst >> 6) & 0x1F;
    if (dest != 31) {
        u8 src = (inst >> 11) & 0x1F;
        u16 imm = inst >> 16;
        core->list[dest] = core->list[src] - imm;
    }
}

FORCE_INLINE void subs_immediate(CPU::CPUCore* core, u32 inst) {
    u8 dest = (inst >> 6) & 0x1F;
    u8 src = (inst >> 11) & 0x1F;
    u16 imm = inst >> 16;
    u32 result = add_with_carry_setting_flags(core, core->list[src], !imm, 1);
    core->list[dest] = dest == 31 ? result : 0;
}

FORCE_INLINE void and_immediate(CPU::CPUCore* core, u32 inst) {
    u8 dest = (inst >> 6) & 0x1F;
    if (dest != 31) {
        u8 src = (inst >> 11) & 0x1F;
        u16 imm = inst >> 16;
        core->list[dest] = core->list[src] & imm;
    }
}

FORCE_INLINE void ands_immediate(CPU::CPUCore* core, u32 inst) {
    u8 dest = (inst >> 6) & 0x1F;
    u8 src = (inst >> 11) & 0x1F;
    u16 imm = inst >> 16;
    u32 result = and_setting_flags(core, core->list[src], imm);
    core->list[dest] = dest == 31 ? result : 0;
}

FORCE_INLINE void or_immediate(CPU::CPUCore* core, u32 inst) {
    u8 dest = (inst >> 6) & 0x1F;
    if (dest != 31) {
        u8 src = (inst >> 11) & 0x1F;
        u16 imm = inst >> 16;
        core->list[dest] = core->list[src] | imm;
    }
}

FORCE_INLINE void eor_immediate(CPU::CPUCore* core, u32 inst) {
    u8 dest = (inst >> 6) & 0x1F;
    if (dest != 31) {
        u8 src = (inst >> 11) & 0x1F;
        u16 imm = inst >> 16;
        core->list[dest] = core->list[src] ^ imm;
    }
}

FORCE_INLINE void tbz(CPU::CPUCore* core, u32 inst) {}
FORCE_INLINE void tbnz(CPU::CPUCore* core, u32 inst) {}
FORCE_INLINE void cbz(CPU::CPUCore* core, u32 inst) {}
FORCE_INLINE void cbnz(CPU::CPUCore* core, u32 inst) {}

void CPU::initialize() {}

void CPU::shutdown() {}

#define PROFILE 1

void CPU::dispatch(CPU::CPUCore& core, u32& counter) {
    while (core.cycle_counter < CYCLES_PER_FRAME) {
        if (core.psr.halt) {
            break;
        }

        Bus::read_pc(&core);

        core.pc += 4;

        // One cycle for fetching, decoding and simple arithmetic
        core.cycle_counter++;

        u8 opcode = core.ir & 0x3F;

#define CASE(op, name) case op: name(&core, core.ir); break
        switch (opcode) {
        CASE(NGP_BL, bl);
        CASE(NGP_B, b);
        CASE(NGP_B_COND, bcond);
        CASE(NGP_LOGICAL_ADD_SUB, logical_add_sub);
        CASE(NGP_FP_OP, fp_op);
        CASE(NGP_LOAD_STORE_IMMEDIATE, load_store_immediate);
        CASE(NGP_LOAD_STORE_FP_IMMEDIATE, load_store_fp_immediate);
        CASE(NGP_LOAD_STORE_REGISTER, load_store_register);
        CASE(NGP_LOAD_STORE_PAIR, load_store_pair);
        CASE(NGP_ADDITIONAL, additional);
        CASE(NGP_NON_BINARY, non_binary);
        CASE(NGP_LD_PC, ld_pc);
        CASE(NGP_LD_S_PC, ld_s_pc);
        CASE(NGP_LD_D_PC, ld_d_pc);
        CASE(NGP_LD_Q_PC, ld_q_pc);
        CASE(NGP_ADR_PC, adr_pc);
        CASE(NGP_IMMEDIATE, immediate);
        CASE(NGP_ADD_IMMEDIATE, add_immediate);
        CASE(NGP_ADDS_IMMEDIATE, adds_immediate);
        CASE(NGP_SUB_IMMEDIATE, sub_immediate);
        CASE(NGP_SUBS_IMMEDIATE, subs_immediate);
        CASE(NGP_AND_IMMEDIATE, and_immediate);
        CASE(NGP_ANDS_IMMEDIATE, ands_immediate);
        CASE(NGP_OR_IMMEDIATE, or_immediate);
        CASE(NGP_EOR_IMMEDIATE, eor_immediate);
        CASE(NGP_TBZ, tbz);
        CASE(NGP_TBNZ, tbnz);
        CASE(NGP_CBZ, cbz);
        CASE(NGP_CBNZ, cbnz);
        default:
            break;
        }
#undef CASE

#if PROFILE == 1
        counter++;
#endif
    }

}

void CPU::print_pegisters(CPU::CPUCore& core) {
    printf(
        "R0  = %08X R1  = %08X R2  = %08X R3  = %08X R4  = %08X\n"
        "R5  = %08X R6  = %08X R7  = %08X R8  = %08X R9  = %08X\n"
        "R10 = %08X R11 = %08X R12 = %08X R13 = %08X R14 = %08X\n"
        "R15 = %08X R16 = %08X R17 = %08X R18 = %08X R19 = %08X\n"
        "R20 = %08X R21 = %08X R22 = %08X R23 = %08X R24 = %08X\n"
        "R25 = %08X R26 = %08X R27 = %08X R28 = %08X\n"
        "SP  = %08X LR  = %08X PC  = %08X\n",
        core.gpr.r0, core.gpr.r1, core.gpr.r2, core.gpr.r3, core.gpr.r4,
        core.gpr.r5, core.gpr.r6, core.gpr.r7, core.gpr.r8, core.gpr.r9,
        core.gpr.r10, core.gpr.r11, core.gpr.r12, core.gpr.r13, core.gpr.r14,
        core.gpr.r15, core.gpr.r16, core.gpr.r17, core.gpr.r18, core.gpr.r19,
        core.gpr.r20, core.gpr.r21, core.gpr.r22, core.gpr.r23, core.gpr.r24,
        core.gpr.r25, core.gpr.r26, core.gpr.r27, core.gpr.r28,
        core.gpr.sp, core.gpr.lr, core.pc
    );
}

