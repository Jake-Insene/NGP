#pragma once
#include "Frontend/PreProcessor.h"
#include "Backend/AssemblerUtility.h"
#include <FileFormat/ISA.h>
#include <FileFormat/Rom.h>

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

    union {
        u32 address;
        u64 uvalue;
        i64 ivalue;
        f32 svalue;
        f64 dvalue;
    };
};

struct Assembler {
    bool assembleFile(const char* file_path, const char* output_path, u32 origin);

    // First face: search labels and constants
    void phase1();
    void makeLabel(std::string_view label, u32 address, const char* source_file, u32 line);

    // Second fase: assembly
    void phase2();

    void assembleDirective();
    void assembleInstruction();

    // assemble_instruction();
    void assembleLoadStore(u32& inst, u8 imm_opcode, u8 index_opc, u8 alignment, bool handle_symbol);

    void assembleBinary(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit, bool is_additional_opc, bool use_amount);

    void assembleComparision(u32& inst, u8 opc, u8 opc_imm, u16 immediate_limit);

    void assembleTwoOperands(u32& inst, u32(*fn)(u8, u8, u8));

    void assembleOneOperand(u32& inst, u32(*)(u8, u8));

    void assembleShift(u32& inst, u8 opcode);

    void checkForAmount(u8& adder, u8& amount);

    void advance();
    void syncronize();
    bool expected(TokenType tk, const char* format, ...);
    void gotoNextLine();

    // Utility
    u8 getRegister(Token tk);
    u32& newWord();
    u16& newHalf();
    u8& newByte();
    u8* reserve(u32 count);

    // Parser

    // Prefix
    Token parseMinus(Token, Token);
    Token parseNot(Token, Token);
    Token parseInteger(Token, Token);
    Token parseSingle(Token, Token);
    Token parseDouble(Token, Token);
    Token parseSymbol(Token, Token);
    Token parseRegister(Token, Token);

    // Infix
    Token parseAdd(Token lsh, Token rhs);
    Token parseSub(Token lsh, Token rhs);
    Token parseAnd(Token lsh, Token rhs);
    Token parseOr(Token lsh, Token rhs);
    Token parseXor(Token lsh, Token rhs);
    Token parseShl(Token lsh, Token rhs);
    Token parseShr(Token lsh, Token rhs);
    Token parseAsr(Token lsh, Token rhs);
    Token parseMul(Token lsh, Token rhs);
    Token parseDiv(Token lsh, Token rhs);

    Token parseExpresion(ParsePrecedence precedence);

    // expresions

    std::unordered_map<std::string, Symbol>::iterator findLabel(std::string_view label, bool& founded);

    PreProcessor pre_processor;

    Token* last;
    Token* current;
    Token* next;
    u32 token_index;

    std::vector<u8> program;
    std::string_view last_label;
    std::unordered_map<std::string, Symbol> symbols;

    Symbol entry_point;
    u32 entry_point_address;
    u32 origin_address;
    u32 last_size;
};
