/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Core/Constants.h"
#include "CPU/CPU.h"
#include "Memory/MemoryBus.h"
#include "Platform/OS.h"
#include "Platform/Time.h"
#include <stdio.h>

#define PROFILE 1

static inline void setFlags(u32 src1, u32 src2, u64 res) {
    CPU::registers.psr = {};

    // Set N flag (negative flag)
    CPU::registers.psr.n = res & 0x8000'0000;

    // Set Z flag (zero flag)
    CPU::registers.psr.z = res ? false : true;

    // Set C flag (carry flag)
    CPU::registers.psr.c = res > 0xFFFF'FFFF;

    // Set V flag (overflow flag)
    CPU::registers.psr.v = (~((u64)src1 ^ (u64)src2) & ((u64)src1 ^ res)) & 0x8000'0000;
}

extern "C" u32 compare(u32 src1, u32 src2) {
    CPU::registers.cycle_counter++; // subtraction
    u64 res = u64(src1) - u64(src2);
    CPU::registers.cycle_counter++; // setting flags
    setFlags(src1, src2, res);
    return u32(res);
}

extern "C" u32 compare_add(u32 src1, u32 src2) {
    CPU::registers.cycle_counter++; // addition
    u64 res = (u64)src1 + (u64)src2;
    CPU::registers.cycle_counter++; // setting flags
    setFlags(src1, src2, res);
    return (u32)res;
}

extern "C" u32 compare_and(u32 src1, u32 src2) {
    CPU::registers.cycle_counter++; // bitwise and
    u64 res = (u64)src1 & (u64)src2;
    CPU::registers.cycle_counter++; // setting flags
    setFlags(src1, src2, res);
    return (u32)res;
}

extern "C" void interpreter_main(u32 inst, u32* registers);

// Type:
// 0 - Word
// 1 - Half
// 2 - SHalf
// 3 - Byte
// 4 - SByte
// 5 - QWord
// 6 - Single
// 7 - Double

extern "C" void memory_read(u32 dest, u32 address, u32 type) {
    // write back
    CPU::registers.cycle_counter++;

    switch (type) {
    case 0:
        // When we use the ZR write back dont happen
        if (dest == 0) {
            CPU::registers.cycle_counter--;
            return;
        }
        CPU::registers.list[dest] = MemoryBus::read_word(address);
        break;
    case 1:
        if (dest == 0) {
            CPU::registers.cycle_counter--;
            return;
        }
        CPU::registers.list[dest] = MemoryBus::read_half(address);
        break;
    case 2:
        if (dest == 0) {
            CPU::registers.cycle_counter--;
            return;
        }
        CPU::registers.ilist[dest] = static_cast<i16>(MemoryBus::read_half(address));
        break;
    case 3:
        if (dest == 0) {
            CPU::registers.cycle_counter--;
            return;
        }
        CPU::registers.list[dest] = MemoryBus::read_byte(address);
        break;
    case 4:
        if (dest == 0) {
            CPU::registers.cycle_counter--;
            return;
        }
        CPU::registers.ilist[dest] = static_cast<i8>(MemoryBus::read_byte(address));
        break;
    case 5:
        CPU::simd.qfp[dest] = MemoryBus::read_qword(address);
        break;
    case 6:
        CPU::simd.ncsfp[dest << 2] = MemoryBus::read_word(address);
        break;
    case 7:
        CPU::simd.ncdfp[dest << 1] = MemoryBus::read_dword(address);
        break;
    }
}

extern "C" void memory_write(u32 source, u32 address, u32 type) {
    // write back dont happend here
    switch (type) {
    case 0:
        MemoryBus::write_word(address, CPU::registers.list[source]);
        break;
    case 1:
        MemoryBus::write_half(address, static_cast<u16>(CPU::registers.list[source]));
        break;
    case 3:
        MemoryBus::write_byte(address, static_cast<u8>(CPU::registers.list[source]));
        break;
    case 5:
        MemoryBus::write_qword(address, CPU::simd.qfp[source]);
        break;
    case 6:
        MemoryBus::write_word(address, CPU::simd.ncsfp[source<<2]);
        break;
    case 7:
        MemoryBus::write_dword(address, CPU::simd.ncdfp[source<<1]);
        break;
    }
}

extern "C" void memory_read_pair(u32 dest1, u32 dest2, u32 address, u32 type) {
    // write back 2 registers
    CPU::registers.cycle_counter += 2;

    switch (type) {
    case 0:
        // When we use the ZR write back dont happen
        if (dest1 == 0) {
            CPU::registers.cycle_counter--;
        }
        else {
            CPU::registers.list[dest1] = MemoryBus::read_word(address);
        }

        if (dest2 == 0) {
            CPU::registers.cycle_counter--;
        }
        else {
            CPU::registers.list[dest2] = MemoryBus::read_word(address + 4);
        }

        break;
    case 5:
        CPU::simd.qfp[dest1] = MemoryBus::read_qword(address);
        CPU::simd.qfp[dest2] = MemoryBus::read_qword(address + 16);
        break;
    case 6:
        CPU::simd.ncsfp[dest1 << 2] = MemoryBus::read_word(address);
        CPU::simd.ncsfp[dest1 << 2] = MemoryBus::read_word(address + 4);
        break;
    case 7:
        CPU::simd.ncdfp[dest1 << 1] = MemoryBus::read_dword(address);
        CPU::simd.ncdfp[dest2 << 1] = MemoryBus::read_dword(address + 8);
        break;
    }
}

extern "C" void memory_write_pair(u32 source1, u32 source2, u32 address, u32 type) {
    // write back dont happend here
    switch (type) {
    case 0:
        MemoryBus::write_word(address, CPU::registers.list[source1]);
        MemoryBus::write_word(address + 4, CPU::registers.list[source2]);
        break;
    case 5:
        MemoryBus::write_qword(address, CPU::simd.qfp[source1]);
        MemoryBus::write_qword(address + 16, CPU::simd.qfp[source2]);
        break;
    case 6:
        MemoryBus::write_word(address, CPU::simd.ncsfp[source1 << 2]);
        MemoryBus::write_word(address, CPU::simd.ncsfp[source2<<2]);
        break;
    case 7:
        MemoryBus::write_dword(address, CPU::simd.ncdfp[source1 << 1]);
        MemoryBus::write_dword(address, CPU::simd.ncdfp[source2 << 1]);
        break;
    }
}

i64 start;
i64 frequency;
void CPU::initialize() {
    start = Time::get_counter();
    frequency = Time::get_timer_frequency();
}

void CPU::shutdown() {}

void CPU::dispatch() {
    while (CPU::registers.cycle_counter < CyclesPerFrame) {
        registers.ir = MemoryBus::read_word(CPU::registers.pc);
        
        if (CPU::registers.psr.halt) {
            break;
        }

        CPU::registers.pc += 4;


        // One cycle for fetching,
        // other for decoding and the last for execution
        CPU::registers.cycle_counter += 3;
        interpreter_main(CPU::registers.ir, CPU::registers.list);
        CPU::instruction_counter++;
    }

    CPU::registers.cycle_counter = 0;
}

void CPU::delay_for_timing() {
    f64 elapsed = f64(Time::get_counter() - start) / frequency;
    f64 target_time = f64(CPU::registers.cycle_counter) / CyclesPerFrame;
    f64 to_wait = target_time - elapsed;

    if (to_wait > 0.0) {
        OS::sleep(i32(to_wait * 1000));
    }
}

void CPU::print_pegisters() {
    printf(
        "R0  = %08X R1  = %08X R2  = %08X R3  = %08X  R4 = %08X\n"
        "R5  = %08X R6  = %08X R7  = %08X R8  = %08X  R9 = %08X\n"
        "R10 = %08X R11 = %08X R12 = %08X R13 = %08X R14 = %08X\n"
        "R15 = %08X R16 = %08X R17 = %08X R18 = %08X R19 = %08X\n"
        "R20 = %08X R21 = %08X R22 = %08X R23 = %08X R24 = %08X\n"
        "R25 = %08X R26 = %08X R27 = %08X R28 = %08X\n"
        "SP  = %08X LR  = %08X PC  = %08X\n",
        CPU::registers.gpr.r0, CPU::registers.gpr.r1, CPU::registers.gpr.r2, CPU::registers.gpr.r3, CPU::registers.gpr.r4,
        CPU::registers.gpr.r5, CPU::registers.gpr.r6, CPU::registers.gpr.r7, CPU::registers.gpr.r8, CPU::registers.gpr.r9,
        CPU::registers.gpr.r10, CPU::registers.gpr.r11, CPU::registers.gpr.r12, CPU::registers.gpr.r13, CPU::registers.gpr.r14,
        CPU::registers.gpr.r15, CPU::registers.gpr.r16, CPU::registers.gpr.r17, CPU::registers.gpr.r18, CPU::registers.gpr.r19,
        CPU::registers.gpr.r20, CPU::registers.gpr.r21, CPU::registers.gpr.r22, CPU::registers.gpr.r23, CPU::registers.gpr.r24,
        CPU::registers.gpr.r25, CPU::registers.gpr.r26, CPU::registers.gpr.r27, CPU::registers.gpr.r28,
        CPU::registers.gpr.sp, CPU::registers.gpr.lr, CPU::registers.pc
    );
}

