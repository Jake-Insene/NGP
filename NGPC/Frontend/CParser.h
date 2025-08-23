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

using ASTNodeID = usize;

enum ASTNodeType
{
	AST_NODE_UNKNOWN = 0,

	AST_STATEMENT,

	AST_NODE_STRUCT,
	AST_NODE_STATEMENT,
	AST_NODE_DEFINITION,
	AST_NODE_CONST,
	AST_NODE_FUNCTION,
	AST_NODE_IF,
	AST_NODE_ELSE,
	AST_NODE_FOR,
	AST_NODE_WHILE,
	AST_NODE_DO_WHILE,
	AST_NODE_GOTO,
	AST_NODE_RETURN,
};

enum ASTStatementType
{
	AST_STATEMENT_UNKNOWN = 0,

	AST_STATEMENT_FUNC,
	AST_STATEMENT_VAR,
	AST_STATEMENT_CONST,
	AST_STATEMENT_EXPRESION_STATEMENT,
};

struct ASTNode
{
	ASTNodeType type;
};

struct ASTStorageInfo
{
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
	bool is_ptr;
	bool is_signed;
};

struct ASTStatement : ASTNode
{
	ASTStatement() { type = AST_NODE_STATEMENT; };

	ASTStatementType statement_type;
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

using ASTNodeID = usize;

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

	ASTNodeID parse_function();
	ASTNodeID parse_var();
	ASTNodeID parse_const();
	ASTNodeID try_parse_expression_statement();

	ASTStorageInfo parse_storage_info();
};