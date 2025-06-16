/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Frontend/AsmPreProcessor.h"

#include "Backend/AssemblerUtility.h"
#include "ErrorManager.h"
#include "Frontend/Token.h"
#include "Frontend/AsmUtility.h"
#include <fstream>  

void AsmPreProcessor::process(const char* file_path)
{
    sources.clear();

    std::ifstream input{ file_path, std::ios::binary | std::ios::ate };
    if (!input.is_open())
    {
        printf("error: couldn't load the file '%s'\n", file_path);
        exit(-1);
    }

    auto& source = sources.emplace_back();
    source.file_path = StringPool::get_or_insert(file_path);
    source.index = u32(sources.size() - 1);

    u32 size = (u32)input.tellg();
    input.seekg(0);
    source.source_code = new u8[size + 1];
    source.source_len = size + 1;
    input.read((char*)source.source_code, size);
    source.source_code[size] = '\0';
    input.close();

    lexer.set(source.file_path, (u8*)source.source_code, source.source_len);

    advance();
    advance();

    process_source();

    Token end_of_file{ .type = TOKEN_END_OF_FILE };
    tokens.emplace_back(end_of_file);
}

void AsmPreProcessor::process_source()
{
    while (!current.is(TOKEN_END_OF_FILE))
    {
        switch (current.type)
        {
        case TOKEN_DIRECTIVE:
            process_directive();
            break;
        default:
            if (current.is(TOKEN_SYMBOL))
            {
                auto it = macros.find(current.str);
                if (it != macros.end())
                {
                    advance();
                    expand_macro(it->second);
                }
            }

            tokens.emplace_back(current);
            advance();
            break;
        }
    }
}

void AsmPreProcessor::process_directive()
{
    advance();

    switch (last.subtype)
    {
    case TD_INCLUDE:
    {
        if (!expected(TOKEN_STRING, "a file path was expected"))
        {
            break;
        }

        AsmLexer last_lexer = lexer;
        Token last_current = current;
        Token last_next = next;

        std::string file_path_tmp{};
        file_path_tmp.resize(last.get_str().size());
        encode_string((u8*)file_path_tmp.data(), last.str);
        StringID file_path_tmp_id = StringPool::get_or_insert(file_path_tmp);

        std::string file_path = AsmUtility::path_relative_to(last.source_file, file_path_tmp_id);
        StringID file_path_id = StringPool::get_or_insert(file_path);

        std::ifstream input{ file_path, std::ios::binary | std::ios::ate };
        if (!input.is_open())
        {
            ErrorManager::error(
                last.get_source_file().data(), last.line,
                "the file '%s' was not founded", file_path.c_str()
            );
            return;
        }

        for (auto& source : sources)
        {
            if (source.get_file_path() == file_path)
            {
                lexer = last_lexer;
                current = last_current;
                next = last_next;
                return;
            }
        }

        auto& new_source = sources.emplace_back();
        new_source.index = u32(sources.size() - 1);
        new_source.file_path = file_path_id;

        u32 size = (u32)input.tellg();
        input.seekg(0);
        new_source.source_code = new u8[size + 1];
        new_source.source_len = size;

        input.read((char*)new_source.source_code, size);
        new_source.source_code[size] = '\0';
        input.close();

        lexer.set(
            new_source.file_path,
            new_source.source_code,
            new_source.source_len
        );

        advance();
        advance();

        process_source();

        lexer = last_lexer;
        current = last_current;
        next = last_next;
    }
    break;
    case TD_MACRO:
    {
        if(!expected(TOKEN_SYMBOL, "an macro name was expected"))
        {
            return;
        }

        if(macros.find(last.str) != macros.end())
        {
            ErrorManager::error(last.get_source_file().data(), last.line, "the macro '%.*s' was already defined", last.get_str().length(), last.get_str().data());
            break;
        }

        MacroDefinition& macro = macros.emplace(
            last.str,
            MacroDefinition()
        ).first->second;

        while(!current.is(TOKEN_LEFT_BRACE) && !current.is(TOKEN_END_OF_FILE))
        {
            if (current.is(TOKEN_SYMBOL))
            {
                macro.args_name.emplace_back(current.str);
                advance();

                if (!current.is(TOKEN_LEFT_BRACE) && !expected(TOKEN_COMMA, "a ',' or '{' was expected after a symbol"))
                {
                    break;
                }
            }
            else
                break;
        }

        if (!expected(TOKEN_LEFT_BRACE, "'{' was expected after macro declaration"))
        {
            break;
        }

        while (!current.is(TOKEN_RIGHT_BRACE) && !current.is(TOKEN_END_OF_FILE))
        {
            macro.tokens.emplace_back(current);
            advance();
        }

        if (!expected(TOKEN_RIGHT_BRACE, "'{' was expected after macro declaration"))
        {
            break;
        }
    }
    break;
    default:
        tokens.emplace_back(last);
        break;
    }
}

void AsmPreProcessor::expand_macro(const MacroDefinition& macro)
{
    StringID source_file = last.source_file;
    const u32 line = last.line;

    // Get arguments
    const usize argument_count = macro.args_name.size();
    usize arg_index = 0;

    std::unordered_map<StringID, Token> argument_values = {};

    while (!current.is(TOKEN_NEW_LINE) && !current.is(TOKEN_END_OF_FILE))
    {
        if (arg_index >= argument_count)
        {
            ErrorManager::error(current.get_source_file().data(), current.line, "too much macro arguments");
            return;
        }

        StringID arg_name = macro.args_name[arg_index];

        argument_values.emplace(arg_name, current);
        arg_index++;
        advance();

        if (current.is(TOKEN_NEW_LINE) || !expected(TOKEN_COMMA, "a ',' or 'new line' was expected after symbol"))
        {
            break;
        }
    }

    if (arg_index < argument_count)
    {
        ErrorManager::error(current.get_source_file().data(), current.line, "too few macro arguments");
        return;
    }

    Token new_line =
    {
        .source_file = current.source_file,
        .line = current.line,
        .type = TOKEN_NEW_LINE,
    };

    for (auto& tk : macro.tokens)
    {
        if (tk.is(TOKEN_SYMBOL))
        {
            StringID s = tk.str;
            auto it = argument_values.find(s);
            if (it != argument_values.end())
            {
                Token new_token = it->second;
                new_token.source_file = source_file;
                new_token.line = line;
                tokens.emplace_back(new_token);
            }
        }
        else
        {
            Token new_token = tk;
            new_token.source_file = source_file;
            new_token.line = line;
            tokens.emplace_back(new_token);
        }
    }
}

void AsmPreProcessor::advance()
{
    last = current;
    current = next;
    next = lexer.get_next();
}

bool AsmPreProcessor::expected(TokenType type, const char* format, ...)
{
    advance();
    if (last.type != type)
    {
        va_list args;
        va_start(args, format);
        ErrorManager::errorV(last.get_source_file().data(), last.line, format, args);
        va_end(args);
        return false;
    }

    return true;
}
