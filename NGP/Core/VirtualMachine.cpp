#include "Core/VirtualMachine.h"
#include "Core/SystemCalls.h"
#include "FileFormat/ISA.h"
#include <fstream>

VirtualMachine::VirtualMachine() :
    header(), display(800, 600), sp(), data(), pc()
{
    sp = new u8[0xFF]{};
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

    data = new u8[header.size_of_raw_data]{};
    file.read((char*)data, header.size_of_raw_data);

    file.close();
    return STATUS_OK;
}

void VirtualMachine::start()
{
    pc = header.address_of_entry_point * 4;

    while (true) {
        display.update();

        u32 inst = *(u32*)(data + pc);
        pc += 4;

        u8 opcode = inst & 0x3F;
        switch (inst & 0x3F)
        {
        case NGP_CALL:
            break;
        case NGP_BRANCH:
            break;
        case NGP_SC:
        {
            u32 code = (inst >> 6);
            system_calls[code](this);
        }
        break;
        case NGP_RET:
            break;
        case NGP_HALT:
            return;
        case NGP_BRANCH_COND:
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
                reg[dest] = reg[src1];
                break;
            case NGP_ADD:
                reg[dest] = reg[src1] + reg[src2];
                break;
            case NGP_SUB:
                reg[dest] = reg[src1] - reg[src2];
                break;
            case NGP_MUL:
                ireg[dest] = ireg[src1] * ireg[src2];
                break;
            case NGP_UMUL:
                reg[dest] = reg[src1] * reg[src2];
                break;
            case NGP_DIV:
                ireg[dest] = ireg[src1] / ireg[src2];
                break;
            case NGP_UDIV:
                reg[dest] = reg[src1] / reg[src2];
                break;
            case NGP_AND:
                reg[dest] = reg[src1] & reg[src2];
                break;
            case NGP_OR:
                reg[dest] = reg[src1] | reg[src2];
                break;
            case NGP_XOR:
                reg[dest] = reg[src1] ^ reg[src2];
                break;
            case NGP_SHL:
                reg[dest] = reg[src1] >> reg[src2];
                break;
            case NGP_SHR:
                reg[dest] = reg[src1] << reg[src2];
                break;
            default:
                printf("Invalid binary opcode %X", binopc);
                break;
            }
        }
        break;
        case NGP_ADR_PC:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u32 disp = signExt(u32(inst >> 11), 21, 32);
            reg[dest] = (pc + (disp * 4));
        }
        break;
        case NGP_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 immopc = (inst >> 11) & 0x1F;
            u16 imm = (inst >> 16);

            switch (immopc)
            {
            case NGP_MOV_IMMEDIATE:
                reg[dest] = imm;
                break;
            case NGP_MOVT_IMMEDIATE:
                reg[dest] |= (imm << 16);
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
            reg[dest] = reg[src1] + imm;
        }
            break;
        case NGP_SUB_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            reg[dest] = reg[src1] - imm;
        }
            break;
        case NGP_AND_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            reg[dest] = reg[src1] & imm;
        }
            break;
        case NGP_OR_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            reg[dest] = reg[src1] | imm;
        }
            break;
        case NGP_XOR_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            reg[dest] = reg[src1] ^ imm;
        }
            break;
        case NGP_SHL_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            reg[dest] = reg[src1] >> imm;
        }
            break;
        case NGP_SHR_IMMEDIATE:
        {
            u8 dest = u8((inst >> 6) & 0x1F);
            u8 src1 = u8((inst >> 11) & 0x1F);
            u16 imm = inst >> 16;
            reg[dest] = reg[src1] << imm;
        }
            break;
        default:
            printf("Invalid opcode %X", opcode);
            break;
        }
    }

}
