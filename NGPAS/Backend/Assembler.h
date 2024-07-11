#pragma once
#include "Frontend/PreProcessor.h"
#include "Backend/AssemblerUtility.h"
#include <FileFormat/ISA.h>
#include <FileFormat/Rom.h>

#define MAKE_ERROR(TOKEN, BREAKER, ...) \
    ErrorManager::error(TOKEN.source_file, TOKEN.line, __VA_ARGS__);\
    current_status = ERROR;\
    BREAKER;

enum class ParsePrecedence {
    None,
    Assignment, // =
    Equality, // == !=
    Comparision, // < > <= >=
    Term, // + -
    Factor, // * /
    Unar, // ! -
    Primary,
};

struct Assembler {
    i32 assemble_file(const char* file_path, const char* output_path, u32 origin);

    // First face: search labels
    void phase1();
    void assemble_label();
    void assemble_global_label();

    // Second fase: assembly
    void phase2();
    
    void assemble_directive();
    void assemble_instruction();

    // assemble_instruction();
    void assemble_load_store(u32& inst, u8 imm_opcode, u8 index_opc, u8 alignment, bool handle_symbol);

    void assemble_binary(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit, bool is_additional_opc, bool use_amount);
    
    void assemble_comparision(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit);

    void assemble_two_operands(u32& inst, u32(*fn)(u8, u8, u8));

    void assemble_one_operand(u32& inst, u32(*)(u8, u8));
    
    void assemble_shift(u32& inst, u8 opcode);

    void check_for_amount(u8& adder, u8& amount);

    void advance();
    void syncronize();
    bool expected(TokenType tk, const char* format, ...);
    void goto_next_line();

    // Utility
    u8 get_register(Token tk);
    u32& new_word();
    u16& new_half();
    u8& new_byte();
    u8* reserve(u32 count);

    // Parser
    Token parse_expresion(ParsePrecedence precedence);

    // expresions

    std::unordered_map<std::string_view, Label>::iterator find_in_scope(SourceScope& scope, 
        const TokenView& label, bool& founded);

    PreProcessor pre_processor;

    Token* last;
    Token* current;
    Token* next;

    std::vector<u8> program;

    Label entry_point;
    u32 entry_point_address;
    u32 origin_address;
    i32 current_status;
};
