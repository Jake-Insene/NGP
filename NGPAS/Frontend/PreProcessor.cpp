// --------------------
// PreProcessor.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "Frontend/PreProcessor.h"
#include "Backend/AssemblerUtility.h"
#include "ErrorManager.h"
#include <fstream>  

void PreProcessor::process(const char* file_path) {
    sources.clear();

    std::ifstream input{ file_path, std::ios::binary | std::ios::ate };
    if (!input.is_open()) {
        printf("error: couldt'n load the file '%s'\n", file_path);
        exit(-1);
    }

    auto& source = sources.emplace_back();
    source.file_path = file_path;
    source.index= u32(sources.size() - 1);

    u32 size = (u32)input.tellg();
    input.seekg(0);
    source.source_code = new u8[size +1];
    source.source_len = size + 1;
    input.read((char*)source.source_code, size);
    source.source_code[size] = '\0';
    input.close();

    lexer.set(file_path, (u8*)source.source_code, source.source_len);

    advance();
    advance();

    processSource();
}

void PreProcessor::processSource() {
    while (current.isNot(TOKEN_END_OF_FILE)) {
        switch (current.type) {
        case TOKEN_DIRECTIVE:
            processDirective();
            break;
        default:
            tokens.emplace_back(current);
            advance();
            break;
        }
    }
}

void PreProcessor::processDirective() {
    advance();

    switch (last.subtype) {
    case TD_INCLUDE:
    {
        if (!expected(TOKEN_STRING, "a file path was expected")) {
            return;
        }

        Lexer last_lexer = lexer;
        Token last_current = current;
        Token last_next = current;

        std::string file_path{};
        file_path.resize(last.str.size());
        encodeString((u8*)file_path.data(), last.str);

        std::string source_folder = last.source_file;
        source_folder = source_folder.substr(0, source_folder.find_last_of('/'));
        file_path = source_folder + "/" + file_path;

        std::ifstream input{ file_path, std::ios::binary | std::ios::ate };
        if (!input.is_open()) {
            ErrorManager::error(
                last.source_file, last.line,
                "the file '%s' was not founded", file_path.c_str()
            );
            return;
        }

        auto& new_source = sources.emplace_back();
        new_source.index = u32(sources.size() - 1);
        new_source.file_path = file_path;

        u32 size = (u32)input.tellg();
        input.seekg(0);
        new_source.source_code = new u8[size + 1];
        new_source.source_len = size;

        input.read((char*)new_source.source_code, size);
        new_source.source_code[size] = '\0';
        input.close();

        lexer.set(
            new_source.file_path.c_str(), 
            new_source.source_code,
            new_source.source_len
        );

        advance();
        advance();

        processSource();

        lexer = last_lexer;
        current = last_current;
        next = last_next;
    }
        break;
    default:
        tokens.emplace_back(last);
        break;
    }

    
}

void PreProcessor::advance() {
    last = current;
    current = next;
    next = lexer.getNext();
}

bool PreProcessor::expected(TokenType type, const char* format, ...) {
    advance();
    if (last.type != type) {
        va_list args;
        va_start(args, format);
        ErrorManager::errorV(last.source_file, last.line, format, args);
        va_end(args);
        return false;
    }

    return true;
}
