/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Frontend/CParser.h"
#include "ErrorManager.h"


struct ParseFn
{
	ASTNodeID(CParser::* prefix)(ASTNodeID, ASTNodeID);
	ASTNodeID(CParser::* infix)(ASTNodeID, ASTNodeID);

	CParsePrecedence precedence;
};

static inline ParseFn rules[TOKEN_COUNT] =
{
	{}, // ERROR
	{}, // END OF FILE
	{&CParser::parse_string, nullptr, CParsePrecedence::None},
	{&CParser::parse_number, nullptr, CParsePrecedence::None},
	{},
	{&CParser::parse_identifier, nullptr, CParsePrecedence::None},
	{}, // Keywords
	{}, // :
	{}, // ;
	{}, // ,
	{nullptr, &CParser::parse_access, CParsePrecedence::None}, // .
	{&CParser::parse_group, &CParser::parse_call, CParsePrecedence::Call}, // (
	{}, // )
	{&CParser::parse_array_list, &CParser::parse_index, CParsePrecedence::Call}, // [
	{}, // ]
	{&CParser::parse_block, nullptr, CParsePrecedence::None}, // {
	{}, // }
	{nullptr, &CParser::parse_assign, CParsePrecedence::Call}, // =
	{nullptr, &CParser::parse_comparison, CParsePrecedence::Equality}, // ==
	{nullptr, &CParser::parse_comparison, CParsePrecedence::Equality}, // !=
	{nullptr, &CParser::parse_comparison, CParsePrecedence::Comparision}, // <
	{nullptr, &CParser::parse_comparison, CParsePrecedence::Comparision}, // <=
	{nullptr, &CParser::parse_comparison, CParsePrecedence::Comparision}, // >
	{nullptr, &CParser::parse_comparison, CParsePrecedence::Comparision}, // >=
	{nullptr, &CParser::parse_binary_op, CParsePrecedence::BitwiseOr}, // or
	{nullptr, &CParser::parse_binary_op, CParsePrecedence::BitwiseAnd}, // and
	{&CParser::parse_unary_op, nullptr, CParsePrecedence::None}, // not
	{nullptr, &CParser::parse_binary_op, CParsePrecedence::BitwiseXor}, // xor
	{nullptr, &CParser::parse_binary_op, CParsePrecedence::BitwiseShr}, // shr
	{nullptr, &CParser::parse_binary_op, CParsePrecedence::BitwiseShl}, // shl

	{nullptr, &CParser::parse_binary_op, CParsePrecedence::Term}, // +
	{&CParser::parse_unary_op, &CParser::parse_binary_op, CParsePrecedence::Term}, // -
	{nullptr, &CParser::parse_binary_op, CParsePrecedence::Term}, // *
	{nullptr, &CParser::parse_binary_op, CParsePrecedence::Factor}, // /
};

static inline ParseFn get_rule(CTokenType type)
{
	return rules[type];
}

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
	else if (current->is_keyword(TK_IF))
	{
		return parse_if();
	}
	else if (current->is_keyword(TK_FOR))
	{
		return parse_for();
	}
	else if (current->is_keyword(TK_WHILE))
	{
		return parse_while();
	}

	ASTNodeID pe = try_parse_expression_statement();
	expected(TOKEN_SEMICOLON, "';' was expected");
	return pe;
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
		storage_info = ASTStorageInfo::integer(1, StringPool::get_or_insert("i8"), true);
		break;
	case TK_U8:
		storage_info = ASTStorageInfo::integer(1, StringPool::get_or_insert("u8"), false);
		break;
	case TK_I16:
		storage_info = ASTStorageInfo::integer(2, StringPool::get_or_insert("i16"), true);
		break;
	case TK_U16:
		storage_info = ASTStorageInfo::integer(2, StringPool::get_or_insert("u16"), false);
		break;
	case TK_I32:
		storage_info = ASTStorageInfo::integer(4, StringPool::get_or_insert("i32"), true);
		break;
	case TK_U32:
		storage_info = ASTStorageInfo::integer(4, StringPool::get_or_insert("u32"), false);
		break;
	case TK_F32:
		storage_info = ASTStorageInfo::floating(StringPool::get_or_insert("f32"));
		break;
	}

	advance();

	while (current->is(TOKEN_LEFT_KEY))
	{
		storage_info.is_array = true;
		storage_info.pointer_deep++;
		advance(); // [

		if(current->is(TOKEN_NUMBER))
		{
			storage_info.byte_size *= current->uword;
			advance();
		}

		if (!expected(TOKEN_RIGHT_KEY, "']' was expected"))
		{
			return ASTStorageInfo();
		}
	}

	return storage_info;
}

ASTNodeID CParser::parse_directive(ASTNodeID, ASTNodeID)
{
	return ASTNodeID(-1);
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
		return_info.type_name = StringPool::get_or_insert("void");
	}
	else
	{
		return_info = parse_storage_info();
		if (return_info.type == ASTStorageInfo::Unknown)
		{
			return ASTNodeID(-1);
		}
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
	advance(); // const keyword

	StringID name = current->str;
	if (declarations.find(name) != declarations.end())
	{
		MAKE_ERROR((*last), return ASTNodeID(-1), "'%.*s' is already defined", last->get_str().length(), last->get_str().data());
	}
	advance(); // name

	if (!expected(TOKEN_COLON, "':' was expected"))
	{
		return ASTNodeID(-1);
	}

	ASTStorageInfo type_info = parse_storage_info();
	if (type_info.type == ASTStorageInfo::Unknown)
	{
		return ASTNodeID(-1);
	}

	ASTNodeID expression_id = ASTNodeID(-1);
	if (current->is(TOKEN_EQUAL))
	{
		advance(); // =

		ASTNodeID expression_id = parse_expression(CParsePrecedence::Start);
		if (expression_id == ASTNodeID(-1))
		{
			return ASTNodeID(-1);
		}
	}

	if (!expected(TOKEN_SEMICOLON, "';' was expected"))
	{
		return ASTNodeID(-1);
	}

	ASTNodeID stat_var_id = create_node<ASTStatementVar>();
	ASTStatementVar* stat_var = get_node<ASTStatementVar>(stat_var_id);

	stat_var->name = name;
	stat_var->storage_info = type_info;
	stat_var->value = expression_id;

	return stat_var_id;
}

ASTNodeID CParser::parse_const()
{
	advance(); // const keyword

	StringID name = current->str;
	if (declarations.find(name) != declarations.end())
	{
		MAKE_ERROR((*last), return ASTNodeID(-1), "'%.*s' is already defined", last->get_str().length(), last->get_str().data());
	}
	advance(); // name

	if (!expected(TOKEN_COLON, "':' was expected"))
	{
		return ASTNodeID(-1);
	}

	ASTStorageInfo type_info = parse_storage_info();
	if (type_info.type == ASTStorageInfo::Unknown)
	{
		return ASTNodeID(-1);
	}

	if (!expected(TOKEN_EQUAL, "'=' was expected, always initialize a constant value"))
	{
		return ASTNodeID(-1);
	}

	ASTNodeID expression_id = parse_expression(CParsePrecedence::Start);
	if (expression_id == ASTNodeID(-1))
	{
		return ASTNodeID(-1);
	}

	if (!expected(TOKEN_SEMICOLON, "';' was expected"))
	{
		return ASTNodeID(-1);
	}

	ASTNodeID stat_const_id = create_node<ASTStatementConst>();
	ASTStatementConst* stat_const = get_node<ASTStatementConst>(stat_const_id);

	stat_const->name = name;
	stat_const->storage_info = type_info;
	stat_const->value = expression_id;

	return stat_const_id;
}

ASTNodeID CParser::parse_if()
{
	return ASTNodeID(-1);
}

ASTNodeID CParser::parse_for()
{
	return ASTNodeID(-1);
}

ASTNodeID CParser::parse_while()
{
	advance(); // while

	ASTNodeID cond_expr = parse_expression(CParsePrecedence::Start);
	if (cond_expr == ASTNodeID(-1))
	{
		return ASTNodeID(-1);
	}
	
	ASTNodeID expr = parse_expression(CParsePrecedence::Start);
	if (expr == ASTNodeID(-1))
	{
		return ASTNodeID(-1);
	}

	ASTNodeID stat_while_id = create_node<ASTStatementWhile>();
	ASTStatementWhile* stat_while = get_node<ASTStatementWhile>(stat_while_id);

	stat_while->cond_expr = cond_expr;
	stat_while->expression = expr;

	return stat_while_id;
}

ASTNodeID CParser::try_parse_expression_statement()
{
	ASTNodeID expr_stat_id = create_node<ASTExpressionStatement>();
	ASTExpressionStatement* expr_stat = get_node<ASTExpressionStatement>(expr_stat_id);

	expr_stat->value = parse_expression(CParsePrecedence::Start);
	if (expr_stat->value == ASTNodeID(-1))
	{
		return ASTNodeID(-1);
	}

	return expr_stat_id;
}

ASTNodeID CParser::parse_expression(CParsePrecedence precedence)
{
	ParseFn rule = get_rule(current->type);
	advance();

	ASTNodeID result = ASTNodeID(-1);
	if (rule.prefix)
	{
		result = (this->*rule.prefix)(ASTNodeID(-1), ASTNodeID(-1));
	}
	else
	{
		MAKE_ERROR((*last), return result, "invalid expression");
	}

	while (u32(precedence) <= u32((rule = get_rule(current->type)).precedence))
	{
		advance();
		result = (this->*rule.infix)(result, {});
	}

	return result;
}

ASTNodeID CParser::parse_string(ASTNodeID, ASTNodeID)
{
	return ASTNodeID(-1);
}

ASTNodeID CParser::parse_number(ASTNodeID, ASTNodeID)
{
	ASTNodeID num_id = create_node<ASTExpressionConstant>();
	ASTExpressionConstant* num = get_node<ASTExpressionConstant>(num_id);

	switch (last->subtype)
	{
	case TN_BYTE:
		num->storage_info = ASTStorageInfo::integer(1, StringPool::get_or_insert("u8"), false);
		break;
	case TN_SIGNED:
		num->storage_info = ASTStorageInfo::integer(4, StringPool::get_or_insert("i32"), true);
		break;
	case TN_UNSIGNED:
		num->storage_info = ASTStorageInfo::integer(4, StringPool::get_or_insert("u32"), false);
		break;
	case TN_FLOAT:
		num->storage_info = ASTStorageInfo::floating(StringPool::get_or_insert("f32"));
		break;
	}

	// Raw copy
	num->u = last->uword;

	return num_id;
}

ASTNodeID CParser::parse_identifier(ASTNodeID, ASTNodeID)
{
	ASTNodeID identifier_id = create_node<ASTExpressionIdentifier>();
	ASTExpressionIdentifier* identifier = get_node<ASTExpressionIdentifier>(identifier_id);

	identifier->id = last->str;

	return identifier_id;
}

ASTNodeID CParser::parse_access(ASTNodeID, ASTNodeID)
{
	return ASTNodeID(-1);
}

ASTNodeID CParser::parse_group(ASTNodeID, ASTNodeID)
{
	ASTNodeID group_id = create_node<ASTExpressionGroup>();
	ASTExpressionGroup* group = get_node<ASTExpressionGroup>(group_id);

	group->expression = parse_expression(CParsePrecedence::Start);

	if (!expected(TOKEN_RIGHT_PARENT, "')' was expected"))
	{
		return ASTNodeID(-1);
	}

	return group_id;
}

ASTNodeID CParser::parse_call(ASTNodeID function, ASTNodeID)
{
	ASTNodeID call_id = create_node<ASTExpressionCall>();
	ASTExpressionCall* call = get_node<ASTExpressionCall>(call_id);

	call->function = function;

	while (!current->is(TOKEN_RIGHT_PARENT))
	{
		ASTNodeID expr = parse_expression(CParsePrecedence::Start);
		if (expr == ASTNodeID(-1))
		{
			return ASTNodeID(-1);
		}

		call->arguments.emplace_back(expr);

		if (current->is(TOKEN_COMMA))
		{
			advance(); // ,
		}
	}

	if (!expected(TOKEN_RIGHT_PARENT, "')' was expected"))
	{
		return ASTNodeID(-1);
	}

	return call_id;
}

ASTNodeID CParser::parse_index(ASTNodeID base, ASTNodeID)
{
	ASTNodeID index_id = create_node<ASTExpressionIndex>();
	ASTExpressionIndex* index = get_node<ASTExpressionIndex>(index_id);

	index->base = base;

	index->index = parse_expression(CParsePrecedence::Start);
	if (index->index == ASTNodeID(-1))
	{
		return ASTNodeID(-1);
	}

	if (!expected(TOKEN_RIGHT_KEY, "']' was expected"))
	{
		return ASTNodeID(-1);
	}

	return index_id;
}

ASTNodeID CParser::parse_array_list(ASTNodeID, ASTNodeID)
{
	ASTNodeID array_list_id = create_node<ASTExpressionArrayList>();
	ASTExpressionArrayList* array_list = get_node<ASTExpressionArrayList>(array_list_id);

	while (!current->is(TOKEN_RIGHT_KEY))
	{
		ASTNodeID expr = parse_expression(CParsePrecedence::Start);
		if (expr == ASTNodeID(-1))
		{
			return ASTNodeID(-1);
		}

		array_list->expressions.emplace_back(expr);

		if (current->is(TOKEN_COMMA))
		{
			advance(); // ,
		}
	}

	if (!expected(TOKEN_RIGHT_KEY, "']' was expected"))
	{
		return ASTNodeID(-1);
	}

	return array_list_id;
}

ASTNodeID CParser::parse_block(ASTNodeID, ASTNodeID)
{
	ASTNodeID block_id = create_node<ASTExpressionBlock>();
	ASTExpressionBlock* block = get_node<ASTExpressionBlock>(block_id);

	while (!current->is(TOKEN_RIGHT_BRACE))
	{
		ASTNodeID expr = try_parse_statement();
		if (expr == ASTNodeID(-1))
		{
			return ASTNodeID(-1);
		}

		block->statements.emplace_back(expr);
	}

	if (!expected(TOKEN_RIGHT_BRACE, "'}' was expected"))
	{
		return ASTNodeID(-1);
	}

	return block_id;
}

ASTNodeID CParser::parse_assign(ASTNodeID dest, ASTNodeID)
{
	ASTNodeID assign_id = create_node<ASTExpressionAssing>();
	ASTExpressionAssing* assign = get_node<ASTExpressionAssing>(assign_id);

	assign->dest = dest;

	assign->value = parse_expression(CParsePrecedence::Start);
	if (assign->value == ASTNodeID(-1))
	{
		return ASTNodeID(-1);
	}

	return assign_id;
}

ASTNodeID CParser::parse_comparison(ASTNodeID, ASTNodeID)
{
	return ASTNodeID(-1);
}

ASTNodeID CParser::parse_binary_op(ASTNodeID left, ASTNodeID)
{
	ASTNodeID binary_op_id = create_node<ASTExpressionBinaryOp>();
	ASTExpressionBinaryOp* binary_op = get_node<ASTExpressionBinaryOp>(binary_op_id);

	binary_op->left = left;

	switch (last->type)
	{
	case TOKEN_OR:
		binary_op->op = ASTExpressionBinaryOp::Or;
		break;
	case TOKEN_AND:
		binary_op->op = ASTExpressionBinaryOp::And;
		break;
	case TOKEN_XOR:
		binary_op->op = ASTExpressionBinaryOp::Xor;
		break;
	case TOKEN_SHR:
		binary_op->op = ASTExpressionBinaryOp::Shr;
		break;
	case TOKEN_SHL:
		binary_op->op = ASTExpressionBinaryOp::Shl;
		break;
	case TOKEN_PLUS:
		binary_op->op = ASTExpressionBinaryOp::Add;
		break;
	case TOKEN_MINUS:
		binary_op->op = ASTExpressionBinaryOp::Sub;
		break;
	case TOKEN_STAR:
		binary_op->op = ASTExpressionBinaryOp::Mul;
		break;
	case TOKEN_SLASH:
		binary_op->op = ASTExpressionBinaryOp::Div;
		break;
	}

	binary_op->right = parse_expression(get_rule(last->type).precedence);
	if (binary_op->right == ASTNodeID(-1))
	{
		return ASTNodeID(-1);
	}

	return binary_op_id;
}

ASTNodeID CParser::parse_unary_op(ASTNodeID, ASTNodeID)
{
	ASTNodeID unary_op_id = create_node<ASTExpressionUnaryOp>();
	ASTExpressionUnaryOp* unary_op = get_node<ASTExpressionUnaryOp>(unary_op_id);

	switch (last->type)
	{
	case TOKEN_NOT:
		unary_op->op = ASTExpressionUnaryOp::Not;
		break;
	case TOKEN_MINUS:
		unary_op->op = ASTExpressionUnaryOp::Neg;
		break;
	}

	unary_op->expression = parse_expression(CParsePrecedence::Primary);
	if (unary_op->expression == ASTNodeID(-1))
	{
		return ASTNodeID(-1);
	}

	return unary_op_id;
}
