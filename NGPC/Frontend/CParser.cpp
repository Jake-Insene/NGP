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
		ASTNodeID node = try_parse_statement();
		if (node == ASTNodeID(-1))
		{
			if (ErrorManager::must_synchronize)
			{
				synchronize();
			}
		}
		else
		{
			main_nodes.emplace_back(node);
		}
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

void CParser::synchronize()
{
	while (true)
	{
		if (current->is(TOKEN_END_OF_FILE))
		{
			return;
		}
		else if (!current->is(TOKEN_KEYWORD))
		{
			advance();
			continue;
		}

		switch (current->subtype)
		{
		case TK_FUNC:
		case TK_VAR:
		case TK_CONST:
			return;
		}

		advance();
	}
}

ASTNodeID CParser::try_parse_statement()
{
	if (current->is_func())
	{
		return parse_function();
	}
	else if (current->is_var())
	{
		return parse_var();
	}
	else if (current->is_const())
	{
		return parse_const();
	}

	ASTNodeID pe = try_parse_expression_statement();
	expected(TOKEN_SEMICOLON, "';' was expected");
	return pe;
}

ASTNodeID CParser::parse_function()
{
	advance(); // func keyword

	StringID name = current->str;
	if (declarations.find(name) != declarations.end())
	{
		MAKE_ERROR((*last), return ASTNodeID(-1), "'%.*s' is already defined", last->get_str().length(), last->get_str().data());
	}
	advance(); // name

	if (!expected(TOKEN_LEFT_PARENT, "'(' was expected"))
	{
		return ASTNodeID(-1);
	}

	if (!expected(TOKEN_RIGHT_PARENT, "')' was expected"))
	{
		return ASTNodeID(-1);
	}

	ASTStorageInfo return_info = {};
	if (current->is(TOKEN_LEFT_BRACE))
	{
		return_info.type = ASTStorageInfo::Void;
	}
	else
	{
		return_info = parse_storage_info();
	}

	if (!expected(TOKEN_LEFT_BRACE, "'{' was expected"))
	{
		return ASTNodeID(-1);
	}

	// Get expressions.
	std::vector<ASTNodeID> statements = {};
	while (!current->is(TOKEN_END_OF_FILE) && !current->is(TOKEN_RIGHT_BRACE))
	{
		ASTNodeID node = try_parse_statement();
		statements.emplace_back(node);
	}

	if (!expected(TOKEN_RIGHT_BRACE, "'}' was expected"))
	{
		return ASTNodeID(-1);
	}

	ASTNodeID func_id = create_node<ASTStatementFunc>();
	ASTStatementFunc* func = get_node<ASTStatementFunc>(func_id);
	func->return_info = return_info;
	func->name = name;
	func->statements = statements;
	return func_id;
}

ASTNodeID CParser::parse_var()
{
	advance(); // var keyword

	return ASTNodeID(-1);
}

ASTNodeID CParser::parse_const()
{
	advance(); // const keyword

	return ASTNodeID(-1);
}

ASTNodeID CParser::try_parse_expression_statement()
{
	return ASTNodeID(-1);
}

ASTStorageInfo CParser::parse_storage_info()
{
	ASTStorageInfo storage_info = {};
	
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
	case TK_I8:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 1;
		storage_info.type_name = StringPool::get_or_insert("char");
		storage_info.is_signed = true;
		break;
	case TK_U8:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 1;
		storage_info.type_name = StringPool::get_or_insert("short");
		break;
	case TK_I16:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 2;
		storage_info.type_name = StringPool::get_or_insert("int");
		storage_info.is_signed = true;
		break;
	case TK_U16:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 2;
		storage_info.type_name = StringPool::get_or_insert("long");
		break;
	case TK_I32:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 4;
		storage_info.type_name = StringPool::get_or_insert("long");
		storage_info.is_signed = true;
		break;
	case TK_U32:
		storage_info.type = ASTStorageInfo::Integer;
		storage_info.byte_size = 4;
		storage_info.type_name = StringPool::get_or_insert("long");
		break;
	case TK_F32:
		storage_info.type = ASTStorageInfo::FloatingPoint;
		storage_info.byte_size = 4;
		storage_info.type_name = StringPool::get_or_insert("long");
		storage_info.is_signed = true;
		break;
	}

	return storage_info;
}