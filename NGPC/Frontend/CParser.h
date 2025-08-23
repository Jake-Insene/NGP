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

	AST_NODE_STRUCT,
	AST_NODE_DECLARATION,
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
	bool is_const;
	bool is_ptr;
	bool is_signed;
};

struct ASTDeclaration : ASTNode
{
	ASTDeclaration() { type = AST_NODE_DECLARATION; };

	ASTStorageInfo storage_info;
	StringID name;

	bool is_func;
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
	std::vector<ASTNode> main_nodes;

	std::unordered_map<StringID, ASTDeclaration> declarations;

	void process(const char* file_path);
	void process_tokens();
	void advance();

	bool expected(CTokenType type, const char* format, ...);
	void add_declaration(const ASTStorageInfo& storage_info, StringID name, bool is_func);

	void try_parse_statement();
	void try_parse_function(const ASTStorageInfo& return_type, StringID name);
	void try_parse_definition(const ASTStorageInfo& storage_info, StringID name);
	ASTStorageInfo parse_storage_info();
};