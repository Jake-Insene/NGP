/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Frontend/CParser.h"


struct AllocationInfo
{
	enum AllocationType
	{
		Stack,
		Register,
	};

	AllocationType allocation_type;
	usize sp_offset;
	usize register_index;
};

struct LocalInfo
{
	ASTStorageInfo type_info;
	AllocationInfo allocation;
};

struct FunctionInfo
{
	StringID name;
	ASTStorageInfo return_type;

	usize sp;
	std::unordered_map<StringID, LocalInfo> locals;

	LocalInfo add_local(StringID name, ASTStorageInfo type_info)
	{
		LocalInfo& local = locals.insert({ name, LocalInfo() }).first->second;
		local.type_info = type_info;
		local.allocation.allocation_type = AllocationInfo::Stack;
		local.allocation.sp_offset = sp;
		sp += type_info.byte_size;

		return local;
	}
};

struct CCompiler
{
	std::string output_str;
	CParser parser;

	std::unordered_map<StringID, FunctionInfo> functions;

	bool compile_file(const char* file_path, const char* output_path);

	void compile_statement(ASTStatement* statement);

	void compile_function(ASTStatementFunc* stat_func);

	void compile_func_statement(FunctionInfo& func, ASTStatement* statement);
	void compile_func_var(FunctionInfo& func, ASTStatementVar* var);
};