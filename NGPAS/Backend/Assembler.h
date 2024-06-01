#pragma once
#include "Frontend/Lexer.h"
#include "Backend/AssemblerUtility.h"
#include <FileFormat/ISA.h>
#include <FileFormat/Room.h>
#include <unordered_map>
#include <map>

struct Constant {
    i32 i;
    u32 u;
    f32 f;
};

struct InstructionToResolve {
    u32 address;
    TokenInstruction type;
    std::string_view symbol;

    const char* source_file;
    u32 line;
    u32 column;

    [[nodiscard]] constexpr bool is_branch() const 
    {
        return
            type == TI_B ||
            type == TI_BEQ ||
            type == TI_BNE ||
            type == TI_BLT ||
            type == TI_BLE ||
            type == TI_BGT ||
            type == TI_BGE;
    }
};

struct Label {
    std::string_view symbol;
    u32 address;

    const char* source_file;
    u32 line;
    u32 column;
};

struct Assembler {
    Assembler() 
        : lexer(), last(), current(), next(),
        entry_point(), entry_point_address(0), current_status(0)
    {}
    ~Assembler() {}

    i32 assemble_file(const char* file_path, const char* output_path);

    // first fase
    void assemble_program();
    void assemble_directive();
    void assemble_label();
    void assemble_instruction();

    // assemble_instruction();
    bool assemble_load_store(u32& inst, u8 dest, u8 imm_opcode, 
        u16 index_opc, u8 alignment, bool is_fp, bool is_single);

    void encode_string(u8* mem, const std::string_view& str);

    // second fase
    void resolve_labels();

    void advance();
    void syncronize();
    bool expected(TokenType tk, const char* format, ...);
    void skip_whitespaces();

    // Utility
    u8 get_register(Token tk);
    u32& new_word();
    u8* reserve(u32 count);

    Lexer lexer;

    Token last;
    Token current;
    Token next;

    std::unordered_map<std::string_view, Constant> constants;
    std::unordered_map<std::string_view, Label> labels;
    std::vector<InstructionToResolve> to_resolve;
    std::vector<u8> program;

    Label entry_point;
    u32 entry_point_address;
    i32 current_status;
};
