/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/AsmPreProcessor.h"
#include "Backend/AssemblerUtility.h"
#include "FileFormat/ISA.h"
#include "StringPool.h"

#define MAKE_ERROR(TOKEN, BREAKER, ...) \
    {\
        ErrorManager::error(TOKEN.get_source_file().data(), TOKEN.line, __VA_ARGS__);\
        BREAKER;\
    }

#define ADD_RESOLVE(RESOLVE_TYPE, IDX, PIDX) \
    {\
        ToResolveItem& tr = to_resolve.emplace_back();\
        tr.type = RESOLVE_TYPE;\
        tr.address = PIDX;\
        tr.index = IDX;\
        advance_to_next_line();\
    }

#define HANDLE_NOT_DEFINED_VALUE(RESOLVE_TYPE, BREAKER, IDX, PIDX) \
    if (context.undefined_label == true && context.is_in_resolve == false)\
    {\
        ADD_RESOLVE(RESOLVE_TYPE, IDX, PIDX);\
        BREAKER;\
    }\
    else if (context.undefined_label == true && context.is_in_resolve == true)\
        BREAKER\

enum class ParsePrecedence
{
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

struct Symbol
{
    StringID symbol;

    StringID source_file;
    u32 line;
    bool is_defined;

    union
    {
        u32 address;
        u64 uvalue;
        i64 ivalue;
        f32 svalue;
        f64 dvalue;
    };
};

enum ResolveType
{
    ResolveInstruction,
    ResolveDirective,
};

struct ToResolveItem
{
    ResolveType type;
    // index to change
    u32 index;
    u32 address;
};

enum RegisterType
{
    RegisterGP,
    RegisterFP,
    RegisterVector,

    RegisterAny,
    RegisterFPOrVector,
};

struct Assembler
{
    bool assemble_file(const char* file_path, const char* output_path);

    // First phase: search labels and constants
    void phase1();
    Symbol& make_label(const Token& label, u64 address, StringID source_file, u32 line);
    Symbol& make_symbol(const Token& label, u64 value, StringID source_file, u32 line);

    // Second phase: assembly
    void phase2();

    void assemble_directive();
    void assemble_instruction();

    // assemble_instruction();
    void assemble_load_store(u32& inst, u8 imm_opcode, u8 index_opc, u8 alignment, bool handle_symbol);
    void assemble_binary(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit, bool is_additional_opc, bool use_amount);
    void assemble_fbinary(u32& inst, u8 s_opc, u8 d_opc, u8 v_s4_opc, u8 v_d2_opc);
    void assemble_comparison(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit);
    void assemble_three_operands(u32& inst, u32(*fn)(u8, u8, u8, u8));
    void assemble_fp_three_operands(u32& inst, u32(*fn)(u8, u8, u8, u8, FPType));
    void assemble_two_operands(u32& inst, u32(*fn)(u8, u8, u8));
    void assemble_one_operand(u32& inst, u32(*)(u8, u8));
    void assemble_shift(u32& inst, u8 opcode);
    void check_for_amount(u8& adder, u8& amount);

    // Third phase: resolving instructions
    void resolve_pending();

    void advance();
    void synchronize();
    bool expected(TokenType tk, const char* format, ...);
    bool expectedv(TokenType tk, const char* format, va_list va);
    bool expected_comma();
    bool expected_left_key();
    bool expected_right_key();
    void goto_next_line();
    void advance_to_next_line();

    // Utility
    u8 get_register(Token tk);
    bool try_get_register(u8& reg, RegisterType reg_type, const char* format, ...);
    bool try_get_register_tk(Token tk, u8& reg, RegisterType reg_type);
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

    Token parse_expression(ParsePrecedence precedence);

    // expressions

    std::unordered_map<StringID, Symbol>::iterator find_label(StringID label);

    AsmPreProcessor pre_processor;

    Token* last;
    Token* current;
    Token* next;
    u32 token_index;

    struct
    {
        bool undefined_label;
        bool unknown_label;
        bool is_in_resolve;
    } context;

    std::vector<u8> program;
    u32 program_index;

    TokenDirective file_format;
    std::string_view extension;
    StringID last_label;
    std::unordered_map<StringID, Symbol> symbols;
    std::vector<ToResolveItem> to_resolve;

    u32 origin_address;
    u32 last_size;
};
