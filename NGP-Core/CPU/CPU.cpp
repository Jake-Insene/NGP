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

static constexpr void set_flags(u32 src1, u32 src2, u64 res) {
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
    set_flags(src1, src2, res);

    return (u32)res;
}

extern "C" u32 compare_add(u32 src1, u32 src2) {
    CPU::registers.cycle_counter++; // addition
    u64 res = (u64)src1 + (u64)src2;
    CPU::registers.cycle_counter++; // setting flags
    set_flags(src1, src2, res);
    return (u32)res;
}

extern "C" u32 compare_and(u32 src1, u32 src2) {
    CPU::registers.cycle_counter++; // bitwise and
    u64 res = (u64)src1 & (u64)src2;
    CPU::registers.cycle_counter++; // setting flags
    set_flags(src1, src2, res);
    return (u32)res;
}

extern "C" void InterpreterMain(u32 inst, u32* registers);

// Type:
// 0 - Word
// 1 - Half
// 2 - SHalf
// 3 - Byte
// 4 - SByte
// 5 - DWord
// 6 - QWord
// 7 - Single
// 8 - Double

extern "C" void memory_read(u32 dest, u32 address, u32 type) {
    CPU::registers.cycle_counter++; // address calculation
    void* ptr = MemoryBus::get_real_address(address, true);
    CPU::registers.cycle_counter++; // read memory
    if (!ptr) {
        // Emit a exeception if you can't read the address
    }
}

extern "C" void memory_write(u32 source, u32 address, u32 type) {
    CPU::registers.cycle_counter++; // address calculation
    void* ptr = MemoryBus::get_real_address(address, false);
    CPU::registers.cycle_counter++; // write memory
    if (!ptr) {
    // Emit a exeception if you can't write the address
    }
}

extern "C" void memory_read_pair(u32 dest1, u32 dest2, u32 address, u32 type) {
    CPU::registers.cycle_counter++; // address calculation
    void* ptr = MemoryBus::get_real_address(address, true);
    CPU::registers.cycle_counter++; // read memory
    if (!ptr) {
        // Emit a exeception if you can't read the address
    }
}

extern "C" void memory_write_pair(u32 source1, u32 source2, u32 address, u32 type) {
    CPU::registers.cycle_counter++; // address calculation
    void* ptr = MemoryBus::get_real_address(address, false);
    CPU::registers.cycle_counter++; // write memory
    if (!ptr) {
        // Emit a exeception if you can't write the address
    }
}

i64 start;
i64 frequency;
void CPU::initialize() {
    start = Time::get_counter();
    frequency = Time::get_timer_frequency();
}

void CPU::shutdown() {}

i32 CPU::load(const char* room_path)
{
    std::ifstream file{ room_path, std::ios::binary | std::ios::ate };
    if (!file.is_open()) {
        return INVALID_ARGUMENTS;
    }

    u32 size = (u32)file.tellg();
    file.seekg(0);
    if (size < sizeof(RomHeader)) {
        file.close();
        return CORRUPT;
    }

    u8* rom = new u8[size];
    file.read((char*)rom, size);
    MemoryBus::emplace_rom(rom, size);
    delete[] rom;

    header = (RomHeader*)MemoryBus::rom_address();

    if (header->magic != RomSignature || header->check_sum != size) {
        file.close();
        return CORRUPT;
    }
    file.close();
    
    CPU::registers.pc = RomBaseAddress + (header->address_of_entry_point << 2);
    return STATUS_OK;
}

void CPU::dispatch() {
    if (CPU::registers.halt == 0) {
#if PROFILE == 1
        f64 start_tmp = Time::get_time();
#endif
        MemoryBus::read_word(CPU::registers.pc, registers.ir);
        CPU::registers.pc += 4;

        InterpreterMain(CPU::registers.ir, CPU::registers.list);
#if PROFILE == 1
        f64 elapsed = Time::get_time() - start_tmp;
        printf("Executing instruction, Cycles: %d, tooks %fs\n", CPU::registers.cycle_counter, elapsed);
#endif
    }

}

void CPU::delay_for_timing() {
    f64 elapsed = f64(Time::get_counter() - start) / frequency;
    f64 target_time = f64(CPU::registers.cycle_counter) / ClockSpeed;
    f64 to_wait = target_time - elapsed;

    if (to_wait > 0.0) {
        OS::sleep(i32(to_wait * 1000));
    }
}

