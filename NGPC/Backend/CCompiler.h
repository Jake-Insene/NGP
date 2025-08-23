/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/CParser.h"


struct CCompiler
{
	std::string output_str;
	CParser parser;

	bool compile_file(const char* file_path, const char* output_path);

	void compile_statement(ASTStatement* statement);

	void compile_function(ASTStatementFunc* func);

	void compile_func_statement(ASTStatement* statement);
};