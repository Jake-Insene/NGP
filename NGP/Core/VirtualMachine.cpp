#include "Core/VirtualMachine.h"
#include "Core/SystemCalls.h"
#include "FileFormat/ISA.h"
#include <fstream>

u8 RAM[MB(128)] = {};

static union {
    u32 gp[32] = {};
    i32 igp[32];
};

static union {
    f64 dfp[32] = {};
    u64 ncdfp[32];
    f32 sfp[64];
    u32 ncsfp[64];
};

u32 flags = 0;
#define ZERO_FLAG 0
#define CARRY_FLAG 1
#define SIGN_FLAG 2
#define OVERFLOW_FLAG 3

#define get_flag(f) bool(flags & 1 << f)
#define set_flag(f, v) (flags |= (v ? 1 << f : 0 ))

static constexpr void compare(u32 src1, u32 src2, u8 shift) {
    union {
        u32 res;
        i32 ires;
    };

    res = src1 - (src2 << shift);
    
    flags = 0;

    // Set S flag (sign flag)
    set_flag(SIGN_FLAG, res & 0x80000000);

    // Set Z flag (zero flag)
    set_flag(ZERO_FLAG, !(res | (-res)));

    // Set C flag (carry flag)
    set_flag(CARRY_FLAG, !(src1 < src2));

    // Set V flag (overflow flag)
    set_flag(OVERFLOW_FLAG, ((src1^ src2)& (src1^ res)) >> 31);
}

VirtualMachine::VirtualMachine() :
    header(), display(DISPLAY_WIDTH, DISPLAY_HEIGHT), ram(RAM), pc()
{
    ureg = gp;
    sreg = sfp;
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
    if (size < sizeof(RoomHeader)) {
        file.close();
        return CORRUPT;
    }

    file.read((char*)&header, sizeof(RoomHeader));

    if (header.magic != MAGIC_NUMBER) {
        file.close();
        return CORRUPT;
    }

    if ((size - sizeof(RoomHeader)) < header.size_of_raw_data) {
        file.close();
        return CORRUPT;
    }

    file.read((char*)RAM, header.size_of_raw_data);

    file.close();
    return STATUS_OK;
}

void VirtualMachine::start()
{
    pc = header.address_of_entry_point * 4;

    while (true) {
        display.update();

        u32 inst = *(u32*)(RAM + pc);
        pc += 4;

        u8 opcode = inst & 0x3F;
        switch (inst & 0x3F)
        {
        case NGP_CALL:
        {
            push(pc);
            u32 disp = sign_ext((inst >> 6), 26, 32);
            pc += (disp * 4);
        }
        break;
        case NGP_BRANCH:
        {
            u32 disp = sign_ext((inst >> 6), 26, 32);
            pc += (disp * 4);
        }
        break;
        case NGP_SC:
        {
            u32 code = (inst >> 6);
            system_calls[code](this);
        }
        break;
        case NGP_RET:
            pc = pop();
            break;
        case NGP_HALT:
            return;
        case NGP_BRANCH_COND:
        {
            u8 cond = (inst >> 6) & 0xF;
            u32 disp = sign_ext((inst >> 10) & 0x3FFFFF, 22, 32) * 4;

            switch (cond)
            {
            case NGP_BEQ:
                if (get_flag(ZERO_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BNE:
                if (!get_flag(ZERO_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BLT:
                if (get_flag(SIGN_FLAG) != get_flag(OVERFLOW_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BLE:
                if (get_flag(ZERO_FLAG) || get_flag(SIGN_FLAG) != get_flag(OVERFLOW_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BGT:
                if (get_flag(ZERO_FLAG) == 0 && get_flag(SIGN_FLAG) == get_flag(OVERFLOW_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BGE:
                if ((get_flag(SIGN_FLAG) == get_flag(OVERFLOW_FLAG))) {
                    pc += disp;
                }
                break;
            case NGP_BCS:
                if (get_flag(CARRY_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BNC:
                if (!get_flag(CARRY_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BSS:
                if (get_flag(SIGN_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BNS:
                if (!get_flag(SIGN_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BOS:
                if (get_flag(OVERFLOW_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BNO:
                if (!get_flag(OVERFLOW_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BHI:
                if (get_flag(CARRY_FLAG) && !get_flag(ZERO_FLAG)) {
                    pc += disp;
                }
                break;
            case NGP_BLS:
                if (!get_flag(CARRY_FLAG) || get_flag(ZERO_FLAG)) {
                    pc += disp;
                }
                break;
            default:
                printf("Invalid branch cond opcode %X", cond);
                break;
            }
        }
        break;
        case NGP_BINARY:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u8 src2 = u8((inst >> 16) & 0x1F);
            u16 binopc = u8((inst >> 21) & 0x7FF);

            switch (binopc)
            {
            case NGP_MOV:
                gp[dest] = gp[src1] << src2;
                break;
            case NGP_ADD:
                gp[dest] = gp[src1] + gp[src2];
                break;
            case NGP_SUB:
                gp[dest] = gp[src1] - gp[src2];
                break;
            case NGP_MUL:
                igp[dest] = igp[src1] * igp[src2];
                break;
            case NGP_UMUL:
                gp[dest] = gp[src1] * gp[src2];
                break;
            case NGP_DIV:
                igp[dest] = igp[src1] / igp[src2];
                break;
            case NGP_UDIV:
                gp[dest] = gp[src1] / gp[src2];
                break;
            case NGP_AND:
                gp[dest] = gp[src1] & gp[src2];
                break;
            case NGP_OR:
                gp[dest] = gp[src1] | gp[src2];
                break;
            case NGP_XOR:
                gp[dest] = gp[src1] ^ gp[src2];
                break;
            case NGP_SHL:
                gp[dest] = gp[src1] << gp[src2];
                break;
            case NGP_SHR:
                gp[dest] = gp[src1] >> gp[src2];
                break;
            case NGP_CMP:
            {
                compare(gp[dest], gp[src1], src2);
            }
            break;
            case NGP_LD:
                gp[dest] = *(u32*)RAM + (gp[src1] + gp[src2]);
                break;
            case NGP_LDSH:
                igp[dest] = *(i16*)RAM + (gp[src1] + gp[src2]);
                break;
            case NGP_LDH:
                gp[dest] = *(u16*)RAM + (gp[src1] + gp[src2]);
                break;
            case NGP_LDSB:
                igp[dest] = *(i8*)RAM + (gp[src1] + gp[src2]);
                break;
            case NGP_LDB:
                gp[dest] = *(u8*)RAM + (gp[src1] + gp[src2]);
                break;
            case NGP_ST:
                *(u32*)(RAM + (gp[src1] + gp[src2])) = gp[dest];
                break;
            case NGP_STH:
                *(u16*)(RAM + (gp[src1] + gp[src2])) = (u16)gp[dest];
                break;
            case NGP_STB:
                *(u8*)(RAM + (gp[src1] + gp[src2])) = (u8)gp[dest];
                break;
            case NGP_NOT:
            {
                i32 res = gp[src1] << src2;
                igp[dest] = ~res;
            }
            break;
            case NGP_NEG:
            {
                i32 res = gp[src1] << src2;
                igp[dest] = -res;
            }
            break;
            case NGP_ABS:
            {
                i32 res = igp[src1] << src2;
                if (res < 0) {
                    igp[dest] = -res;
                }
                else {
                    igp[dest] = res;
                }
            }
            break;
            default:
                printf("Invalid binary opcode %X", binopc);
                break;
            }
        }
        break;
        case NGP_FBINARY:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u8 src2 = u8((inst >> 16) & 0x1F);
            u16 fbinopc = u8((inst >> 21) & 0x7FF);

            switch (fbinopc)
            {
            case NGP_FMOV_S:
                sfp[dest * 2] = sfp[src1 << 1];
                break;
            case NGP_FMOV_NC_W_S:
                gp[dest] = *(u32*)sfp + (src1 << 1);
                break;
            case NGP_FCVTZS_S:
                igp[dest] = (i32)sfp[src1 << 1];
                break;
            case NGP_FCVTZU_S:
                gp[dest] = (u32)sfp[src1 << 1];
                break;
            case NGP_SCVTF_S:
                sfp[dest << 1] = (f32)igp[src1];
                break;
            case NGP_UCVTF_S:
                sfp[dest << 1] = (f32)gp[src1];
                break;
            case NGP_FADD_S:
                sfp[dest << 1] = sfp[src1 << 1] + sfp[src2 << 1];
                break;
            case NGP_FSUB_S:
                sfp[dest << 1] = sfp[src1 << 1] - sfp[src2 << 1];
                break;
            case NGP_FMUL_S:
                sfp[dest << 1] = sfp[src1 << 1] * sfp[src2 << 1];
                break;
            case NGP_FDIV_S:
                sfp[dest << 1] = sfp[src1 << 1] / sfp[src2 << 1];
                break;
            case NGP_LD_S:
                ncsfp[dest << 1] = *(u32*)(RAM + gp[src1] + gp[src2]);
                break;
            case NGP_ST_S:
                *(u32*)(RAM + (gp[src1] + gp[src2])) = ncsfp[dest << 1];
                break;
            }
        }
        break;
        case NGP_MEMORY_IMMEDIATE:
        {
            u8 dest_src = u8((inst >> 6) & 0x1F);
            u8 base = u8((inst >> 11) & 0x1F);
            u8 memopc = u8((inst >> 16) & 0x7);
            u8 sub = (inst >> 19) & 0x1;
            u16 offset = (inst >> 20);

            switch (memopc)
            {
            case NGP_LD_IMMEDIATE:
                offset <<= 2;
                gp[dest_src] = *(u32*)(RAM + (gp[base] + (sub ? -offset : offset)));
                break;
            case NGP_LDSH_IMMEDIATE:
                offset <<= 1;
                igp[dest_src] = *(i16*)(RAM + (gp[base] + (sub ? -offset : offset)));
                break;
            case NGP_LDH_IMMEDIATE:
                offset <<= 1;
                gp[dest_src] = *(u16*)(RAM + (gp[base] + (sub ? -offset : offset)));
                break;
            case NGP_LDSB_IMMEDIATE:
                igp[dest_src] = *(i8*)(RAM + (gp[base] + (sub ? -offset : offset)));
                break;
            case NGP_LDB_IMMEDIATE:
                gp[dest_src] = *(u8*)(RAM + (gp[base] + (sub ? -offset : offset)));
                break;
            case NGP_ST_IMMEDIATE:
                offset <<= 2;
                *(u32*)(RAM + (gp[base] + (sub ? -offset : offset))) = gp[dest_src];
                break;
            case NGP_STH_IMMEDIATE:
                offset <<= 1;
                *(u16*)(RAM + (gp[base] + (sub ? -offset : offset))) = (u16)gp[dest_src];
                break;
            case NGP_STB_IMMEDIATE:
                *(u8*)(RAM + (gp[base] + (sub ? -offset : offset))) = (u8)gp[dest_src];
                break;
            case NGP_LD_S_IMMEDIATE:
                break;
            case NGP_ST_S_IMMEDIATE:
                break;
            case NGP_LD_D_IMMEDIATE:
                break;
            case NGP_ST_D_IMMEDIATE:
                break;
            default:
                printf("Invalid memory immediate opcode %X", memopc);
                break;
            }
        }
        break;
        case NGP_LD_PC:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u32 disp = sign_ext(u32(inst >> 11), 21, 32);
            u32 address = pc + (disp * 4);
            gp[dest] = *(u32*)(RAM + address);
        }
        break;
        case NGP_LD_S_PC:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u32 disp = sign_ext(u32(inst >> 11), 21, 32);
            u32 address = pc + (disp * 4);
            ncsfp[dest << 1] = *(u32*)(RAM + address);
        }
        break;
        case NGP_LD_D_PC:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u32 disp = sign_ext(u32(inst >> 11), 21, 32);
            u32 address = pc + (disp * 4);
            ncdfp[dest] = *(u64*)(RAM + address);
        }
        break;
        case NGP_ADR_PC:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u32 disp = sign_ext(u32(inst >> 11), 21, 32);
            gp[dest] = (pc + (disp * 4));
        }
        break;
        case NGP_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 immopc = (inst >> 11) & 0x1F;
            u16 imm = (inst >> 16);

            switch (immopc)
            {
            case NGP_CMP_IMMEDIATE:
                compare(gp[dest], imm, 0);
                break;
            case NGP_MOV_IMMEDIATE:
                gp[dest] = imm;
                break;
            case NGP_MOVT_IMMEDIATE:
                gp[dest] |= (imm << 16);
                break;
            default:
                printf("Invalid immediate opcode %X", immopc);
                break;
            }
        }
        break;
        case NGP_ADD_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            gp[dest] = gp[src1] + imm;
        }
        break;
        case NGP_SUB_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            gp[dest] = gp[src1] - imm;
        }
        break;
        case NGP_AND_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            gp[dest] = gp[src1] & imm;
        }
        break;
        case NGP_OR_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            gp[dest] = gp[src1] | imm;
        }
        break;
        case NGP_XOR_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            gp[dest] = gp[src1] ^ imm;
        }
        break;
        case NGP_SHL_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            gp[dest] = gp[src1] << imm;
        }
        break;
        case NGP_SHR_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            gp[dest] = gp[src1] >> imm;
        }
        break;
        default:
            printf("Invalid opcode %X", opcode);
            break;
        }
    }

}

void VirtualMachine::push(u32 word)
{
    gp[SP_INDEX] += 4;
    *(u32*)(RAM + gp[SP_INDEX]) = word;
}

u32 VirtualMachine::pop()
{
    u32 val = *(u32*)(RAM + gp[SP_INDEX]);
    gp[SP_INDEX] -= 4;
    return val;
}
