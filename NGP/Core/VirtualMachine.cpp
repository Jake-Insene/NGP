#include "Core/VirtualMachine.h"
#include "Core/MemoryBus.h"
#include "Core/Constants.h"
#include "FileFormat/ISA.h"

static union {
    u32 gpr[32] = {};
    i32 igpr[32];
};

struct QWord {
    u64 hi;
    u64 lo;
};

static union {
    QWord qfp[32] = {};
    f64 dfp[32*2];
    u64 ncdfp[32*2];
    f32 sfp[64*2];
    u32 ncsfp[64*2];
};

u32 flags = 0;
#define ZERO_FLAG 1
#define CARRY_FLAG 2
#define SIGN_FLAG 3
#define OVERFLOW_FLAG 4

#define get_flag(f) bool(flags & (1 << (f-1)))
#define set_flag(f, v) (flags |= (v ? 1 << (f-1) : 0 ))

u32* swi_table = nullptr;

extern "C" u32 compare(u32 src1, u32 src2) {
    union {
        u32 res;
        i32 ires;
    };

    res = src1 - src2;
    
    flags = 0;

    // Set S flag (sign flag)
    set_flag(SIGN_FLAG, res & 0x80000000);

    // Set Z flag (zero flag)
    set_flag(ZERO_FLAG, res == 0);

    // Set C flag (carry flag)
    set_flag(CARRY_FLAG, !(src1 < src2));

    // Set V flag (overflow flag)
    set_flag(OVERFLOW_FLAG, ((src1^ src2)& (src1^ res)) >> 31);

    return res;
}

extern "C" u32 compare_add(u32 src1, u32 src2) {
    union {
        u32 res;
        i32 ires;
    };

    res = src1 + src2;

    flags = 0;

    // Set S flag (sign flag)
    set_flag(SIGN_FLAG, res & 0x80000000);

    // Set Z flag (zero flag)
    set_flag(ZERO_FLAG, !(res | (-ires)));

    // Set C flag (carry flag)
    set_flag(CARRY_FLAG, !(src1 < src2));

    // Set V flag (overflow flag)
    set_flag(OVERFLOW_FLAG, ((src1 ^ src2) & (src1 ^ res)) >> 31);

    return res;
}

extern "C" u32 compare_and(u32 src1, u32 src2) {
    union {
        u32 res;
        i32 ires;
    };

    res = src1 & src2;

    flags = 0;

    // Set S flag (sign flag)
    set_flag(SIGN_FLAG, res & 0x80000000);

    // Set Z flag (zero flag)
    set_flag(ZERO_FLAG, !(res | (-ires)));

    // Set C flag (carry flag)
    set_flag(CARRY_FLAG, !(src1 < src2));

    // Set V flag (overflow flag)
    set_flag(OVERFLOW_FLAG, ((src1 ^ src2) & (src1 ^ res)) >> 31);

    return res;
}

extern "C" void InterpreterMain(u32 inst, u32* regs, u32* pc, bool* is_halt);

extern "C" void* get_real_address(u32 address) {
    return MemoryBus::get_real_address(address);
}

extern "C" u32 get_flags() {
    return flags;
}

VirtualMachine::VirtualMachine() :
    header(), display(DISPLAY_WIDTH, DISPLAY_HEIGHT), pc(), inst(), is_halt()
{
    gpr[SP_INDEX] = RAMEnd;
}

VirtualMachine::~VirtualMachine()
{
}

i32 VirtualMachine::load(const char* room_path)
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

    MemoryBus::emplace_rom(file, size);
    header = (RomHeader*)MemoryBus::rom_address();

    if (header->magic != RomSignature || header->check_sum != size) {
        file.close();
        return CORRUPT;
    }

    file.close();
    return STATUS_OK;
}

void VirtualMachine::start()
{
    pc = RomBaseAddress + (header->address_of_entry_point << 2);

    while (!is_halt) {
        display.update();

        MemoryBus::read_word(pc, inst);
        pc += 4;
        
        InterpreterMain(inst, gpr, &pc, &is_halt);
    }

}

void VirtualMachine::push(u32 word)
{
    gpr[SP_INDEX] -= 4;
    MemoryBus::write_word(gpr[SP_INDEX], word);
}

u32 VirtualMachine::pop()
{
    u32 val = 0;
    MemoryBus::read_word(gpr[SP_INDEX], val);
    gpr[SP_INDEX] += 4;
    return val;
}
