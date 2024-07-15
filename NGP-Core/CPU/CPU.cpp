#include "Core/Constants.h"
#include "CPU/CPU.h"
#include "Memory/MemoryBus.h"
#include "FileFormat/ISA.h"
#include "Platform/OS.h"
#include "Platform/Time.h"
#include <fstream>

#define PROFILE 1

#define get_flag(f) bool(CPU::registers.flags & (1 << (f)))
#define set_flag(f, v) (CPU::registers.flags |= (v ? 1 << (f) : 0 ))

static inline void setFlags(u32 src1, u32 src2, u64 res) {
    CPU::registers.flags = 0;

    // Set S flag (sign flag)
    set_flag(CPU::NEGATIVE_FLAG, res & 0x8000'0000);

    // Set Z flag (zero flag)
    set_flag(CPU::ZERO_FLAG, res == 0);

    // Set C flag (carry flag)
    set_flag(CPU::CARRY_FLAG, res > 0xFFFF'FFFF);

    // Set V flag (overflow flag)
    set_flag(CPU::OVERFLOW_FLAG, (~((u64)src1 ^ (u64)src2) & ((u64)src1 ^ res)) & 0x8000'0000);
}

extern "C" u32 compare(u32 src1, u32 src2) {
    CPU::registers.cycle_counter++; // subtraction
    u64 res = (u64)src1 - (u64)src2;
    
    CPU::registers.cycle_counter++; // setting flags
    setFlags(src1, src2, res);

    return (u32)res;
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

bool CPU::load(const char* room_path)
{
    std::ifstream file{ room_path, std::ios::binary | std::ios::ate };
    if (!file.is_open()) {
        return false;
    }

    u32 size = (u32)file.tellg();
    file.seekg(0);
    if (size < sizeof(RomHeader)) {
        file.close();
        return false;
    }

    u8* rom = new u8[size];
    file.read((char*)rom, size);
    MemoryBus::emplaceRom(rom, size);
    delete[] rom;

    header = (RomHeader*)MemoryBus::romAddress();

    if (header->magic != RomSignature || header->check_sum != size) {
        file.close();
        return false;
    }
    file.close();
    
    CPU::registers.pc = header->address_of_entry_point << 2;
    return true;
}

void CPU::dispatch() {
    if (CPU::registers.halt == 0) {
#if PROFILE == 1
        f64 start_tmp = Time::getTime();
#endif
        MemoryBus::readWord(CPU::registers.pc, registers.ir);
        CPU::registers.pc += 4;

        interpreterMain(CPU::registers.ir, &CPU::registers.r0);
#if PROFILE == 1
        f64 elapsed = Time::getTime() - start_tmp;
        printf("Executing instruction, Cycles: %d, tooks %fs\n", CPU::registers.cycle_counter, elapsed);
#endif
    }

}

void CPU::delayForTiming() {
    f64 elapsed = f64(Time::getCounter() - start) / frequency;
    f64 target_time = f64(CPU::registers.cycle_counter) / ClockSpeed;
    f64 to_wait = target_time - elapsed;

    if (to_wait > 0.0) {
        OS::sleep(i32(to_wait * 1000));
    }
}

