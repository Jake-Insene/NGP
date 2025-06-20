/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/CPreProcessor.h"
#include <vector>


using ASTNodeID = usize;

enum ASTNodeType
{
	AST_NODE_UNKNOWN = 0,

	AST_NODE_STRUCT,
	AST_NODE_VAR,
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


struct CParser
{
	CPreProcessor pre_processor;

	CToken* last;
	CToken* current;
	CToken* next;
	u32 token_index;

	std::vector<ASTNode> nodes;

	void process(const char* file_path);

	void process_tokens();

	void advance();
};