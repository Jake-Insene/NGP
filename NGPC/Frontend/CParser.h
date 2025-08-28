/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/CPreProcessor.h"
#include <vector>
#include <memory>


enum ASTNodeType
{
	AST_NODE_UNKNOWN = 0,

	AST_STATEMENT,
	AST_EXPRESSION,
};


enum ASTStatementType
{
	AST_STATEMENT_UNKNOWN = 0,

	AST_STATEMENT_FUNC,
	AST_STATEMENT_VAR,
	AST_STATEMENT_CONST,
	AST_STATEMENT_IF,
	AST_STATEMENT_FOR,
	AST_STATEMENT_WHILE,
	AST_STATEMENT_EXPRESSION_STATEMENT,
};

enum ASTExpressionType
{
	AST_EXPRESSION_UNKNOWN = 0,

	AST_EXPRESSION_CONSTANT,
	AST_EXPRESSION_IDENTIFIER,
	AST_EXPRESSION_GROUP,
	AST_EXPRESSION_CALL,
	AST_EXPRESSION_INDEX,
	AST_EXPRESSION_ARRAY_LIST,
	AST_EXPRESSION_BLOCK,
	AST_EXPRESSION_ASSIGN,
	AST_EXPRESSION_BINARY_OP,
	AST_EXPRESSION_UNARY_OP,
};

using ASTNodeID = usize;

struct ASTNode
{
	ASTNodeType type;
};

struct ASTStatement : ASTNode
{
	ASTStatement() { type = AST_STATEMENT; };

	ASTStatementType statement_type;
};

struct ASTExpression : ASTNode
{
	ASTExpression() { type = AST_EXPRESSION; };

	ASTExpressionType expression_type;
};

struct ASTStorageInfo
{
	static constexpr ASTStorageInfo integer(usize byte_size, StringID type_name, bool is_signed)
	{
		return ASTStorageInfo
		{
			.type = Integer,
			.byte_size = byte_size,
			.type_name = type_name,
			.is_signed = is_signed,
		};
	}

	static constexpr ASTStorageInfo floating(StringID type_name)
	{
		return ASTStorageInfo
		{
			.type = Integer,
			.byte_size = 4,
			.type_name = type_name,
			.is_signed = true,
		};
	}

	enum StorageType
	{
		Unknown = 0,
		Void,
		Integer,
		FloatingPoint,
	};


	StorageType type;
	usize byte_size;
	StringID type_name;
	usize pointer_deep;

	bool is_static;
	bool is_inline;
	bool is_array;
	bool is_signed;
};

struct ASTStatementFunc : ASTStatement
{
	ASTStatementFunc() { statement_type = AST_STATEMENT_FUNC; }
	
	ASTStorageInfo return_info;
	StringID name;
	std::vector<ASTNodeID> statements;
};

struct ASTStatementVar : ASTStatement
{
	ASTStatementVar() { statement_type = AST_STATEMENT_VAR; }

	ASTStorageInfo storage_info;
	StringID name;
	ASTNodeID value;
};

struct ASTStatementConst : ASTStatement
{
	ASTStatementConst() { statement_type = AST_STATEMENT_CONST; }

	ASTStorageInfo storage_info;
	StringID name;
	ASTNodeID value;
};

struct ASTStatementIf : ASTStatement
{
	ASTStatementIf() { statement_type = AST_STATEMENT_IF; }

	ASTNodeID cond_expr;
	ASTNodeID expression;
	ASTNodeID elif_expression;
	ASTNodeID else_expression;
};

struct ASTStatementFor : ASTStatement
{
	ASTStatementFor() { statement_type = AST_STATEMENT_FOR; }

	ASTNodeID pre_expr;
	ASTNodeID cond_expr;
	ASTNodeID post_expr;
	ASTNodeID expression;
};

struct ASTStatementWhile : ASTStatement
{
	ASTStatementWhile() { statement_type = AST_STATEMENT_WHILE; }

	ASTNodeID cond_expr;
	ASTNodeID expression;
};

struct ASTExpressionStatement : ASTStatement
{
	ASTExpressionStatement() { statement_type = AST_STATEMENT_EXPRESSION_STATEMENT; }

	ASTNodeID value;
};

struct ASTExpressionConstant : ASTExpression
{
	ASTExpressionConstant() { expression_type = AST_EXPRESSION_CONSTANT; }

	ASTStorageInfo storage_info;
	union
	{
		i32 i;
		u32 u;
		f32 s;
		ASTNodeID str;
	};
};

struct ASTExpressionIdentifier : ASTExpression
{
	ASTExpressionIdentifier() { expression_type = AST_EXPRESSION_IDENTIFIER; }

	StringID id;
};

struct ASTExpressionGroup : ASTExpression
{
	ASTExpressionGroup() { expression_type = AST_EXPRESSION_GROUP; }

	ASTNodeID expression;
};

struct ASTExpressionCall : ASTExpression
{
	ASTExpressionCall() { expression_type = AST_EXPRESSION_CALL; }

	ASTNodeID function;
	std::vector<ASTNodeID> arguments;
};

struct ASTExpressionIndex : ASTExpression
{
	ASTExpressionIndex() { expression_type = AST_EXPRESSION_INDEX; }

	ASTNodeID base;
	ASTNodeID index;
};

struct ASTExpressionArrayList : ASTExpression
{
	ASTExpressionArrayList() { expression_type = AST_EXPRESSION_ARRAY_LIST; }

	std::vector<ASTNodeID> expressions;
};

struct ASTExpressionBlock : ASTExpression
{
	ASTExpressionBlock() { expression_type = AST_EXPRESSION_BLOCK; }

	std::vector<ASTNodeID> statements;
};

struct ASTExpressionAssing : ASTExpression
{
	ASTExpressionAssing() { expression_type = AST_EXPRESSION_ASSIGN; }

	ASTNodeID dest;
	ASTNodeID value;
};

struct ASTExpressionBinaryOp : ASTExpression
{
	ASTExpressionBinaryOp() { expression_type = AST_EXPRESSION_BINARY_OP; }

	enum BinaryOpType
	{
		Or,
		And,
		Xor,
		Shr,
		Shl,
		Add,
		Sub,
		Mul,
		Div,
	};

	BinaryOpType op;
	ASTNodeID left;
	ASTNodeID right;
};

struct ASTExpressionUnaryOp : ASTExpression
{
	ASTExpressionUnaryOp() { expression_type = AST_EXPRESSION_UNARY_OP; }

	enum UnaryOpType
	{
		Not,
		Neg,
	};

	UnaryOpType op;
	ASTNodeID expression;
};

struct ASTNodePool
{
	std::vector<ASTNode*> nodes;

	~ASTNodePool()
	{
		for (auto node : nodes)
		{
			delete node;
		}
	}

	[[nodiscard]] ASTNodeID add(ASTNode* node)
	{
		nodes.emplace_back(node);
		return nodes.size() - 1;
	}

	[[nodiscard]] ASTNode* get(ASTNodeID id)
	{
		return nodes[id];
	}
};

enum class CParsePrecedence
{
	None,
	Start,
	BitwiseOr,
	BitwiseXor,
	BitwiseAnd,
	BitwiseShr,
	BitwiseShl,
	Equality, // == !=
	Comparision, // < > <= >=
	Shift, // >> <<
	Term, // + -
	Factor, // * /
	Unary, // ! -
	Primary,
	Call,
};

struct CParser
{
	CPreProcessor pre_processor;

	CToken* last;
	CToken* current;
	CToken* next;
	u32 token_index;

	ASTNodePool pool;
	std::vector<ASTNodeID> main_nodes;

	std::unordered_map<StringID, ASTNodeID> declarations;

	void process(const char* file_path);
	void process_tokens();
	void advance();

	bool expected(CTokenType type, const char* format, ...);
	void synchronize();

	template<typename T>
	[[nodiscard]] ASTNodeID create_node()
	{ 
		T* node = new T();
		return pool.add(node);
	}

	template<typename T>
	[[nodiscard]] T* get_node(ASTNodeID node)
	{
		return reinterpret_cast<T*>(pool.get(node));
	}

	std::vector<ASTNodeID>& get_main_nodes() { return main_nodes; }

	ASTNodeID try_parse_statement();
	ASTStorageInfo parse_storage_info();

	// Statements
	ASTNodeID parse_directive(ASTNodeID, ASTNodeID);
	ASTNodeID parse_function();
	ASTNodeID parse_var();
	ASTNodeID parse_const();
	ASTNodeID parse_if();
	ASTNodeID parse_for();
	ASTNodeID parse_while();
	ASTNodeID try_parse_expression_statement();

	// Expressions
	ASTNodeID parse_expression(CParsePrecedence precedence);

	ASTNodeID parse_string(ASTNodeID, ASTNodeID);
	ASTNodeID parse_number(ASTNodeID, ASTNodeID);
	ASTNodeID parse_identifier(ASTNodeID, ASTNodeID);
	ASTNodeID parse_access(ASTNodeID, ASTNodeID);
	ASTNodeID parse_group(ASTNodeID, ASTNodeID);
	ASTNodeID parse_call(ASTNodeID, ASTNodeID);
	ASTNodeID parse_index(ASTNodeID, ASTNodeID);
	ASTNodeID parse_array_list(ASTNodeID, ASTNodeID);
	ASTNodeID parse_block(ASTNodeID, ASTNodeID);
	ASTNodeID parse_assign(ASTNodeID, ASTNodeID);
	ASTNodeID parse_comparison(ASTNodeID, ASTNodeID);
	ASTNodeID parse_binary_op(ASTNodeID, ASTNodeID);
	ASTNodeID parse_unary_op(ASTNodeID, ASTNodeID);
};