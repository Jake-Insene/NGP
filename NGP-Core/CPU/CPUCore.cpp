/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/CPUCore.h"

#include "FileFormat/ISA.h"
#include "Memory/Bus.h"

#include <cstdio>
#include <bit>

#undef OVERFLOW


#define MAKE_SIMPLE_ARITH_LOGIC(name, op) \
    static FORCE_INLINE  void name(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        core.list[dest] = (dest != ZeroRegister) * (op);\
    }

#define MAKE_SIMPLE_OP(name, op) \
    static FORCE_INLINE  void name(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        core.list[dest] = (dest != ZeroRegister) * (op);\
    }

#define MAKE_SETTING_FLAGS(name, op) \
    static FORCE_INLINE  void name(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3) {\
        core.list[dest] = (dest != ZeroRegister) * (op);\
    }

#define MAKE_IMMEDIATE_OP(name, op) \
    static FORCE_INLINE  void name(CPUCore& core, u32 inst)\
    {\
        u8 dest = (inst >> 6) & 0x1F;\
        u8 src = (inst >> 11) & 0x1F;\
        u16 imm = inst >> 16;\
        core.list[dest] = (dest != ZeroRegister) * op;\
    }

#define HANDLE_BRANCH(cond, core, disp) if(cond) { core.pc += disp; core.handle_pc_change(); }

#define HANDLE_READ(core, dest, func, address) \
    core.list[dest] = (dest != CPUCore::ZeroRegister) * func(core, address)

#define HANDLE_READ_SIGNED(core, dest, func, address) \
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * func(core, address)

#define HANDLE_SIMD_READ(core, dest, index, func, address) \
    core.simd[dest].index = func(core, address)

#define HANDLE_WRITE(core, src, type, func, address) \
    func(core, address, static_cast<type>(core.list[src]));

#define HANDLE_SIMD_WRITE(core, src, index, func, address) \
    func(core, address, core.simd[src].index)

// Logical Add sub
static FORCE_INLINE void set_flags(CPUCore& core, u32 src1, u32 src2, u64 res, bool is_sub)
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
static FORCE_INLINE u32 add_with_carry_setting_flags(CPUCore& core, u32 src1, u32 src2, u32 carry)
{
    const u64 lhs = src1;
    const u64 rhs = src2;
    const u64 res = lhs + rhs + carry;
    set_flags(core, src1, src2, res, false);

    return u32(res);
}

static u32 FORCE_INLINE and_setting_flags(CPUCore& core, u32 src1, u32 src2)
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
static FORCE_INLINE void memory_read_single(CPUCore& core, u32 dest, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, w, Bus::read_word, address);
}

static FORCE_INLINE void memory_read_double(CPUCore& core, u32 dest, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, dw, Bus::read_dword, address);
}

static FORCE_INLINE void memory_read_qword(CPUCore& core, u32 dest, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, qw, Bus::read_qword, address);
}

static FORCE_INLINE void memory_write_single(CPUCore& core, u32 src, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, w, Bus::write_word, address);
}

static FORCE_INLINE void memory_write_double(CPUCore& core, u32 src, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, dw, Bus::write_dword, address);
}

static FORCE_INLINE void memory_write_qword(CPUCore& core, u32 src, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, qw, Bus::write_qword, address);
}

static FORCE_INLINE void memory_pair_read_word(CPUCore& core, u32 dest1, u32 dest2, VirtualAddress address)
{
    HANDLE_READ(core, dest1, Bus::read_word, address);
    HANDLE_READ(core, dest2, Bus::read_word, address + 4);
}

static FORCE_INLINE void memory_pair_read_single(CPUCore& core, u32 dest1, u32 dest2, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, w, Bus::read_word, address);
    HANDLE_SIMD_READ(core, dest2, w, Bus::read_word, address + 4);
}

static FORCE_INLINE void memory_pair_read_double(CPUCore& core, u32 dest1, u32 dest2, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, dw, Bus::read_dword, address);
    HANDLE_SIMD_READ(core, dest2, dw, Bus::read_dword, address + 8);
}

static FORCE_INLINE void memory_pair_read_qword(CPUCore& core, u32 dest1, u32 dest2, VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, qw, Bus::read_qword, address);
    HANDLE_SIMD_READ(core, dest2, qw, Bus::read_qword, address + 16);
}

static FORCE_INLINE void memory_pair_write_word(CPUCore& core, u32 src1, u32 src2, VirtualAddress address)
{
    HANDLE_WRITE(core, src1, Word, Bus::write_word, address);
    HANDLE_WRITE(core, src2, Word, Bus::write_word, address + 4);
}

static FORCE_INLINE void memory_pair_write_single(CPUCore& core, u32 src1, u32 src2, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, w, Bus::write_word, address);
    HANDLE_SIMD_WRITE(core, src2, w, Bus::write_word, address + 4);
}

static FORCE_INLINE void memory_pair_write_double(CPUCore& core, u32 src1, u32 src2, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, dw, Bus::write_dword, address);
    HANDLE_SIMD_WRITE(core, src2, dw, Bus::write_dword, address + 8);
}

static FORCE_INLINE void memory_pair_write_qword(CPUCore& core, u32 src1, u32 src2, VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, qw, Bus::write_qword, address);
    HANDLE_SIMD_WRITE(core, src2, qw, Bus::write_qword, address + 16);
}

// ExtendedAlu
static FORCE_INLINE void madd(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.list[dest] = (dest != ZeroRegister) * 
        (core.list[src3] + (core.list[src1] * core.list[src2]));
}

static FORCE_INLINE void msub(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.list[dest] = (dest != ZeroRegister) *
        (core.list[src3] - (core.list[src1] * core.list[src2]));
}

static FORCE_INLINE void udiv(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    if (core.list[src2] == 0)
    {
        core.make_exception(CPUCore::DivideByZeroException, 0, 0);
        return;
    }

    core.list[dest] = (dest != ZeroRegister) * (core.list[src1] / core.list[src2]);
}

static FORCE_INLINE void div(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    if (core.list[src2] == 0)
    {
        core.make_exception(CPUCore::DivideByZeroException, 0, 0);
        return;
    }

    core.ilist[dest] = (dest != ZeroRegister) * (core.ilist[src1] / core.ilist[src2]);
}

static FORCE_INLINE void shl(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.ilist[dest] = (dest != ZeroRegister) * (core.ilist[src1] << src2);
}

static FORCE_INLINE void shr(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.ilist[dest] = (dest != ZeroRegister) * (core.ilist[src1] >> src2);
}

static FORCE_INLINE void asr(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.ilist[dest] = (dest != ZeroRegister) * (core.ilist[src1] >> src2);
}

static FORCE_INLINE void ror(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.list[dest] = (dest != ZeroRegister) * (std::rotr(core.list[src1], (i32)src2));
}

static FORCE_INLINE void abs(CPUCore& core, u8 dest, u8 src1, u8 src2, u8 src3)
{
    core.ilist[dest] = (dest != ZeroRegister) *
        (core.ilist[src1] >= 0 ? core.ilist[src1] : -core.ilist[src1]);
}

// Non Binary
static FORCE_INLINE void ret(CPUCore& core, u8, u8 src1, u8)
{
    core.pc = core.list[src1];
    core.handle_pc_change();
}

static FORCE_INLINE void br(CPUCore& core, u8, u8 src1, u8)
{
    core.pc = core.list[src1];
    core.handle_pc_change();
}

static FORCE_INLINE void blr(CPUCore& core, u8, u8 src1, u8)
{
    core.gpr.lr = core.pc;
    core.pc = core.list[src1];
    core.handle_pc_change();
}

static FORCE_INLINE void brk(CPUCore& core, u8 op, u8 src1, u8 src2)
{
    core.handle_breakpoint(op | src1 << 6 | src2 << 11);
}

static FORCE_INLINE void svc(CPUCore& core, u8 op, u8 src1, u8 src2)
{
    core.make_exception(CPUCore::SupervisorException, 0x0, op | src1 << 6 | src2 << 11);
}

static FORCE_INLINE void evc(CPUCore& core, u8 op, u8 src1, u8 src2)
{
    core.make_exception(CPUCore::ExtendedSupervisorException, 0x0, op | src1 << 6 | src2 << 11);
}

static FORCE_INLINE void smc(CPUCore& core, u8 op, u8 src1, u8 src2)
{
    core.make_exception(CPUCore::SecureMachineControllerException, 0x0, op | src1 << 6 | src2 << 11);
}

static FORCE_INLINE void eret(CPUCore& core, u8, u8, u8) { core.return_exception(); }
static FORCE_INLINE void wfi(CPUCore& core, u8, u8, u8) {}

static FORCE_INLINE void msr(CPUCore& core, u8, u8, u8) {}
static FORCE_INLINE void mrs(CPUCore& core, u8, u8, u8) {}

static FORCE_INLINE void hlt(CPUCore& core, u8, u8, u8) { core.psr.HALT = true; }

// Main opcodes
static FORCE_INLINE void bl(CPUCore& core, u32 inst)
{
    const u32 disp_inst = inst >> 6;
    const u32 disp = inst & 0x8000000 ? (0xFC00'0000 | disp_inst) << 2 : disp_inst << 2;

    core.gpr.lr = core.pc;
    HANDLE_BRANCH(true, core, disp);
}

static FORCE_INLINE void b(CPUCore& core, u32 inst)
{
    const u32 disp_inst = inst >> 6;
    const u32 disp = inst & 0x8000000 ? (0xFC00'0000 | disp_inst) << 2 : disp_inst << 2;
    HANDLE_BRANCH(true, core, disp);
}

 static FORCE_INLINE void bcond(CPUCore& core, u32 inst)
{
    u8 cond = (inst >> 6) & 0xF;
    i32 disp = (inst >> 10);
    disp = disp & 0x200000 ? 0xFFC0'0000 | disp : disp;
    disp <<= 2;

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

 static FORCE_INLINE void logical_add_sub(CPUCore& core, u32 inst)
{
    u8 las = (inst >> 6) & 0x3F;
    u8 dest = (inst >> 12) & 0x1F;
    u8 src1 = (inst >> 17) & 0x1F;
    u8 src2 = (inst >> 22) & 0x1F;
    u8 src3 = (inst >> 27) & 0x1F;

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
    default:
        // TODO: invalid instruction
        break;
    }
#undef CASE
}

 static FORCE_INLINE void fp_op(CPUCore& core, u32 inst) {}

 static FORCE_INLINE void load_store_immediate(CPUCore& core, u32 inst)
{
    u8 memopc = (inst >> 6) & 0x7;

    u8 dest_src = (inst >> 9) & 0x1F;
    u8 base = (inst >> 14) & 0x1F;
    i16 imm = (inst >> 20);
    imm = inst & 0x8'0000 ? -imm : imm;

#define CASE_R(op, func) case op: \
    HANDLE_READ(core, dest_src, func, core.list[base] + imm); break
#define CASE_R_SIGNED(op, func) case op: \
    HANDLE_READ_SIGNED(core, dest_src, func, core.list[base] + imm); break
#define CASE_W(op, type, func) case op: \
    HANDLE_WRITE(core, dest_src, type, func, core.list[base] + imm); break

    switch (memopc)
    {
        CASE_R(NGP_LD_IMMEDIATE, Bus::read_word);
        CASE_R_SIGNED(NGP_LDSH_IMMEDIATE, Bus::read_ihalf);
        CASE_R(NGP_LDH_IMMEDIATE, Bus::read_half);
        CASE_R_SIGNED(NGP_LDSB_IMMEDIATE, Bus::read_ibyte);
        CASE_R(NGP_LDB_IMMEDIATE, Bus::read_byte);
        CASE_W(NGP_ST_IMMEDIATE, Word, Bus::write_word);
        CASE_W(NGP_STH_IMMEDIATE, u16, Bus::write_half);
        CASE_W(NGP_STB_IMMEDIATE, u8, Bus::write_byte);
    default:
        break;
    }
#undef CASE_R
#undef CASE_R_SIGNED
#undef CASE_W
}

static FORCE_INLINE void load_store_fp_immediate(CPUCore& core, u32 inst) {}

static FORCE_INLINE void load_store_register(CPUCore& core, u32 inst)
{
    u16 opc = (inst >> 6) & 0x7FF;
	u8 dest_src = (inst >> 17) & 0x1F;
    u8 base = (inst >> 22) & 0x1F;
	u8 index = (inst >> 27) & 0x1F;

#define CASE_R(op, func) case op: \
    HANDLE_READ(core, dest_src, func, core.list[base] + core.list[index]); break
#define CASE_R_SIGNED(op, func) case op: \
    HANDLE_READ_SIGNED(core, dest_src, func, core.list[base] + core.list[index]); break
#define CASE_SIMD_R(op, field, func) case op: \
    HANDLE_SIMD_READ(core, dest_src, ##field, func, core.list[base] + core.list[index]); break

#define CASE_W(op, type, func) case op: \
    HANDLE_WRITE(core, dest_src, ##type, func, core.list[base] + core.list[index]); break
#define CASE_SIMD_W(op, field, func) case op: \
    HANDLE_SIMD_WRITE(core, dest_src, ##field, func, core.list[base] + core.list[index]); break

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
        CASE_SIMD_R(NGP_LD_Q, qw, Bus::read_qword);
        CASE_SIMD_W(NGP_ST_S, w, Bus::write_word);
        CASE_SIMD_W(NGP_ST_D, dw, Bus::write_dword);
        CASE_SIMD_W(NGP_ST_Q, qw, Bus::write_qword);
    }
#undef CASE_R
#undef CASE_R_SIGNED
#undef CASE_SIMD_R
#undef CASE_W
#undef CASE_SIMD_W
}

static FORCE_INLINE void load_store_pair(CPUCore& core, u32 inst) {}

static FORCE_INLINE void extendedalu(CPUCore& core, u32 inst)
{
    u8 extopc = (inst >> 6) & 0x3F;
    u8 dest = (inst >> 12) & 0x1F;
    u8 src1 = (inst >> 17) & 0x1F;
    u8 src2 = (inst >> 22) & 0x1F;
    u8 src3 = (inst >> 27) & 0x1F;

#define CASE(_op, name) case _op: name(core, dest, src1, src2, src3); break
    switch (extopc)
    {
        CASE(NGP_MADD, madd);
        CASE(NGP_MSUB, msub);
        CASE(NGP_UDIV, udiv);
        CASE(NGP_DIV, div);
        CASE(NGP_SHL, shl);
        CASE(NGP_SHR, shr);
        CASE(NGP_ASR, asr);
        CASE(NGP_ROR, ror);
        CASE(NGP_ABS, abs);
    default:
        break;
    }
#undef CASE
}

static FORCE_INLINE void non_binary(CPUCore& core, u32 inst)
{
    u16 nonopc = (inst >> 6) & 0x3FF;
    u8 op = (inst >> 16) & 0x3F;
    u8 src1 = (inst >> 22) & 0x1F;
    u8 src2 = (inst >> 27) & 0x1F;

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

static FORCE_INLINE void ld_pc(CPUCore& core, u32 inst)
{
    u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.list[dest] = (dest != ZeroRegister) * Bus::read_word(core, core.pc + disp);
}

static FORCE_INLINE void ld_s_pc(CPUCore& core, u32 inst) {}
static FORCE_INLINE void ld_d_pc(CPUCore& core, u32 inst) {}
static FORCE_INLINE void ld_q_pc(CPUCore& core, u32 inst) {}

static FORCE_INLINE void adr_pc(CPUCore& core, u32 inst)
{
    u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.list[dest] = (dest != ZeroRegister) * (core.pc + disp);
}

static FORCE_INLINE void immediate(CPUCore& core, u32 inst)
{
    u16 imm = inst >> 16;
    u8 immopc = (inst >> 6) & 0x1F;
    u8 reg = (inst >> 11) & 0x1F;

    if (immopc == NGP_MOVT_IMMEDIATE)
    {
        core.list[reg] |= (imm << 16);
    }
}

MAKE_IMMEDIATE_OP(add_immediate, core.list[src] + imm);

static FORCE_INLINE void adds_immediate(CPUCore& core, u32 inst)
{
    u8 dest = (inst >> 6) & 0x1F;
    u8 src = (inst >> 11) & 0x1F;
    u16 imm = inst >> 16;
    u32 result = add_with_carry_setting_flags(core, core.list[src], imm, 0);
    core.list[dest] = dest == ZeroRegister ? result : 0;
}

MAKE_IMMEDIATE_OP(sub_immediate, core.list[src] - imm);

static FORCE_INLINE void subs_immediate(CPUCore& core, u32 inst)
{
    u8 dest = (inst >> 6) & 0x1F;
    u8 src = (inst >> 11) & 0x1F;
    u16 imm = inst >> 16;
    u32 result = add_with_carry_setting_flags(core, core.list[src], ~imm, 1);
    core.list[dest] = dest == ZeroRegister ? 0 : result;
}

MAKE_IMMEDIATE_OP(and_immediate, core.list[src] & imm);

static FORCE_INLINE void ands_immediate(CPUCore& core, u32 inst)
{
    u8 dest = (inst >> 6) & 0x1F;
    u8 src = (inst >> 11) & 0x1F;
    u16 imm = inst >> 16;
    u32 result = and_setting_flags(core, core.list[src], imm);
    core.list[dest] = dest == ZeroRegister ? result : 0;
}

MAKE_IMMEDIATE_OP(or_immediate, core.list[src] | imm);
MAKE_IMMEDIATE_OP(eor_immediate, core.list[src] ^ imm);

static FORCE_INLINE void tbz(CPUCore& core, u32 inst) {}
static FORCE_INLINE void tbnz(CPUCore& core, u32 inst) {}
static FORCE_INLINE void cbz(CPUCore& core, u32 inst) {}
static FORCE_INLINE void cbnz(CPUCore& core, u32 inst) {}


void CPUCore::initialize()
{
    mem_pc = (Word*)Bus::get_physical_addr(0);
}

void CPUCore::shutdown() {}

void CPUCore::handle_pc_change()
{
}

Word CPUCore::fetch_inst()
{
    return mem_pc[(pc >> 2)];
}


void CPUCore::dispatch(u64 num_cycles)
{
    // One cycle for fetching, decoding and simple arithmetic
    cycles_in_second += num_cycles;
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
        inst_counter++;
#endif
    }
    cycles_in_second -= num_cycles;
}

void CPUCore::print_registers()
{
    printf(
        "R0  = %08X R1  = %08X R2  = %08X R3  = %08X R4  = %08X\n"
        "R5  = %08X R6  = %08X R7  = %08X R8  = %08X R9  = %08X\n"
        "R10 = %08X R11 = %08X R12 = %08X R13 = %08X R14 = %08X\n"
        "R15 = %08X R16 = %08X R17 = %08X R18 = %08X R19 = %08X\n"
        "R20 = %08X R21 = %08X R22 = %08X R23 = %08X R24 = %08X\n"
        "R25 = %08X R26 = %08X R27 = %08X R28 = %08X\n"
        "SP  = %08X LR  = %08X PC  = %08X\n",
        gpr.r0, gpr.r1, gpr.r2, gpr.r3, gpr.r4,
        gpr.r5, gpr.r6, gpr.r7, gpr.r8, gpr.r9,
        gpr.r10, gpr.r11, gpr.r12, gpr.r13, gpr.r14,
        gpr.r15, gpr.r16, gpr.r17, gpr.r18, gpr.r19,
        gpr.r20, gpr.r21, gpr.r22, gpr.r23, gpr.r24,
        gpr.r25, gpr.r26, gpr.r27, gpr.r28,
        gpr.sp, gpr.lr, pc
    );
}

void CPUCore::make_exception(ExceptionCode code, VirtualAddress vec_offset, u16 comment)
{
    switch (code)
    {
    case DivideByZeroException:
        break;
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
        if (Bus::check_virtual_address(vba, Bus::WriteableAddress) == Bus::ValidVirtualAddress
            && Bus::check_virtual_address(vba + VBASize, Bus::WriteableAddress) == Bus::ValidVirtualAddress)
        {
            pc = Bus::read_word(*this, vba + vec_offset);
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
    break;
    }
}

void CPUCore::return_exception()
{
    VirtualAddress target_pc = elr.elr_el[psr.CURRENT_EL];
    ProgramStateRegister last_psr = spsr.spsr[psr.CURRENT_EL];

    psr = last_psr;
    pc = target_pc;
    handle_pc_change();
}

void CPUCore::handle_breakpoint(u16 comment)
{
    
}

