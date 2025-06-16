/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/NGPV1/NGPV1.h"

#include "FileFormat/ISA.h"
#include "Memory/Bus.h"

#include <cstdio>
#include <bit>
#include <algorithm>

#undef OVERFLOW


#define MAKE_SIMPLE_ARITH_LOGIC(NAME, OP) \
    static FORCE_INLINE void NAME(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        core.list[dest] = (dest != CPUCore::ZeroRegister) * (OP);\
    }

#define MAKE_SIMPLE_OP(NAME, OP) \
    static FORCE_INLINE void NAME(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        core.list[dest] = (dest != CPUCore::ZeroRegister) * (OP);\
    }

#define MAKE_SETTING_FLAGS(NAME, OP) \
    static FORCE_INLINE void NAME(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        core.list[dest] = (dest != CPUCore::ZeroRegister) * (OP);\
    }

#define MAKE_IMMEDIATE_OP(NAME, OP) \
    static FORCE_INLINE void NAME(NGPV1& core, u32 inst)\
    {\
        u8 dest = (inst >> 6) & 0x1F;\
        u8 src = (inst >> 11) & 0x1F;\
        u16 imm = inst >> 16;\
        core.list[dest] = (dest != CPUCore::ZeroRegister) * OP;\
    }

#define HANDLE_BRANCH(COND, CORE, DISP) if(COND) { CORE.pc += DISP; CORE.handle_pc_change(); }

#define HANDLE_READ(CORE, DEST, FUNC, ADDRESS) \
    CORE.list[DEST] = (DEST != CPUCore::ZeroRegister) * FUNC(ADDRESS)

#define HANDLE_READ_SIGNED(CORE, DEST, FUNC, ADDRESS) \
    CORE.ilist[DEST] = (DEST != CPUCore::ZeroRegister) * FUNC(ADDRESS)

#define HANDLE_SIMD_READ(CORE, DEST, FIELD, FUNC, ADDRESS) \
    CORE.simd[DEST].FIELD = FUNC(ADDRESS)

#define HANDLE_WRITE(CORE, SRC, TYPE, FUNC, ADDRESS) \
    FUNC(ADDRESS, static_cast<TYPE>(CORE.list[SRC]));

#define HANDLE_SIMD_WRITE(CORE, SRC, FIELD, FUNC, ADDRESS) \
    FUNC(ADDRESS, CORE.simd[SRC].FIELD)

// Logical Add sub
static FORCE_INLINE void set_flags(NGPV1& core, u32 src1, u32 src2, u64 res, bool is_sub)
{
    // Set Z flag (zero flag)
    core.psr.ZERO = u32(res) == 0;

    // Set C flag (carry flag)
    if (is_sub)
    {
        // Carry for subtraction (borrow logic)
        core.psr.CARRY = src1 >= src2;
    }
    else
    {
        // Carry for addition
        core.psr.CARRY = ((u64)src1 + (u64)src2) > 0xFFFF'FFFF;
    }

    // Set N flag (negative flag)
    core.psr.NEGATIVE = bool(res & 0x8000'0000);

    // Set V flag (overflow flag)
    core.psr.OVERFLOW = ((~((u64)src1 ^ (u64)src2) & ((u64)src1 ^ res)) & 0x8000'0000) != 0;
}

// Comparison
static FORCE_INLINE u32 add_with_carry_setting_flags(NGPV1& core, u32 src1, u32 src2, u32 carry)
{
    const u64 lhs = src1;
    const u64 rhs = src2;
    const u64 res = lhs + rhs + carry;
    set_flags(core, src1, src2, res, false);

    return u32(res);
}

static u32 FORCE_INLINE and_setting_flags(NGPV1& core, u32 src1, u32 src2)
{
    u32 res = src1 & src2;
    core.psr.NEGATIVE = (res & 0x8000'0000) != 0;
    core.psr.ZERO = res == 0;

    return res;
}

static FORCE_INLINE u32 add_with_carry(u32 src1, u32 src2, u32 carry)
{
    u64 res = u64(src1) + u64(src2) + carry;
    return u32(res);
}

MAKE_SIMPLE_ARITH_LOGIC(add_shl, core.list[src1] + (core.list[src2] << src3));
MAKE_SIMPLE_ARITH_LOGIC(add_shr, core.list[src1] + (core.list[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(add_asr, core.list[src1] + (core.ilist[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(adc, add_with_carry(core.list[src1], core.list[src2], core.psr.CARRY));

MAKE_SIMPLE_ARITH_LOGIC(sub_shl, core.list[src1] - (core.list[src2] << src3));
MAKE_SIMPLE_ARITH_LOGIC(sub_shr, core.list[src1] - (core.list[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(sub_asr, core.list[src1] - (core.ilist[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(sbc, add_with_carry(core.list[src1], ~core.list[src2], core.psr.CARRY));

MAKE_SIMPLE_ARITH_LOGIC(and_shl, core.list[src1] & (core.list[src2] << src3));
MAKE_SIMPLE_ARITH_LOGIC(and_shr, core.list[src1] & (core.list[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(and_asr, core.list[src1] & (core.ilist[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(and_ror, core.list[src1] & (std::rotr(core.list[src2], src3)));

MAKE_SIMPLE_ARITH_LOGIC(or_shl, core.list[src1] | (core.list[src2] << src3));
MAKE_SIMPLE_ARITH_LOGIC(or_shr, core.list[src1] | (core.list[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(or_asr, core.list[src1] | (core.ilist[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(or_ror, core.list[src1] | (core.ilist[src2]));

MAKE_SIMPLE_ARITH_LOGIC(orn_shl, core.list[src1] | ~(core.list[src2] << src3));
MAKE_SIMPLE_ARITH_LOGIC(orn_shr, core.list[src1] | ~(core.list[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(orn_asr, core.list[src1] | ~(core.ilist[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(orn_ror, core.list[src1] | ~(std::rotr(core.list[src2], src3)));

MAKE_SIMPLE_ARITH_LOGIC(eor_shl, core.list[src1] ^ (core.list[src2] << src3));
MAKE_SIMPLE_ARITH_LOGIC(eor_shr, core.list[src1] ^ (core.list[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(eor_asr, core.list[src1] ^ (core.ilist[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(eor_ror, core.list[src1] ^ std::rotr(core.list[src2], src3));

MAKE_SETTING_FLAGS(adds_shl, add_with_carry_setting_flags(core, core.list[src1], core.list[src2] << src3, 0));
MAKE_SETTING_FLAGS(adds_shr, add_with_carry_setting_flags(core, core.list[src1], core.list[src2] >> src3, 0));
MAKE_SETTING_FLAGS(adds_asr, add_with_carry_setting_flags(core, core.list[src1], core.ilist[src2] >> src3, 0));
MAKE_SETTING_FLAGS(adds_ror, add_with_carry_setting_flags(core, core.list[src1], std::rotr(core.list[src2], src3), 0));


MAKE_SETTING_FLAGS(subs_shl, add_with_carry_setting_flags(core, core.list[src1], ~(core.list[src2] << src3), 1));
MAKE_SETTING_FLAGS(subs_shr, add_with_carry_setting_flags(core, core.list[src1], ~(core.list[src2] >> src3), 1));
MAKE_SETTING_FLAGS(subs_asr, add_with_carry_setting_flags(core, core.list[src1], ~(core.ilist[src2] >> src3), 1));
MAKE_SETTING_FLAGS(subs_ror, add_with_carry_setting_flags(core, core.list[src1], std::rotr(core.list[src2], src3), 1));

MAKE_SETTING_FLAGS(ands_shl, and_setting_flags(core, core.list[src1], core.list[src2] << src3));
MAKE_SETTING_FLAGS(ands_shr, and_setting_flags(core, core.list[src1], core.list[src2] >> src3));
MAKE_SETTING_FLAGS(ands_asr, and_setting_flags(core, core.list[src1], core.ilist[src2] >> src3));
MAKE_SETTING_FLAGS(ands_ror, and_setting_flags(core, core.list[src1], std::rotr(core.list[src2], src3)));

MAKE_SIMPLE_ARITH_LOGIC(bic_shl, core.list[src1] & ~(core.list[src2] << src3));
MAKE_SIMPLE_ARITH_LOGIC(bic_shr, core.list[src1] & ~(core.list[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(bic_asr, core.list[src1] & ~(core.ilist[src2] >> src3));
MAKE_SIMPLE_ARITH_LOGIC(bic_ror, core.list[src1] & ~(std::rotr(core.list[src2], src3)));

MAKE_SETTING_FLAGS(bics_shl, and_setting_flags(core, core.list[src1], ~(core.list[src2] << src3)));
MAKE_SETTING_FLAGS(bics_shr, and_setting_flags(core, core.list[src1], ~(core.list[src2] >> src3)));
MAKE_SETTING_FLAGS(bics_asr, and_setting_flags(core, core.list[src1], ~(core.ilist[src2] >> src3)));
MAKE_SETTING_FLAGS(bics_ror, and_setting_flags(core, core.list[src1], std::rotr(core.list[src2], src3)));

MAKE_SIMPLE_ARITH_LOGIC(adcs, add_with_carry_setting_flags(core, core.list[src1], core.list[src2], core.psr.CARRY));
MAKE_SIMPLE_ARITH_LOGIC(sbcs, add_with_carry_setting_flags(core, core.list[src1], ~core.list[src2], core.psr.CARRY));


// Memory
static FORCE_INLINE void memory_read_single(NGPV1& core, u32 dest, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, w, Bus::read_word, address);
}

static FORCE_INLINE void memory_read_double(NGPV1& core, u32 dest, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, dw, Bus::read_dword, address);
}

static FORCE_INLINE void memory_read_v(NGPV1& core, u32 dest, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, qw, Bus::read_qword, address);
}

static FORCE_INLINE void memory_write_single(NGPV1& core, u32 src, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, w, Bus::write_word, address);
}

static FORCE_INLINE void memory_write_double(NGPV1& core, u32 src, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, dw, Bus::write_dword, address);
}

static FORCE_INLINE void memory_write_v(NGPV1& core, u32 src, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, qw, Bus::write_qword, address);
}

static FORCE_INLINE void memory_pair_read_word(NGPV1& core, u32 dest1, u32 dest2, VirtualAddress address)
{
    HANDLE_READ(core, dest1, Bus::read_word, address);
    HANDLE_READ(core, dest2, Bus::read_word, address + 4);
}

static FORCE_INLINE void memory_pair_read_single(NGPV1& core, u32 dest1, u32 dest2, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, w, Bus::read_word, address);
    HANDLE_SIMD_READ(core, dest2, w, Bus::read_word, address + 4);
}

static FORCE_INLINE void memory_pair_read_double(NGPV1& core, u32 dest1, u32 dest2, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, dw, Bus::read_dword, address);
    HANDLE_SIMD_READ(core, dest2, dw, Bus::read_dword, address + 8);
}

static FORCE_INLINE void memory_pair_read_v(NGPV1& core, u32 dest1, u32 dest2, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, qw, Bus::read_qword, address);
    HANDLE_SIMD_READ(core, dest2, qw, Bus::read_qword, address + 16);
}

static FORCE_INLINE void memory_pair_write_word(NGPV1& core, u32 src1, u32 src2, VirtualAddress address)
{
    HANDLE_WRITE(core, src1, Word, Bus::write_word, address);
    HANDLE_WRITE(core, src2, Word, Bus::write_word, address + 4);
}

static FORCE_INLINE void memory_pair_write_single(NGPV1& core, u32 src1, u32 src2, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, w, Bus::write_word, address);
    HANDLE_SIMD_WRITE(core, src2, w, Bus::write_word, address + 4);
}

static FORCE_INLINE void memory_pair_write_double(NGPV1& core, u32 src1, u32 src2, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, dw, Bus::write_dword, address);
    HANDLE_SIMD_WRITE(core, src2, dw, Bus::write_dword, address + 8);
}

static FORCE_INLINE void memory_pair_write_v(NGPV1& core, u32 src1, u32 src2, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, qw, Bus::write_qword, address);
    HANDLE_SIMD_WRITE(core, src2, qw, Bus::write_qword, address + 16);
}

// ExtendedAlu
static FORCE_INLINE void madd(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.list[dest] = (dest != CPUCore::ZeroRegister) *
        (core.list[src3] + (core.list[src1] * core.list[src2]));
}

static FORCE_INLINE void msub(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.list[dest] = (dest != CPUCore::ZeroRegister) *
        (core.list[src3] - (core.list[src1] * core.list[src2]));
}

static FORCE_INLINE void udiv(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    if (core.list[src2] == 0)
    {
        core.make_exception(NGPV1::DivideByZeroException, 0, 0);
        return;
    }

    core.list[dest] = (dest != CPUCore::ZeroRegister) * (core.list[src1] / core.list[src2]);
}

static FORCE_INLINE void div(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    if (core.list[src2] == 0)
    {
        core.make_exception(NGPV1::DivideByZeroException, 0, 0);
        return;
    }

    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * (core.ilist[src1] / core.ilist[src2]);
}

static FORCE_INLINE void shl(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * (core.ilist[src1] << src2);
}

static FORCE_INLINE void shr(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * (core.ilist[src1] >> src2);
}

static FORCE_INLINE void asr(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * (core.ilist[src1] >> src2);
}

static FORCE_INLINE void ror(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.list[dest] = (dest != CPUCore::ZeroRegister) * (std::rotr(core.list[src1], (i32)src2));
}

static FORCE_INLINE void abs(NGPV1& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) *
        (core.ilist[src1] >= 0 ? core.ilist[src1] : -core.ilist[src1]);
}

// Non Binary
static FORCE_INLINE void ret(NGPV1& core, u8, u8 src1, u8)
{
    core.pc = core.list[src1];
    core.handle_pc_change();
}

static FORCE_INLINE void br(NGPV1& core, u8, u8 src1, u8)
{
    core.pc = core.list[src1];
    core.handle_pc_change();
}

static FORCE_INLINE void blr(NGPV1& core, u8, u8 src1, u8)
{
    core.gpr.lr = core.pc;
    core.pc = core.list[src1];
    core.handle_pc_change();
}

static FORCE_INLINE void brk(NGPV1& core, u8 op, u8 src1, u8 src2)
{
    core.handle_breakpoint(op | src1 << 6 | src2 << 11);
}

static FORCE_INLINE void svc(NGPV1& core, u8 op, u8 src1, u8 src2)
{
    core.make_exception(NGPV1::SupervisorException, 0x0, op | src1 << 6 | src2 << 11);
}

static FORCE_INLINE void evc(NGPV1& core, u8 op, u8 src1, u8 src2)
{
    core.make_exception(NGPV1::ExtendedSupervisorException, 0x0, op | src1 << 6 | src2 << 11);
}

static FORCE_INLINE void smc(NGPV1& core, u8 op, u8 src1, u8 src2)
{
    core.make_exception(NGPV1::SecureMachineControllerException, 0x0, op | src1 << 6 | src2 << 11);
}

static FORCE_INLINE void eret(NGPV1& core, u8, u8, u8) { core.return_exception(); }
static FORCE_INLINE void wfi(NGPV1& core, u8, u8, u8) {}

static FORCE_INLINE void msr(NGPV1& core, u8, u8, u8) {}
static FORCE_INLINE void mrs(NGPV1& core, u8, u8, u8) {}

static FORCE_INLINE void hlt(NGPV1& core, u8, u8, u8) { core.psr.HALT = true; }

// Main opcodes
static FORCE_INLINE void bl(NGPV1& core, u32 inst)
{
    const u32 disp_inst = inst >> 6;
    const u32 disp = inst & 0x8000000 ? (0xFC00'0000 | disp_inst) << 2 : disp_inst << 2;

    core.gpr.lr = core.pc;
    HANDLE_BRANCH(true, core, disp);
}

static FORCE_INLINE void b(NGPV1& core, u32 inst)
{
    const u32 disp_inst = inst >> 6;
    const u32 disp = inst & 0x8000000 ? (0xFC00'0000 | disp_inst) << 2 : disp_inst << 2;
    HANDLE_BRANCH(true, core, disp);
}

static FORCE_INLINE void bcond(NGPV1& core, u32 inst)
{
    const u8 cond = (inst >> 6) & 0xF;
    const i32 disp_inst = (inst >> 10);
    const i32 disp = inst & 0x8000'0000 ? (0xFFC0'0000 | disp_inst) << 2 : disp_inst << 2;

    switch (cond)
    {
    case NGP_BEQ:
        HANDLE_BRANCH(core.psr.ZERO, core, disp);
        break;
    case NGP_BNE:
        HANDLE_BRANCH(!core.psr.ZERO, core, disp);
        break;
    case NGP_BLT:
        HANDLE_BRANCH(core.psr.NEGATIVE != core.psr.OVERFLOW, core, disp);
        break;
    case NGP_BLE:
        HANDLE_BRANCH(core.psr.ZERO || core.psr.NEGATIVE ^ core.psr.OVERFLOW, core, disp);
        break;
    case NGP_BGT:
        HANDLE_BRANCH(!core.psr.ZERO && core.psr.NEGATIVE == core.psr.OVERFLOW, core, disp);
        break;
    case NGP_BGE:
        HANDLE_BRANCH(core.psr.NEGATIVE == core.psr.OVERFLOW, core, disp);
        break;
    case NGP_BCS:
        HANDLE_BRANCH(core.psr.CARRY, core, disp);
        break;
    case NGP_BCC:
        HANDLE_BRANCH(!core.psr.CARRY, core, disp);
        break;
    case NGP_BMI:
        HANDLE_BRANCH(core.psr.NEGATIVE, core, disp);
        break;
    case NGP_BPL:
        HANDLE_BRANCH(!core.psr.NEGATIVE, core, disp);
        break;
    case NGP_BVS:
        HANDLE_BRANCH(core.psr.OVERFLOW, core, disp);
        break;
    case NGP_BVC:
        HANDLE_BRANCH(!core.psr.OVERFLOW, core, disp);
        break;
    case NGP_BHI:
        break;
    case NGP_BLS:
        break;
    case NGP_BAL:
        core.pc += disp;
        break;
    default:
        // TODO: invalid instruction exception
        break;
    }
}

static FORCE_INLINE void logical_add_sub(NGPV1& core, u32 inst)
{
    const u8 las = (inst >> 6) & 0x3F;
    const u8 dest = (inst >> 12) & 0x1F;
    const u8 src1 = (inst >> 17) & 0x1F;
    const u8 src2 = (inst >> 22) & 0x1F;
    const u8 src3 = (inst >> 27) & 0x1F;

#define CASE(op, name) case op: name(core, dest, src1, src2, src3); break
    switch (las)
    {
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
        CASE(NGP_SHL, shl);
        CASE(NGP_SHR, shr);
        CASE(NGP_ASR, asr);
        CASE(NGP_ROR, ror);
        CASE(NGP_ABS, abs);
    default:
        // TODO: invalid instruction
        break;
    }
#undef CASE
}

static FORCE_INLINE void fp_op(NGPV1& core, u32 inst)
{
    const u16 fpop = (inst >> 6) & 0x7FF;
    const u8 dest = (inst >> 17) & 0x1F;
    const u8 src1 = (inst >> 22) & 0x1F;
    const u8 src2 = (inst >> 27) & 0x1F;

    switch (fpop)
    {
    case NGP_FMOV_S_S:
        core.simd[dest].s = core.simd[src1].s;
        break;
    case NGP_FMOV_D_D:
        core.simd[dest].d = core.simd[src1].d;
        break;
    case NGP_FMOV_V_V:
        core.simd[dest].vec = core.simd[src1].vec;
        break;
    case NGP_FMOV_W_S:
        core.list[dest] = (dest != ZeroRegister) * core.simd[src1].w;
        break;
    case NGP_FMOV_S_W:
        core.simd[dest].w = core.list[src1];
        break;
    case NGP_FCVT_S_D:
        core.simd[dest].s = core.simd[src1].d;
        break;
    case NGP_FCVT_D_S:
        core.simd[dest].d = core.simd[src1].s;
        break;
    case NGP_SCVTF_S_W:
        core.simd[dest].s = core.ilist[src1];
        break;
    case NGP_SCVTF_D_W:
        core.simd[dest].d = core.ilist[src1];
        break;
    case NGP_UCVTF_S_W:
        core.simd[dest].s = core.list[src1];
        break;
    case NGP_UCVTF_D_W:
        core.simd[dest].d = core.list[src1];
        break;
    case NGP_FADD_S:
        core.simd[dest].s = core.simd[src1].s + core.simd[src2].s;
        break;
    case NGP_FSUB_S:
        core.simd[dest].s = core.simd[src1].s - core.simd[src2].s;
        break;
    case NGP_FMUL_S:
        core.simd[dest].s = core.simd[src1].s * core.simd[src2].s;
        break;
    case NGP_FDIV_S:
        if (core.simd[src2].s == 0)
        {
            core.make_exception(NGPV1::DivideByZeroException, 0, 0);
            break;
        }
        core.simd[dest].s = core.simd[src1].s / core.simd[src2].s;
        break;
    case NGP_FADD_D:
        core.simd[dest].d = core.simd[src1].d + core.simd[src2].d;
        break;
    case NGP_FSUB_D:
        core.simd[dest].d = core.simd[src1].d - core.simd[src2].d;
        break;
    case NGP_FMUL_D:
        core.simd[dest].d = core.simd[src1].d * core.simd[src2].d;
        break;
    case NGP_FDIV_D:
        if (core.simd[src2].d == 0)
        {
            core.make_exception(NGPV1::DivideByZeroException, 0, 0);
            break;
        }
        core.simd[dest].d = core.simd[src1].d / core.simd[src2].d;
        break;
    case NGP_FABS_S:
        core.simd[dest].s = std::fabs(core.simd[dest].s);
        break;
    case NGP_FABS_D:
        core.simd[dest].d = std::fabs(core.simd[dest].d);
        break;
    case NGP_FNEG_S:
        core.simd[dest].s = -core.simd[dest].s;
        break;
    case NGP_FNEG_D:
        core.simd[dest].d = -core.simd[dest].d;
        break;
    case NGP_FINS_V_S4_W:
        core.simd[dest].vec.s4[src1 & 0x3] = core.list[src2];
        break;
    case NGP_FSMOV_W_V_S4:
        core.ilist[dest] = core.simd[src1].vec.s4[src2 & 0x3];
        break;
    case NGP_FUMOV_W_V_S4:
        core.list[dest] = core.simd[src1].vec.s4[src2 & 0x3];
        break;
    case NGP_FDUP_S_V_S4:
    {
        NGPV1::Vec128 vec = core.simd[src1].vec;
        core.simd[dest].s = vec.s4[src2 & 0x3];
    }
        break;
    case NGP_FDUP_D_V_D2:
    {
        NGPV1::Vec128 vec = core.simd[src1].vec;
        core.simd[dest].d = vec.d2[src2 & 0x1];
    }
        break;
    case NGP_FDUP_V_V_S4:
    {
        NGPV1::Vec128 vec = core.simd[src1].vec;
        core.simd[dest].vec.s4[0] = vec.s4[src2 & 0x3];
        core.simd[dest].vec.s4[1] = vec.s4[src2 & 0x3];
        core.simd[dest].vec.s4[2] = vec.s4[src2 & 0x3];
        core.simd[dest].vec.s4[3] = vec.s4[src2 & 0x3];
    }
        break;
    case NGP_FDUP_V_V_D2:
    {
        NGPV1::Vec128 vec = core.simd[src1].vec;
        core.simd[dest].vec.d2[0] = vec.d2[src2 & 0x1];
        core.simd[dest].vec.d2[1] = vec.d2[src2 & 0x1];
    }
        break;
    case NGP_FADD_V_S4:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        NGPV1::Vec128 op2 = core.simd[src2].vec;
        core.simd[dest].vec.s4[0] = op1.s4[0] + op2.s4[0];
        core.simd[dest].vec.s4[1] = op1.s4[1] + op2.s4[1];
        core.simd[dest].vec.s4[2] = op1.s4[2] + op2.s4[2];
        core.simd[dest].vec.s4[3] = op1.s4[3] + op2.s4[3];
    }
    break;
    case NGP_FSUB_V_S4:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        NGPV1::Vec128 op2 = core.simd[src2].vec;
        core.simd[dest].vec.s4[0] = op1.s4[0] - op2.s4[0];
        core.simd[dest].vec.s4[1] = op1.s4[1] - op2.s4[1];
        core.simd[dest].vec.s4[2] = op1.s4[2] - op2.s4[2];
        core.simd[dest].vec.s4[3] = op1.s4[3] - op2.s4[3];
    }
        break;
    case NGP_FMUL_V_S4:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        NGPV1::Vec128 op2 = core.simd[src2].vec;
        core.simd[dest].vec.s4[0] = op1.s4[0] * op2.s4[0];
        core.simd[dest].vec.s4[1] = op1.s4[1] * op2.s4[1];
        core.simd[dest].vec.s4[2] = op1.s4[2] * op2.s4[2];
        core.simd[dest].vec.s4[3] = op1.s4[3] * op2.s4[3];
    }
        break;
    case NGP_FDIV_V_S4:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        NGPV1::Vec128 op2 = core.simd[src2].vec;
        if (op2.s4[0] == 0 || op2.s4[1] == 0 || op2.s4[2] == 0 || op2.s4[3] == 0)
        {
            core.make_exception(NGPV1::DivideByZeroException, 0, 0);
            break;
        }
        core.simd[dest].vec.s4[0] = op1.s4[0] / op2.s4[0];
        core.simd[dest].vec.s4[1] = op1.s4[1] / op2.s4[1];
        core.simd[dest].vec.s4[2] = op1.s4[2] / op2.s4[2];
        core.simd[dest].vec.s4[3] = op1.s4[3] / op2.s4[3];
    }
        break;
    case NGP_FADD_V_D2:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        NGPV1::Vec128 op2 = core.simd[src2].vec;
        core.simd[dest].vec.d2[0] = op1.d2[0] + op2.d2[0];
        core.simd[dest].vec.d2[1] = op1.d2[1] + op2.d2[1];
    }
    break;
    case NGP_FSUB_V_D2:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        NGPV1::Vec128 op2 = core.simd[src2].vec;
        core.simd[dest].vec.d2[0] = op1.s4[0] - op2.d2[0];
        core.simd[dest].vec.d2[1] = op1.s4[1] - op2.d2[1];
    }
        break;
    case NGP_FMUL_V_D2:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        NGPV1::Vec128 op2 = core.simd[src2].vec;
        core.simd[dest].vec.d2[0] = op1.d2[0] * op2.d2[0];
        core.simd[dest].vec.d2[1] = op1.d2[1] * op2.d2[1];
    }
        break;
    case NGP_FDIV_V_D2:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        NGPV1::Vec128 op2 = core.simd[src2].vec;
        if (op2.d2[0] == 0 || op2.d2[1] == 0)
        {
            core.make_exception(NGPV1::DivideByZeroException, 0, 0);
            break;
        }
        core.simd[dest].vec.d2[0] = op1.d2[0] / op2.d2[0];
        core.simd[dest].vec.d2[1] = op1.d2[1] / op2.d2[1];
    }
        break;
    case NGP_FNEG_V_S4:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        core.simd[dest].vec.s4[0] = -op1.s4[0];
        core.simd[dest].vec.s4[1] = -op1.s4[1];
        core.simd[dest].vec.s4[2] = -op1.s4[2];
        core.simd[dest].vec.s4[3] = -op1.s4[3];
    }
        break;
    case NGP_FNEG_V_D2:
    {
        NGPV1::Vec128 op1 = core.simd[src1].vec;
        core.simd[dest].vec.d2[0] = -op1.d2[0];
        core.simd[dest].vec.d2[1] = -op1.d2[1];
    }
    break;
    }
}

static FORCE_INLINE void load_store_immediate(NGPV1& core, u32 inst)
{
    const u8 memopc = (inst >> 6) & 0x7;
    const u8 dest_src = (inst >> 9) & 0x1F;
    const u8 base = (inst >> 14) & 0x1F;
    const i16 imm_inst = (inst >> 20);
    const i16 imm = inst & 0x8'0000 ? -imm_inst : imm_inst;

#define CASE_R(op, func, shift_amount) case op: \
    HANDLE_READ(core, dest_src, func, core.list[base] + (imm << shift_amount)); break
#define CASE_R_SIGNED(op, func, shift_amount) case op: \
    HANDLE_READ_SIGNED(core, dest_src, func, core.list[base] + (imm << shift_amount)); break
#define CASE_W(op, type, func, shift_amount) case op: \
    HANDLE_WRITE(core, dest_src, type, func, core.list[base] + (imm << shift_amount)); break

    switch (memopc)
    {
        CASE_R(NGP_LD_IMMEDIATE, Bus::read_word, 2);
        CASE_R_SIGNED(NGP_LDSH_IMMEDIATE, Bus::read_ihalf, 1);
        CASE_R(NGP_LDH_IMMEDIATE, Bus::read_half, 1);
        CASE_R_SIGNED(NGP_LDSB_IMMEDIATE, Bus::read_ibyte, 0);
        CASE_R(NGP_LDB_IMMEDIATE, Bus::read_byte, 0);
        CASE_W(NGP_ST_IMMEDIATE, Word, Bus::write_word, 2);
        CASE_W(NGP_STH_IMMEDIATE, u16, Bus::write_half, 1);
        CASE_W(NGP_STB_IMMEDIATE, u8, Bus::write_byte, 1);
    default:
        break;
    }
#undef CASE_R
#undef CASE_R_SIGNED
#undef CASE_W
}

static FORCE_INLINE void load_store_fp_immediate(NGPV1& core, u32 inst)
{
    const u8 memopc = (inst >> 6) & 0x7;
    const u8 dest_src = (inst >> 9) & 0x1F;
    const u8 base = (inst >> 14) & 0x1F;
    const i16 imm_inst = (inst >> 20);
    const i16 imm = inst & 0x8'0000 ? -imm_inst : imm_inst;

    switch (memopc)
    {
    case NGP_LD_S_IMMEDIATE:
        core.simd[dest_src].w = Bus::read_word(core.list[base] + (imm << 2));
        break;
    case NGP_LD_D_IMMEDIATE:
        core.simd[dest_src].dw = Bus::read_dword(core.list[base] + (imm << 3));
        break;
    case NGP_LD_V_IMMEDIATE:
        core.simd[dest_src].qw = Bus::read_qword(core.list[base] + (imm << 4));
        break;
    case NGP_ST_S_IMMEDIATE:
        Bus::write_word(core.list[base] + (imm << 2), core.simd[dest_src].w);
        break;
    case NGP_ST_D_IMMEDIATE:
        Bus::write_dword(core.list[base] + (imm << 3), core.simd[dest_src].dw);
        break;
    case NGP_ST_V_IMMEDIATE:
        Bus::write_qword(core.list[base] + (imm << 4), core.simd[dest_src].qw);
        break;
    }
}

static FORCE_INLINE void load_store_register(NGPV1& core, u32 inst)
{
    const u16 opc = (inst >> 6) & 0x7FF;
	const u8 dest_src = (inst >> 17) & 0x1F;
    const u8 base = (inst >> 22) & 0x1F;
	const u8 index = (inst >> 27) & 0x1F;

#define CASE_R(op, func) case op: \
    HANDLE_READ(core, dest_src, func, core.list[base] + core.list[index]); break
#define CASE_R_SIGNED(op, func) case op: \
    HANDLE_READ_SIGNED(core, dest_src, func, core.list[base] + core.list[index]); break
#define CASE_SIMD_R(op, field, func) case op: \
    HANDLE_SIMD_READ(core, dest_src, field, func, core.list[base] + core.list[index]); break

#define CASE_W(op, type, func) case op: \
    HANDLE_WRITE(core, dest_src, type, func, core.list[base] + core.list[index]); break
#define CASE_SIMD_W(op, field, func) case op: \
    HANDLE_SIMD_WRITE(core, dest_src, field, func, core.list[base] + core.list[index]); break

    switch (opc)
    {
        CASE_R(NGP_LD, Bus::read_word);
        CASE_R_SIGNED(NGP_LDSH, Bus::read_ihalf);
        CASE_R(NGP_LDH, Bus::read_half);
        CASE_R_SIGNED(NGP_LDSB, Bus::read_ibyte);
        CASE_R(NGP_LDB, Bus::read_byte);
        CASE_W(NGP_ST, Word, Bus::write_word);
        CASE_W(NGP_STH, u16, Bus::write_word);
        CASE_W(NGP_STB, u8, Bus::write_word);
        CASE_SIMD_R(NGP_LD_S, w, Bus::read_word);
        CASE_SIMD_R(NGP_LD_D, dw, Bus::read_dword);
        CASE_SIMD_R(NGP_LD_V, qw, Bus::read_qword);
        CASE_SIMD_W(NGP_ST_S, w, Bus::write_word);
        CASE_SIMD_W(NGP_ST_D, dw, Bus::write_dword);
        CASE_SIMD_W(NGP_ST_V, qw, Bus::write_qword);
    }
#undef CASE_R
#undef CASE_R_SIGNED
#undef CASE_SIMD_R
#undef CASE_W
#undef CASE_SIMD_W
}

static FORCE_INLINE void load_store_pair(NGPV1& core, u32 inst) {}

static FORCE_INLINE void extendedalu(NGPV1& core, u32 inst)
{
    const u8 extopc = (inst >> 6) & 0x3F;
    const u8 dest = (inst >> 12) & 0x1F;
    const u8 src1 = (inst >> 17) & 0x1F;
    const u8 src2 = (inst >> 22) & 0x1F;
    const u8 src3 = (inst >> 27) & 0x1F;

#define CASE(_op, name) case _op: name(core, dest, src1, src2, src3); break
    switch (extopc)
    {
        CASE(NGP_MADD, madd);
        CASE(NGP_MSUB, msub);
        CASE(NGP_UDIV, udiv);
        CASE(NGP_DIV, div);
    default:
        break;
    }
#undef CASE
}

static FORCE_INLINE void non_binary(NGPV1& core, u32 inst)
{
    const u16 nonopc = (inst >> 6) & 0x3FF;
    const u8 op = (inst >> 16) & 0x3F;
    const u8 src1 = (inst >> 22) & 0x1F;
    const u8 src2 = (inst >> 27) & 0x1F;

#define CASE(_op, name) case _op: name(core, op, src1, src2); break
    switch (nonopc)
    {
        CASE(NGP_RET, ret);
        CASE(NGP_BR, br);
        CASE(NGP_BLR, blr);
        CASE(NGP_BRK, brk);
        CASE(NGP_SVC, svc);
        CASE(NGP_SMC, smc);
        CASE(NGP_ERET, eret);
        CASE(NGP_WFI, wfi);
        CASE(NGP_MSR, msr);
        CASE(NGP_MRS, mrs);
        CASE(NGP_HALT, hlt);
    case NGP_NOP:
    default:
        break;
    }
#undef CASE
}

static FORCE_INLINE void ld_pc(NGPV1& core, u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.list[dest] = (dest != CPUCore::ZeroRegister) * Bus::read_word(core.pc + disp);
}

static FORCE_INLINE void ld_s_pc(NGPV1& core, u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.simd[dest].w = Bus::read_word(core.pc + disp);
}

static FORCE_INLINE void ld_d_pc(NGPV1& core, u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.simd[dest].dw = Bus::read_dword(core.pc + disp);
}

static FORCE_INLINE void ld_v_pc(NGPV1& core, u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.simd[dest].qw = Bus::read_qword(core.pc + disp);
}

static FORCE_INLINE void adr_pc(NGPV1& core, u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.list[dest] = (dest != CPUCore::ZeroRegister) * (core.pc + disp);
}

static FORCE_INLINE void immediate(NGPV1& core, u32 inst)
{
    const u16 imm = inst >> 16;
    const u8 immopc = (inst >> 6) & 0x1F;
    const u8 reg = (inst >> 11) & 0x1F;

    if (immopc == NGP_MOVT_IMMEDIATE)
    {
        core.list[reg] |= (imm << 16);
    }
}

MAKE_IMMEDIATE_OP(add_immediate, core.list[src] + imm);

static FORCE_INLINE void adds_immediate(NGPV1& core, u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u8 src = (inst >> 11) & 0x1F;
    const u16 imm = inst >> 16;
    const u32 result = add_with_carry_setting_flags(core, core.list[src], imm, 0);
    core.list[dest] = dest == CPUCore::ZeroRegister ? result : 0;
}

MAKE_IMMEDIATE_OP(sub_immediate, core.list[src] - imm);

static FORCE_INLINE void subs_immediate(NGPV1& core, u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u8 src = (inst >> 11) & 0x1F;
    const u16 imm = inst >> 16;
    const u32 result = add_with_carry_setting_flags(core, core.list[src], ~imm, 1);
    core.list[dest] = dest == CPUCore::ZeroRegister ? 0 : result;
}

MAKE_IMMEDIATE_OP(and_immediate, core.list[src] & imm);

static FORCE_INLINE void ands_immediate(NGPV1& core, u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u8 src = (inst >> 11) & 0x1F;
    const u16 imm = inst >> 16;
    const u32 result = and_setting_flags(core, core.list[src], imm);
    core.list[dest] = dest == CPUCore::ZeroRegister ? result : 0;
}

MAKE_IMMEDIATE_OP(or_immediate, core.list[src] | imm);
MAKE_IMMEDIATE_OP(eor_immediate, core.list[src] ^ imm);

static FORCE_INLINE void tbz(NGPV1& core, u32 inst) {}
static FORCE_INLINE void tbnz(NGPV1& core, u32 inst) {}
static FORCE_INLINE void cbz(NGPV1& core, u32 inst) {}
static FORCE_INLINE void cbnz(NGPV1& core, u32 inst) {}

static FORCE_INLINE void fp_4op(NGPV1& core, u32 inst)
{
    const u8 fpopc = (inst >> 6) & 0x3F;
    const u8 dest = (inst >> 12) & 0x1F;
    const u8 src1 = (inst >> 17) & 0x1F;
    const u8 src2 = (inst >> 22) & 0x1F;
    const u8 src3 = (inst >> 27) & 0x1F;

    switch (fpopc)
    {
    case NGP_FMADD_S:
        core.simd[dest].s = core.simd[src3].s + (core.simd[src1].s * core.simd[src2].s);
        break;
    case NGP_FMADD_D:
        core.simd[dest].d = core.simd[src3].d + (core.simd[src1].d * core.simd[src2].d);
        break;
    case NGP_FMSUB_S:
        core.simd[dest].s = core.simd[src3].s - (core.simd[src1].s * core.simd[src2].s);
        break;
    case NGP_FMSUB_D:
        core.simd[dest].d = core.simd[src3].d - (core.simd[src1].d * core.simd[src2].d);
        break;
    case NGP_FINS_V_S4:
        core.simd[dest].vec.s4[src1 & 0x3] = core.simd[src2].vec.s4[src3 & 0x3];
        break;
    case NGP_FINS_V_D2:
        core.simd[dest].vec.d2[src1 & 0x1] = core.simd[src2].vec.d2[src3 & 0x1];
        break;
    }
}


void NGPV1::initialize()
{
    mem_pc = (Word*)Bus::get_physical_addr(0);
}

void NGPV1::shutdown() {}


usize NGPV1::dispatch(usize num_cycles)
{
    return run(num_cycles);
}

void NGPV1::print_registers()
{
    for (u32 i = 0; i < 29; i++)
    {
        printf("R%d = %08X\n", i, list[i]);
    }
    printf("SP = %08X LR = %08X PC = %08X\n", gpr.sp, gpr.lr, pc);

    for (u32 i = 0; i < 32; i++)
    {
        printf("S%d = %f, D%d = %f, Q%d = { %f, %f, %f, %f }\n", 
            i, simd[i].s,
            i, simd[i].d,
            i, simd[i].vec.vec4.x, simd[i].vec.vec4.y, simd[i].vec.vec4.z, simd[i].vec.vec4.w
        );
    }
}

void NGPV1::set_psr(ProgramStateRegister psr)
{}

CPUCore::ProgramStateRegister NGPV1::get_psr()
{
    return psr;
}

void NGPV1::set_pc(VirtualAddress new_pc)
{
    pc = new_pc;
}

VirtualAddress NGPV1::get_pc()
{
    return pc;
}

void NGPV1::set_clock_speed(usize new_clock_speed)
{
    clock_speed = new_clock_speed;
}

usize NGPV1::get_clock_speed()
{
    return clock_speed;
}

void NGPV1::handle_exception(ExceptionCode code, VirtualAddress addr)
{
    switch (code)
    {
    case DivideByZeroException:
    case SupervisorException:
    case ExtendedSupervisorException:
    case SecureMachineControllerException:
    case Breakpoint:
    case InvalidRead:
    case InvalidWrite:
    case AccessViolation:
        break;
    }
}

usize NGPV1::run(usize num_cycles)
{
    usize cycle_count = num_cycles;

    // One cycle for fetching, decoding and simple arithmetic
    while (num_cycles && !psr.HALT)
    {
        num_cycles--;

        u32 inst = fetch_inst();

        pc += 4;

        u8 opcode = inst & 0x3F;

#define CASE(op, name) case op: name(*this, inst); break
        switch (opcode)
        {
            CASE(NGP_BL, bl);
            CASE(NGP_B, b);
            CASE(NGP_B_COND, bcond);
            CASE(NGP_ALU, logical_add_sub);
            CASE(NGP_FP_OP, fp_op);
            CASE(NGP_LOAD_STORE_IMMEDIATE, load_store_immediate);
            CASE(NGP_LOAD_STORE_FP_IMMEDIATE, load_store_fp_immediate);
            CASE(NGP_LOAD_STORE_REGISTER, load_store_register);
            CASE(NGP_LOAD_STORE_PAIR, load_store_pair);
            CASE(NGP_EXTENDEDALU, extendedalu);
            CASE(NGP_NON_BINARY, non_binary);
            CASE(NGP_LD_PC, ld_pc);
            CASE(NGP_LD_S_PC, ld_s_pc);
            CASE(NGP_LD_D_PC, ld_d_pc);
            CASE(NGP_LD_V_PC, ld_v_pc);
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
            CASE(NGP_FP_4OP, fp_4op);
        default:
            break;
        }
#undef CASE
    }
    return num_cycles;
}

void NGPV1::handle_pc_change()
{}

Word NGPV1::fetch_inst()
{
    return mem_pc[(pc >> 2)];
}

void NGPV1::make_exception(ExceptionCode code, VirtualAddress vec_offset, u16 comment)
{
    switch (code)
    {
    case SupervisorException:
    case ExtendedSupervisorException:
    case SecureMachineControllerException:
    {
        u8 target_exception_level = code - SupervisorException;
        elr.elr_el[target_exception_level] = pc;
        spsr.spsr[target_exception_level] = psr;
        psr.CURRENT_EL = target_exception_level + 1;
        ecr.ecr_el[target_exception_level] = comment;

        VirtualAddress vba = vbar.vbar_el[target_exception_level];
        if (Bus::check_virtual_address(vba, Bus::WriteableAddress) == Bus::ValidAddress
            && Bus::check_virtual_address(vba + VBASize - 1, Bus::WriteableAddress) == Bus::ValidAddress)
        {
            pc = Bus::read_word(vba + vec_offset);
        }
        else
        {
            hlt(*this, 0, 0, 0);
        }
    }
    break;
    case Breakpoint:
    case InvalidRead:
    case InvalidWrite:
    case AccessViolation:
    case DivideByZeroException:
        break;
    break;
    }
}

void NGPV1::return_exception()
{
    VirtualAddress target_pc = elr.elr_el[psr.CURRENT_EL];
    ProgramStateRegister last_psr = spsr.spsr[psr.CURRENT_EL];

    psr = last_psr;
    pc = target_pc;
    handle_pc_change();
}

void NGPV1::handle_breakpoint(u16 comment)
{
    
}

