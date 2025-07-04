/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "CPU/NGPV1/NGPV1.h"

#include "FileFormat/ISA.h"
#include "Memory/Bus.h"
#include "Emulator.h"

#include <cstdio>
#include <bit>
#include <algorithm>

#undef OVERFLOW


#define MAKE_SIMPLE_ARITH_LOGIC(NAME, OP) \
    static FORCE_INLINE void NAME(NGPV1& core, const u8 dest, const u8 src1, const u8 src2)\
    {\
        core.list[dest] = (dest != CPUCore::ZeroRegister) * (OP);\
    }

#define MAKE_SETTING_FLAGS(NAME, OP) \
    static FORCE_INLINE void NAME(NGPV1& core, const u8 dest, const u8 src1, const u8 src2)\
    {\
        core.list[dest] = (dest != CPUCore::ZeroRegister) * (OP);\
    }

#define MAKE_IMMEDIATE_OP(NAME, OP) \
    static FORCE_INLINE void NAME(NGPV1& core, const u32 inst)\
    {\
        const u8 dest = (inst >> 6) & 0x1F;\
        const u8 src = (inst >> 11) & 0x1F;\
        const u16 imm = inst >> 16;\
        core.list[dest] = (dest != CPUCore::ZeroRegister) * OP;\
    }

#define HANDLE_BRANCH(COND, CORE, DISP) if(COND)\
    {\
        CORE.pc += DISP;\
        CORE.handle_pc_change();\
    }

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
static FORCE_INLINE void set_flags(NGPV1& core, const u32 src1, const u32 src2, const u64 res, const bool is_sub)
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
static FORCE_INLINE u32 add_with_carry_setting_flags(NGPV1& core, const u32 src1, const u32 src2, const u32 carry)
{
    const u64 lhs = src1;
    const u64 rhs = src2;
    const u64 res = lhs + rhs + carry;
    set_flags(core, src1, src2, res, false);

    return u32(res);
}

static u32 FORCE_INLINE and_setting_flags(NGPV1& core, const u32 src1, const u32 src2)
{
    u32 res = src1 & src2;
    core.psr.NEGATIVE = (res & 0x8000'0000) != 0;
    core.psr.ZERO = res == 0;

    return res;
}

static FORCE_INLINE u32 add_with_carry(const u32 src1, const u32 src2, const u32 carry)
{
    u64 res = u64(src1) + u64(src2) + carry;
    return u32(res);
}

MAKE_SIMPLE_ARITH_LOGIC(_add, core.list[src1] + core.list[src2]);
MAKE_SIMPLE_ARITH_LOGIC(_adc, add_with_carry(core.list[src1], core.list[src2], core.psr.CARRY));
MAKE_SIMPLE_ARITH_LOGIC(_sub, core.list[src1] - core.list[src2]);
MAKE_SIMPLE_ARITH_LOGIC(_sbc, add_with_carry(core.list[src1], ~core.list[src2], core.psr.CARRY));
MAKE_SIMPLE_ARITH_LOGIC(_and, core.list[src1] & core.list[src2]);
MAKE_SIMPLE_ARITH_LOGIC(_or, core.list[src1] | core.list[src2]);
MAKE_SIMPLE_ARITH_LOGIC(_orn, core.list[src1] | ~core.list[src2]);
MAKE_SIMPLE_ARITH_LOGIC(_eor, core.list[src1] ^ core.list[src2]);
MAKE_SETTING_FLAGS(_adds, add_with_carry_setting_flags(core, core.list[src1], core.list[src2], 0));
MAKE_SETTING_FLAGS(_subs, add_with_carry_setting_flags(core, core.list[src1], ~core.list[src2], 1));
MAKE_SETTING_FLAGS(_ands, and_setting_flags(core, core.list[src1], core.list[src2]));
MAKE_SIMPLE_ARITH_LOGIC(_bic, core.list[src1] & ~core.list[src2]);
MAKE_SETTING_FLAGS(_bics, and_setting_flags(core, core.list[src1], ~core.list[src2]));
MAKE_SIMPLE_ARITH_LOGIC(_adcs, add_with_carry_setting_flags(core, core.list[src1], core.list[src2], core.psr.CARRY));
MAKE_SIMPLE_ARITH_LOGIC(_sbcs, add_with_carry_setting_flags(core, core.list[src1], ~core.list[src2], core.psr.CARRY));


// Memory
static FORCE_INLINE void memory_read_single(NGPV1& core, const u8 dest, const VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, w, Bus::read_word, address);
}

static FORCE_INLINE void memory_read_double(NGPV1& core, const u8 dest, const VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, dw, Bus::read_dword, address);
}

static FORCE_INLINE void memory_read_v(NGPV1& core, const u8 dest, const VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest, qw, Bus::read_qword, address);
}

static FORCE_INLINE void memory_write_single(NGPV1& core, const u8 src, const VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, w, Bus::write_word, address);
}

static FORCE_INLINE void memory_write_double(NGPV1& core, const u8 src, const VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, dw, Bus::write_dword, address);
}

static FORCE_INLINE void memory_write_v(NGPV1& core, const u8 src, const VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src, qw, Bus::write_qword, address);
}

static FORCE_INLINE void memory_pair_read_word(NGPV1& core, const u8 dest1, const u8 dest2, const VirtualAddress address)
{
    HANDLE_READ(core, dest1, Bus::read_word, address);
    HANDLE_READ(core, dest2, Bus::read_word, address + 4);
}

static FORCE_INLINE void memory_pair_read_single(NGPV1& core, const u8 dest1, const u8 dest2, const VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, w, Bus::read_word, address);
    HANDLE_SIMD_READ(core, dest2, w, Bus::read_word, address + 4);
}

static FORCE_INLINE void memory_pair_read_double(NGPV1& core, const u8 dest1, const u8 dest2, const VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, dw, Bus::read_dword, address);
    HANDLE_SIMD_READ(core, dest2, dw, Bus::read_dword, address + 8);
}

static FORCE_INLINE void memory_pair_read_v(NGPV1& core, const u8 dest1, const u8 dest2, const VirtualAddress address)
{
    HANDLE_SIMD_READ(core, dest1, qw, Bus::read_qword, address);
    HANDLE_SIMD_READ(core, dest2, qw, Bus::read_qword, address + 16);
}

static FORCE_INLINE void memory_pair_write_word(NGPV1& core, const u8 src1, const u8 src2, const VirtualAddress address)
{
    HANDLE_WRITE(core, src1, Word, Bus::write_word, address);
    HANDLE_WRITE(core, src2, Word, Bus::write_word, address + 4);
}

static FORCE_INLINE void memory_pair_write_single(NGPV1& core, const u8 src1, const u8 src2, const VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, w, Bus::write_word, address);
    HANDLE_SIMD_WRITE(core, src2, w, Bus::write_word, address + 4);
}

static FORCE_INLINE void memory_pair_write_double(NGPV1& core, const u8 src1, const u8 src2, const VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, dw, Bus::write_dword, address);
    HANDLE_SIMD_WRITE(core, src2, dw, Bus::write_dword, address + 8);
}

static FORCE_INLINE void memory_pair_write_v(NGPV1& core, const u8 src1, const u8 src2, const VirtualAddress address)
{
    HANDLE_SIMD_WRITE(core, src1, qw, Bus::write_qword, address);
    HANDLE_SIMD_WRITE(core, src2, qw, Bus::write_qword, address + 16);
}

// ExtendedAlu
static FORCE_INLINE void madd(NGPV1& core, const u8 dest, const u8 src1, const u8 src2, const u8 src3)
{
    core.list[dest] = (dest != CPUCore::ZeroRegister) *
        (core.list[src3] + (core.list[src1] * core.list[src2]));
}

static FORCE_INLINE void msub(NGPV1& core, const u8 dest, const u8 src1, const u8 src2, const u8 src3)
{
    core.list[dest] = (dest != CPUCore::ZeroRegister) *
        (core.list[src3] - (core.list[src1] * core.list[src2]));
}

static FORCE_INLINE void udiv(NGPV1& core, const u8 dest, const u8 src1, const u8 src2, const u8)
{
    if (core.list[src2] == 0)
    {
        core.make_exception(NGPV1::DivideByZeroException, 0, 0);
        return;
    }

    core.list[dest] = (dest != CPUCore::ZeroRegister) * (core.list[src1] / core.list[src2]);
}

static FORCE_INLINE void div(NGPV1& core, const u8 dest, const u8 src1, const u8 src2, const u8)
{
    if (core.list[src2] == 0)
    {
        core.make_exception(NGPV1::DivideByZeroException, 0, 0);
        return;
    }

    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * (core.ilist[src1] / core.ilist[src2]);
}

// Src2 is a immediate value
static FORCE_INLINE void _shl(NGPV1& core, const u8 dest, const u8 src1, const u8 src2)
{
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * (core.ilist[src1] << src2);
}

// Src2 is a immediate value
static FORCE_INLINE void _shr(NGPV1& core, const u8 dest, const u8 src1, const u8 src2)
{
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * (core.ilist[src1] >> src2);
}

// Src2 is a immediate value
static FORCE_INLINE void _asr(NGPV1& core, const u8 dest, const u8 src1, const u8 src2)
{
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) * (core.ilist[src1] >> src2);
}

// Src2 is a immediate value
static FORCE_INLINE void _ror(NGPV1& core, const u8 dest, const u8 src1, const u8 src2)
{
    core.list[dest] = (dest != CPUCore::ZeroRegister) * (std::rotr(core.list[src1], (i32)src2));
}

static FORCE_INLINE void _abs(NGPV1& core, const u8 dest, const u8 src1, const u8)
{
    core.ilist[dest] = (dest != CPUCore::ZeroRegister) *
        (core.ilist[src1] >= 0 ? core.ilist[src1] : -core.ilist[src1]);
}

// Non Binary
static FORCE_INLINE void ret(NGPV1& core, const u8 src, const u8, const u32)
{
    core.pc = core.list[src];
    core.handle_pc_change();
}

static FORCE_INLINE void br(NGPV1& core, const u8 src, const u8, const u32)
{
    core.pc = core.list[src];
    core.handle_pc_change();
}

static FORCE_INLINE void blr(NGPV1& core, const u8 src, const u8, const u32)
{
    core.gpr.lr = core.pc;
    core.pc = core.list[src];
    core.handle_pc_change();
}

static FORCE_INLINE void brk(NGPV1& core, const u8 src1, const u8 src2, const u32 op)
{
    core.handle_breakpoint(src1 | src2 << 5 | op << 10);
}

static FORCE_INLINE void svc(NGPV1& core, const u8 src1, const u8 src2, const u32 op)
{
    core.make_exception(NGPV1::SupervisorException, NGPV1::ExceptionVBOffset, src1 | src2 << 5 | op << 10);
}

static FORCE_INLINE void evc(NGPV1& core, const u8 src1, const u8 src2, const u32 op)
{
    core.make_exception(NGPV1::ExtendedSupervisorException, NGPV1::ExceptionVBOffset, src1 | src2 << 5 | op << 10);
}

static FORCE_INLINE void smc(NGPV1& core, const u8 src1, const u8 src2, const u32 op)
{
    core.make_exception(NGPV1::SecureMachineControllerException, NGPV1::ExceptionVBOffset, src1 | src2 << 5 | op << 10);
}

static FORCE_INLINE void eret(NGPV1& core, const u8, const u8, const u32) { core.return_exception(); }
static FORCE_INLINE void wfi(NGPV1& core, const u8, const u8, const u32) {}

static FORCE_INLINE void msr(NGPV1& core, const u8 src, const u8 src2, const u32 op)
{
    const NGPSystemRegister sr = NGPSystemRegister(src2 | op << 5);
    switch (sr)
    {
    case NGP_PSTATE:
        core.psr.ZERO = bool(core.list[src] & 0x1);
        core.psr.CARRY = bool(core.list[src] & 0x2);
        core.psr.NEGATIVE = bool(core.list[src] & 0x4);
        core.psr.OVERFLOW = bool(core.list[src] & 0x8);
        break;
    case NGP_SPSR_EL1:
    case NGP_SPSR_EL2:
    case NGP_SPSR_EL3:
        core.system_regs.spsr.spsr[sr - NGP_SPSR_EL1].raw = core.list[src];
        break;
    case NGP_EDR_EL1:
    case NGP_EDR_EL2:
    case NGP_EDR_EL3:
        core.system_regs.edr.edr_el[sr - NGP_EDR_EL1] = core.list[src];
        break;
    case NGP_ELR_EL1:
    case NGP_ELR_EL2:
    case NGP_ELR_EL3:
        core.system_regs.elr.elr_el[sr - NGP_ELR_EL1] = core.list[src];
        break;
    case NGP_VBAR_EL1:
    case NGP_VBAR_EL2:
    case NGP_VBAR_EL3:
        core.system_regs.vbar.vbar_el[sr - NGP_VBAR_EL1] = core.list[src];
        break;
    case NGP_FAR_EL1:
    case NGP_FAR_EL2:
    case NGP_FAR_EL3:
        core.system_regs.far.far_el[sr - NGP_FAR_EL1] = core.list[src];
        break;
    default:
        break;
    }
}

static FORCE_INLINE void mrs(NGPV1& core, const u8 dest, const u8 src2, const u32 op)
{
    const NGPSystemRegister sr = NGPSystemRegister(src2 | op << 5);
    switch (sr)
    {
    case NGP_PSTATE:
        core.list[dest] = (dest != ZeroRegister) * core.psr.raw & 0xF;
        break;
    case NGP_CURRENT_EL:
        core.list[dest] = (dest != ZeroRegister) * core.psr.CURRENT_EL;
        break;
    case NGP_SPSR_EL1:
    case NGP_SPSR_EL2:
    case NGP_SPSR_EL3:
        core.list[dest] = (dest != ZeroRegister) * core.system_regs.spsr.spsr[sr - NGP_SPSR_EL1].raw;
        break;
    case NGP_EDR_EL1:
    case NGP_EDR_EL2:
    case NGP_EDR_EL3:
        core.list[dest] = (dest != ZeroRegister) * core.system_regs.edr.edr_el[sr - NGP_EDR_EL1];
        break;
    case NGP_ELR_EL1:
    case NGP_ELR_EL2:
    case NGP_ELR_EL3:
        core.list[dest] = (dest != ZeroRegister) * core.system_regs.elr.elr_el[sr - NGP_ELR_EL1];
        break;
    case NGP_VBAR_EL1:
    case NGP_VBAR_EL2:
    case NGP_VBAR_EL3:
        core.list[dest] = (dest != ZeroRegister) * core.system_regs.vbar.vbar_el[sr - NGP_VBAR_EL1];
        break;
    case NGP_FAR_EL1:
    case NGP_FAR_EL2:
    case NGP_FAR_EL3:
        core.list[dest] = (dest != ZeroRegister) * core.system_regs.far.far_el[sr - NGP_FAR_EL1];
        break;
    }
}

static FORCE_INLINE void hlt(NGPV1& core, const u8, const u8, const u32) { core.psr.HALT = true; }

// Main opcodes
static FORCE_INLINE void bl(NGPV1& core, const u32 inst)
{
    const u32 disp_inst = inst >> 6;
    const u32 disp = inst & 0x8000000 ? (0xFC00'0000 | disp_inst) << 2 : disp_inst << 2;

    core.gpr.lr = core.pc;
    HANDLE_BRANCH(true, core, disp);
}

static FORCE_INLINE void b(NGPV1& core, const u32 inst)
{
    const u32 disp_inst = inst >> 6;
    const u32 disp = inst & 0x8000000 ? (0xFC00'0000 | disp_inst) << 2 : disp_inst << 2;
    HANDLE_BRANCH(true, core, disp);
}

static FORCE_INLINE void bcond(NGPV1& core, const u32 inst)
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

static FORCE_INLINE void _3op(NGPV1& core, const u32 inst)
{
    const u16 _alu_opc = (inst >> 6) & 0x7FF;
    const u8 dest_src = (inst >> 17) & 0x1F;
    const u8 src1_base = (inst >> 22) & 0x1F;
    const u8 src2_index = (inst >> 27) & 0x1F;

#define CASE_R(op, func) case op: \
    HANDLE_READ(core, dest_src, func, core.list[src1_base] + core.list[src2_index]); break
#define CASE_R_SIGNED(op, func) case op: \
    HANDLE_READ_SIGNED(core, dest_src, func, core.list[src1_base] + core.list[src2_index]); break
#define CASE_SIMD_R(op, field, func) case op: \
    HANDLE_SIMD_READ(core, dest_src, field, func, core.list[src1_base] + core.list[src2_index]); break

#define CASE_W(op, type, func) case op: \
    HANDLE_WRITE(core, dest_src, type, func, core.list[src1_base] + core.list[src2_index]); break
#define CASE_SIMD_W(op, field, func) case op: \
    HANDLE_SIMD_WRITE(core, dest_src, field, func, core.list[src1_base] + core.list[src2_index]); break

#define CASE(op, name) case op: name(core, dest_src, src1_base, core.list[src2_index] & 0x1F); break
#define CASE_IMM_SHIFT(op, name) case op: name(core, dest_src, src1_base, src2_index); break
    switch (_alu_opc)
    {
        CASE(NGP_ADD, _add);
        CASE(NGP_ADC, _adc);
        CASE(NGP_SUB, _sub);
        CASE(NGP_SBC, _sbc);
        CASE(NGP_AND, _and);
        CASE(NGP_OR, _or);
        CASE(NGP_ORN, _orn);
        CASE(NGP_EOR, _eor);
        CASE(NGP_ADDS, _adds);
        CASE(NGP_SUBS, _subs);
        CASE(NGP_ANDS, _ands);
        CASE(NGP_BIC, _bic);
        CASE(NGP_BICS, _bics);
        CASE(NGP_ADCS, _adcs);
        CASE(NGP_SBCS, _sbcs);
        CASE(NGP_SHL, _shl);
        CASE(NGP_SHR, _shr);
        CASE(NGP_ASR, _asr);
        CASE(NGP_ROR, _ror);
        CASE_IMM_SHIFT(NGP_SHL_IMM, _shl);
        CASE_IMM_SHIFT(NGP_SHR_IMM, _shr);
        CASE_IMM_SHIFT(NGP_ASR_IMM, _asr);
        CASE_IMM_SHIFT(NGP_ROR_IMM, _ror);
        CASE(NGP_ABS, _abs);
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
    default:
        break;
    }
#undef CASE_R
#undef CASE_R_SIGNED
#undef CASE_SIMD_R
#undef CASE_W
#undef CASE_SIMD_W
#undef CASE
}

static FORCE_INLINE void fp_op(NGPV1& core, const u32 inst)
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
            core.make_exception(NGPV1::DivideByZeroException, NGPV1::ExceptionVBOffset, NGPV1::CommentNone);
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
            core.make_exception(NGPV1::DivideByZeroException, NGPV1::ExceptionVBOffset, NGPV1::CommentNone);
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
            core.make_exception(NGPV1::DivideByZeroException, NGPV1::ExceptionVBOffset, NGPV1::CommentNone);
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
            core.make_exception(NGPV1::DivideByZeroException, NGPV1::ExceptionVBOffset, NGPV1::CommentNone);
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

static FORCE_INLINE void load_store_immediate(NGPV1& core, const u32 inst)
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

static FORCE_INLINE void load_store_fp_immediate(NGPV1& core, const u32 inst)
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

static FORCE_INLINE void load_store_register(NGPV1& core, const u32 inst)
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

static FORCE_INLINE void load_store_pair(NGPV1& core, const u32 inst) {}

static FORCE_INLINE void extended_alu(NGPV1& core, const u32 inst)
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

static FORCE_INLINE void non_binary(NGPV1& core, const u32 inst)
{
    const u16 non_opc = (inst >> 6) & 0x3F;
    const u8 dest_src = (inst >> 12) & 0x1F;
    const u8 src2 = (inst >> 17) & 0x1F;
    const u32 op = (inst >> 22) & 0x3FF;

#define CASE(_op, name) case _op: name(core, dest_src, src2, op); break
    switch (non_opc)
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

static FORCE_INLINE void ld_pc(NGPV1& core, const u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.list[dest] = (dest != CPUCore::ZeroRegister) * Bus::read_word(core.pc + disp);
}

static FORCE_INLINE void ld_s_pc(NGPV1& core, const u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.simd[dest].w = Bus::read_word(core.pc + disp);
}

static FORCE_INLINE void ld_d_pc(NGPV1& core, const u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.simd[dest].dw = Bus::read_dword(core.pc + disp);
}

static FORCE_INLINE void ld_v_pc(NGPV1& core, const u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.simd[dest].qw = Bus::read_qword(core.pc + disp);
}

static FORCE_INLINE void adr_pc(NGPV1& core, const u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u32 disp_inst = inst >> 11;
    const u32 disp = inst & 0x8000'0000 ? (0xFFE0'0000 | disp_inst) << 2 : disp_inst << 2;
    core.list[dest] = (dest != CPUCore::ZeroRegister) * (core.pc + disp);
}

static FORCE_INLINE void immediate(NGPV1& core, const u32 inst)
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

static FORCE_INLINE void adds_immediate(NGPV1& core, const u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u8 src = (inst >> 11) & 0x1F;
    const u16 imm = inst >> 16;
    const u32 result = add_with_carry_setting_flags(core, core.list[src], imm, 0);
    core.list[dest] = dest == CPUCore::ZeroRegister ? result : 0;
}

MAKE_IMMEDIATE_OP(sub_immediate, core.list[src] - imm);

static FORCE_INLINE void subs_immediate(NGPV1& core, const u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u8 src = (inst >> 11) & 0x1F;
    const u16 imm = inst >> 16;
    const u32 result = add_with_carry_setting_flags(core, core.list[src], ~imm, 1);
    core.list[dest] = dest == CPUCore::ZeroRegister ? 0 : result;
}

MAKE_IMMEDIATE_OP(and_immediate, core.list[src] & imm);

static FORCE_INLINE void ands_immediate(NGPV1& core, const u32 inst)
{
    const u8 dest = (inst >> 6) & 0x1F;
    const u8 src = (inst >> 11) & 0x1F;
    const u16 imm = inst >> 16;
    const u32 result = and_setting_flags(core, core.list[src], imm);
    core.list[dest] = dest == CPUCore::ZeroRegister ? result : 0;
}

MAKE_IMMEDIATE_OP(or_immediate, core.list[src] | imm);
MAKE_IMMEDIATE_OP(eor_immediate, core.list[src] ^ imm);

static FORCE_INLINE void tbz(NGPV1& core, const u32 inst) {}
static FORCE_INLINE void tbnz(NGPV1& core, const u32 inst) {}
static FORCE_INLINE void cbz(NGPV1& core, const u32 inst) {}
static FORCE_INLINE void cbnz(NGPV1& core, const u32 inst) {}

static FORCE_INLINE void fp_4op(NGPV1& core, const u32 inst)
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
    pc = 0;
    handle_pc_change();
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

void NGPV1::set_psr(ProgramStateRegister new_psr)
{
    psr = new_psr;
}

CPUCore::ProgramStateRegister NGPV1::get_psr()
{
    return psr;
}

void NGPV1::set_pc(VirtualAddress new_pc)
{
    pc = new_pc & 0xFFFF'FFFC;
    handle_pc_change();
}

VirtualAddress NGPV1::get_pc()
{
    return pc;
}

void NGPV1::external_handle_exception(ExceptionCode code, ExceptionComment comment, VirtualAddress addr)
{
    switch (code)
    {
    case SupervisorException:
    case ExtendedSupervisorException:
    case SecureMachineControllerException:
    case BreakpointException:
    case AccessViolationException:
    case DivideByZeroException:
    case BadSystemRegAlignment:
    case BadPCAlignment:
    case BadMemAlignment:
        break;
    }
}

usize NGPV1::run(usize num_cycles)
{
    // One cycle for fetching, decoding and simple arithmetic
    while (num_cycles && !psr.HALT)
    {
        const Word inst = fetch_next_inst();

        pc += 4;
        pc_page_offset += 1;

        const u8 opcode = inst & 0x3F;
#define CASE(op, name) case op: name(*this, inst); break
        switch (opcode)
        {
            CASE(NGP_BL, bl);
            CASE(NGP_B, b);
            CASE(NGP_B_COND, bcond);
            CASE(NGP_3OP, _3op);
            CASE(NGP_FP_OP, fp_op);
            CASE(NGP_LOAD_STORE_IMMEDIATE, load_store_immediate);
            CASE(NGP_LOAD_STORE_FP_IMMEDIATE, load_store_fp_immediate);
            CASE(0x7, [](NGPV1& core, const u32) {});
            CASE(NGP_LOAD_STORE_PAIR, load_store_pair);
            CASE(NGP_EXTENDEDALU, extended_alu);
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
        num_cycles--;
    }

    return num_cycles;
}

void NGPV1::handle_pc_change()
{
    if (pc & 0x3)
    {
        make_exception(NGPV1::BadPCAlignment, NGPV1::ExceptionVBOffset, NGPV1::CommentNone);
        return;
    }

    const Bus::Page& pc_page = Bus::get_page(pc);
    if (pc_page.access & Bus::PageExecute) [[likely]]
    {
        pc_page_index = pc_page.page_index;
        pc_page_addr = (Word*)pc_page.physical_address;
        pc_page_offset = Bus::get_page_offset(pc) >> 2;
        _mm_prefetch((char*)pc_page_addr, _MM_HINT_NTA);
        return;
    }

    pc_page_addr = nullptr;
    make_exception(NGPV1::AccessViolationException, NGPV1::ExceptionVBOffset, NGPV1::CantExecute);
    return;
}

Word NGPV1::fetch_next_inst()
{
    const Word page_index = Bus::get_page_index(pc);
    if (pc_page_index == page_index) [[likely]]
        return pc_page_addr[pc_page_offset];

    handle_pc_change();
    if (pc_page_addr) [[likely]]
        return pc_page_addr[pc_page_offset];
    
    return 0;
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
        system_regs.elr.elr_el[target_exception_level] = pc;
        system_regs.spsr.spsr[target_exception_level] = psr;

        // Minimum target_exception_level is EL1
        psr.CURRENT_EL = target_exception_level + 1;
        system_regs.edr.edr_el[target_exception_level] = comment;

        VirtualAddress vba = system_regs.vbar.vbar_el[target_exception_level];
        if (vba & 0xF)
        {
            make_exception(NGPV1::BadSystemRegAlignment, ExceptionVBOffset, CommentNone);
        }

        if (Bus::check_virtual_address(vba, Bus::WriteableAddress) == Bus::ValidAddress)
        {
            pc = vba + vec_offset;
            handle_pc_change();
        }
        else
        {
            hlt(*this, 0, 0, 0);
        }
    }
    break;
    case BreakpointException:
    case AccessViolationException:
    case DivideByZeroException:
    case BadSystemRegAlignment:
    case BadPCAlignment:
    case BadMemAlignment:
        break;
    default:
        break;
    }
}

void NGPV1::return_exception()
{
    VirtualAddress target_pc; 
    ProgramStateRegister last_psr;
    target_pc = system_regs.elr.elr_el[psr.CURRENT_EL - 1];
    last_psr = system_regs.spsr.spsr[psr.CURRENT_EL - 1];

    psr = last_psr;
    pc = target_pc;
    handle_pc_change();
}

void NGPV1::handle_breakpoint(u16 comment)
{
    
}

