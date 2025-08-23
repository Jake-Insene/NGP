/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Frontend/CParser.h"
#include "ErrorManager.h"


void CParser::process(const char* file_path)
{
	pre_processor.process(file_path);
	token_index = 0;

	advance();
	advance();

	process_tokens();
}

void CParser::process_tokens()
{
	while (current->type != TOKEN_END_OF_FILE)
	{
		try_parse_statement();
	}
}

void CParser::advance()
{
	last = current;
	current = next;

	if (token_index < pre_processor.tokens.size())
	{
		next = &pre_processor.tokens[token_index++];
	}
}

bool CParser::expected(CTokenType type, const char* format, ...)
{
	advance();
	if (last->type != type)
	{
		va_list args;
		va_start(args, format);
		ErrorManager::errorV(last->get_source_file().data(), last->line, format, args);
		va_end(args);
		return false;
	}

	return true;
}

void CParser::add_declaration(const ASTStorageInfo& storage_info, StringID name, bool is_func)
{
	ASTDeclaration decl = {};
	decl.is_func = is_func;
	decl.name = name;
	decl.storage_info = storage_info;
	declarations.insert({ name, decl });
}


void CParser::try_parse_statement()
{
	ASTStorageInfo storage_info = parse_storage_info();

	if (!current->is(TOKEN_IDENTIFIER))
	{
		MAKE_ERROR((*current), return, "a name was expected");
	}

	StringID name = current->str;
	advance();

	if (current->is(TOKEN_LEFT_PARENT))
	{
		try_parse_function(storage_info, name);
	}
	else if (current->is(TOKEN_EQUAL))
	{
		try_parse_definition(storage_info, name);
	}
	else
	{
		expected(TOKEN_SEMICOLON, "';' was expected");
		add_declaration(storage_info, name, true);
	}
}

void CParser::try_parse_function(const ASTStorageInfo& return_type, StringID name)
{
	if (declarations.find(name) != declarations.end())
	{
		MAKE_ERROR((*last), return, "'%.*s' is already defined", last->get_str().length(), last->get_str().data());
	}

	if (!expected(TOKEN_LEFT_PARENT, "'(' was expected"))
	{
		return;
	}

	if (!expected(TOKEN_RIGHT_PARENT, "')' was expected"))
	{
		return;
	}

	if (current->is(TOKEN_SEMICOLON))
	{
		add_declaration(return_type, name, true);
		return;
	}

	if (!expected(TOKEN_LEFT_BRACE, "'{' was expected"))
	{
		return;
	}

	// Get expresions.

	expected(TOKEN_RIGHT_BRACE, "'}' was expected");
}

void CParser::try_parse_definition(const ASTStorageInfo& storage_info, StringID name)
{
	if (declarations.find(name) != declarations.end())
	{
		MAKE_ERROR((*last), return, "'%.*s' is already defined", last->get_str().length(), last->get_str().data());
	}
}

ASTStorageInfo CParser::parse_storage_info()
{
	ASTStorageInfo storage_info = {};

	while (current->is_storage_mod())
	{
		switch (current->subtype)
		{
		case TK_STATIC:
			storage_info.is_static = true;
			break;
		case TK_INLINE:
			storage_info.is_inline = true;
			break;
		case TK_CONST:
			storage_info.is_const = true;
			break;
		}

		advance();
	}

	if (current->is_sign_specifier())
	{
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 4;
		storage_info.is_signed = current->subtype == TK_SIGNED;
		advance();
	}

	if (!current->is_primitive_type())
	{
		MAKE_ERROR((*current), return ASTStorageInfo(), "unknown type");
	}

	switch (current->subtype)
	{
	case TK_VOID:
		storage_info.type = ASTStorageInfo::Void;
		storage_info.byte_size = 0;
		storage_info.type_name = StringPool::get_or_insert("void");
		break;
	case TK_CHAR:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 1;
		storage_info.type_name = StringPool::get_or_insert("char");
		break;
	case TK_SHORT:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 2;
		storage_info.type_name = StringPool::get_or_insert("short");
		break;
	case TK_INT:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 4;
		storage_info.type_name = StringPool::get_or_insert("int");
		break;
	case TK_LONG:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 1;
		storage_info.type_name = StringPool::get_or_insert("long");
		break;
	}

	return storage_info;
}