// --------------------
// CPU.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "Core/Constants.h"
#include "CPU/CPU.h"
#include "Memory/MemoryBus.h"
#include "FileFormat/ISA.h"
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

extern "C" u32 compareAdd(u32 src1, u32 src2) {
    CPU::registers.cycle_counter++; // addition
    u64 res = (u64)src1 + (u64)src2;
    CPU::registers.cycle_counter++; // setting flags
    setFlags(src1, src2, res);
    return (u32)res;
}

extern "C" u32 compareAnd(u32 src1, u32 src2) {
    CPU::registers.cycle_counter++; // bitwise and
    u64 res = (u64)src1 & (u64)src2;
    CPU::registers.cycle_counter++; // setting flags
    setFlags(src1, src2, res);
    return (u32)res;
}

extern "C" void interpreterMain(u32 inst, u32* registers);

// Type:
// 0 - Word
// 1 - Half
// 2 - SHalf
// 3 - Byte
// 4 - SByte
// 5 - QWord
// 6 - Single
// 7 - Double

extern "C" void memoryRead(u32 dest, u32 address, u32 type) {
    CPU::registers.cycle_counter++; // address calculation
    void* ptr = MemoryBus::getAddress(address, true);
    CPU::registers.cycle_counter++; // memory fetch
    CPU::registers.cycle_counter++; // read memory
    if (!ptr) {
        // Emit a exeception if you can't read the address
    }

    union {
        u32* ulist = (u32*)&CPU::registers;
        i32* ilist;
    };


    switch (type) {
    case 0:
        ulist[dest] = *(u32*)ptr;
        break;
    case 1:
        ulist[dest] = *(u16*)ptr;
        break;
    case 2:
        ilist[dest] = *(i16*)ptr;
        break;
    case 3:
        ulist[dest] = *(u8*)ptr;
        break;
    case 4:
        ilist[dest] = *(i8*)ptr;
        break;
    case 5:
        CPU::simd.qfp[dest] = *(QWord*)ptr;
        break;
    case 6:
        CPU::simd.ncsfp[dest << 2] = *(u32*)ptr;
        break;
    case 7:
        CPU::simd.ncdfp[dest << 1] = *(u64*)ptr;
        break;
    }
}

extern "C" void memoryWrite(u32 source, u32 address, u32 type) {
    CPU::registers.cycle_counter++; // address calculation
    void* ptr = MemoryBus::getAddress(address, false);
    CPU::registers.cycle_counter++; // write memory
    if (!ptr) {
        // Emit a exeception if you can't write the address
    }

    union {
        u32* ulist = (u32*)&CPU::registers;
        i32* ilist;
    };


    switch (type) {
    case 0:
        *(u32*)ptr = ulist[source];
        break;
    case 1:
        *(u16*)ptr = (u16)ulist[source];
        break;
    case 2:
        *(i16*)ptr = (i16)ilist[source];
        break;
    case 3:
        *(u8*)ptr = (u8)ulist[source];
        break;
    case 4:
        *(i8*)ptr = (i8)ilist[source];
        break;
    case 5:
        *(QWord*)ptr = CPU::simd.qfp[source];
        break;
    case 6:
        *(u32*)ptr = CPU::simd.ncsfp[source << 2];
        break;
    case 7:
        *(u64*)ptr = CPU::simd.ncdfp[source << 1];
        break;
    }
}

extern "C" void memoryReadPair(u32 dest1, u32 dest2, u32 address, u32 type) {
    CPU::registers.cycle_counter++; // address calculation
    void* ptr = MemoryBus::getAddress(address, true);
    CPU::registers.cycle_counter++; // memory fetch
    CPU::registers.cycle_counter++; // read memory
    if (!ptr) {
        // Emit a exeception if you can't read the address
    }

    union {
        u32* ulist = (u32*)&CPU::registers;
        i32* ilist;
    };

    switch (type) {
    case 0:
        ulist[dest1] = *((u32*)ptr);
        ulist[dest2] = *((u32*)ptr + 1);
        break;
    case 5:
        CPU::simd.qfp[dest1] = *((QWord*)ptr);
        CPU::simd.qfp[dest2] = *((QWord*)ptr + 1);
        break;
    case 6:
        CPU::simd.ncsfp[dest1<<2] = *((u32*)ptr);
        CPU::simd.ncsfp[dest2<<2] = *((u32*)ptr + 1);
        break;
    case 7:
        CPU::simd.ncdfp[dest1 << 1] = *((u64*)ptr);
        CPU::simd.ncdfp[dest2 << 1] = *((u64*)ptr + 1);
        break;
    }
}

extern "C" void memoryWritePair(u32 source1, u32 source2, u32 address, u32 type) {
    CPU::registers.cycle_counter++; // address calculation
    void* ptr = MemoryBus::getAddress(address, false);
    CPU::registers.cycle_counter++; // write memory
    if (!ptr) {
        // Emit a exeception if you can't write the address
    }

    union {
        u32* ulist = (u32*)&CPU::registers;
        i32* ilist;
    };

    switch (type) {
    case 0:
        *((u32*)ptr) = ulist[source1];
        *((u32*)ptr + 1) = ulist[source2];
        break;
    case 5:
        *((QWord*)ptr) = CPU::simd.qfp[source1];
        *((QWord*)ptr + 1) = CPU::simd.qfp[source2];
        break;
    case 6:
        *((u32*)ptr) = CPU::simd.ncsfp[source1];
        *((u32*)ptr + 1) = CPU::simd.ncsfp[source2];
        break;
    case 7:
        *((u64*)ptr) = CPU::simd.ncdfp[source1];
        *((u64*)ptr + 1) = CPU::simd.ncdfp[source2];
        break;
    }
}

i64 start;
i64 frequency;
void CPU::initialize() {
    start = Time::getCounter();
    frequency = Time::getTimerFrequency();
}

void CPU::shutdown() {}

void CPU::dispatch() {
    while (CPU::registers.cycle_counter < CyclesPerFrame) {
        if (CPU::registers.psr.halt) {
            break;
        }
     
        if (MemoryBus::readWord(CPU::registers.pc, registers.ir)) {
            CPU::registers.pc += 4;

            interpreterMain(CPU::registers.ir, CPU::registers.list);
        }
    }

}

void CPU::delayForTiming() {
    f64 elapsed = f64(Time::getCounter() - start) / frequency;
    f64 target_time = f64(CPU::registers.cycle_counter) / CyclesPerSecond;
    f64 to_wait = target_time - elapsed;

    if (to_wait > 0.0) {
        OS::sleep(i32(to_wait * 1000));
    }
}

void CPU::printRegisters() {
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

