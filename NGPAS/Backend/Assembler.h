/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/PreProcessor.h"
#include "Backend/AssemblerUtility.h"
#include "FileFormat/ISA.h"
#include "FileFormat/Rom.h"

#define MAKE_ERROR(TOKEN, BREAKER, ...) \
    ErrorManager::error(TOKEN.source_file, TOKEN.line, __VA_ARGS__);\
    BREAKER;

enum class ParsePrecedence {
    None,
    Start,
    BitwiseOr,
    BitwiseXor,
    BitwiseAnd,
    Equality, // == !=
    Comparision, // < > <= >=
    Shift, // >> <<
    Term, // + -
    Factor, // * /
    Unary, // ! -
    Primary,
};

struct Symbol {
    std::string symbol;

    const char* source_file;
    u32 line;
    bool isDefined;

    union {
        u32 address;
        u64 uvalue;
        i64 ivalue;
        f32 svalue;
        f64 dvalue;
    };
};

struct InstructionToResolve {
    // index to change
    u32 index;
    u32 address;
};

struct Assembler {
    bool assemble_file(const char* file_path, const char* output_path);

    // First phase: search labels and constants
    void phase1();
    Symbol& make_label(const Token& label, u64 address, const char* source_file, u32 line);
    Symbol& make_symbol(const Token& label, u64 value, const char* source_file, u32 line);

    // Second phase: assembly
    void phase2();

    void assemble_directive();
    void assembleInstruction();

    // assemble_instruction();
    void assemble_load_store(u32& inst, u8 imm_opcode, u8 index_opc, u8 alignment, bool handle_symbol);

    void assemble_binary(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit, bool is_additional_opc, bool use_amount);

    void assemble_comparision(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit);

    void assemble_three_operands(u32& inst, u32(*fn)(u8, u8, u8, u8));

    void assemble_two_operands(u32& inst, u32(*fn)(u8, u8, u8));

    void assemble_one_operand(u32& inst, u32(*)(u8, u8));

    void assemble_shift(u32& inst, u8 opcode);

    void check_for_amount(u8& adder, u8& amount);

    // Third phase: resolving instructions
    void resolve_instructions();

    void advance();
    void syncronize();
    bool expected(TokenType tk, const char* format, ...);
    void goto_next_line();
    void advance_to_next_line();

    // Utility
    u8 get_register(Token tk);
    u32& new_word();
    u16& new_half();
    u8& new_byte();
    u8* reserve(u32 count);
    void check_capacity(u32 count);

    // Parser

    // Prefix
    Token parse_minus(Token, Token);
    Token parse_not(Token, Token);
    Token parse_immediate(Token, Token);
    Token parse_symbol(Token, Token);
    Token parse_register(Token, Token);
    Token parse_group(Token, Token);
    Token parse_dollar(Token, Token);
    Token parse_string(Token, Token);

    // Infix
    Token parse_add(Token lsh, Token rhs);
    Token parse_sub(Token lsh, Token rhs);
    Token parse_and(Token lsh, Token rhs);
    Token parse_or(Token lsh, Token rhs);
    Token parse_xor(Token lsh, Token rhs);
    Token parse_shl(Token lsh, Token rhs);
    Token parse_shr(Token lsh, Token rhs);
    Token parse_asr(Token lsh, Token rhs);
    Token parse_mul(Token lsh, Token rhs);
    Token parse_div(Token lsh, Token rhs);

    Token parse_expresion(ParsePrecedence precedence);

    // expresions

    std::unordered_map<std::string, Symbol>::iterator find_label(const std::string_view label);

    PreProcessor pre_processor;

    Token* last;
    Token* current;
    Token* next;
    u32 token_index;

    struct {
        bool undefined_label;
        bool unknown_label;
        bool is_in_resolve;
    } context;

    std::vector<u8> program;
    u32 program_index;

    TokenDirective file_format;
    std::string_view extension;
    std::string_view last_label;
    std::unordered_map<std::string, Symbol> symbols;
    std::vector<InstructionToResolve> to_resolve;

    u32 origin_address;
    u32 last_size;
};
