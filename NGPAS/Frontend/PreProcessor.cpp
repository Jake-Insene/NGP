#include "Frontend/PreProcessor.h"
#include "Backend/AssemblerUtility.h"
#include "ErrorManager.h"
#include <fstream>  

void PreProcessor::process(const char* file_path) {
    sources.clear();

    std::ifstream input{ file_path, std::ios::binary | std::ios::ate };
    if (!input.is_open()) {
        printf("Error: couldt'n load the file '%s'\n", file_path);
        exit(-1);
    }

    sources.emplace_back();
    current_scope().file_path = file_path;
    current_scope().parent_scope_index = u32(-1);
    current_scope_index = u32(sources.size() - 1);
    global_scope_index = current_scope_index;

    u32 size = (u32)input.tellg();
    input.seekg(0);
    current_scope().source_code = new u8[size +1];
    current_scope().source_len = size + 1;
    input.read((char*)current_scope().source_code, size);
    current_scope().source_code[size] = '\0';
    input.close();

    lexer.set(file_path, (u8*)current_scope().source_code, (u32)current_scope().source_len);

    advance();
    advance();

    process_source();
}

void PreProcessor::process_source() {
    while (current.is_not(TOKEN_END_OF_FILE)) {
        switch (current.type) {
        case TOKEN_DIRECTIVE:
            process_directive();
            break;
        default:
            current_scope().tokens.emplace_back(current);
            advance();
            break;
        }
    }
}

void PreProcessor::process_directive() {
    advance();

    switch (last.subtype) {
    case TD_INCLUDE:
    {
        Token tk = last;

        if (!expected(TOKEN_STRING, "a file path was expected")) {
            return;
        }

        Lexer last_lexer = lexer;
        Token last_current = current;
        Token last_next = current;

        std::string file_path{};
        file_path.resize(last.str.len);
        encode_string((u8*)file_path.data(), last.str);

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

        sources.emplace_back();
        u32 new_scope_index = u32(sources.size() - 1);
        sources[new_scope_index].file_path = file_path;
        sources[new_scope_index].parent_scope_index = current_scope_index;
        current_scope_index = new_scope_index;
        tk.include = new_scope_index;

        u32 size = (u32)input.tellg();
        input.seekg(0);
        current_scope().source_code = new u8[size + 1];
        current_scope().source_len = size;

        input.read((char*)current_scope().source_code, size);
        current_scope().source_code[size] = '\0';
        input.close();

        lexer.set(
            current_scope().file_path.c_str(), 
            current_scope().source_code, 
            (u32)current_scope().source_len
        );

        advance();
        advance();

        process_source();

        current_scope_index = sources[new_scope_index].parent_scope_index;
        lexer = last_lexer;
        current = last_current;
        next = last_next;

        current_scope().tokens.emplace_back(tk);
    }
        break;
    default:
        current_scope().tokens.emplace_back(last);
        break;
    }

    
}

void PreProcessor::advance() {
    last = current;
    current = next;
    next = lexer.get_next();
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
