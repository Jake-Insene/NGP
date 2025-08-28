/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Backend/CCompiler.h"
#include "ErrorManager.h"
#include <fstream>


bool CCompiler::compile_file(const char* file_path, const char* output_path)
{
    parser.process(file_path);
    if (ErrorManager::is_panic_mode)
    {
        return false;
    }

    std::vector<ASTNodeID> statements = parser.get_main_nodes();
    for (auto stat_id : statements)
    {
        ASTStatement* statement = parser.get_node<ASTStatement>(stat_id);
        compile_statement(statement);
    }

    std::string output_file_name = output_path;
    output_file_name += ".s";

    std::ofstream output{ output_file_name, std::ios::binary };
    output << output_str;

	return true;
}

void CCompiler::compile_statement(ASTStatement* statement)
{
    if (statement->statement_type == AST_STATEMENT_FUNC)
    {
        compile_function((ASTStatementFunc*)statement);
    }
}

void CCompiler::compile_function(ASTStatementFunc* stat_func)
{
    output_str.append(StringPool::get(stat_func->name));
    output_str.append(":\n");

    FunctionInfo& func = functions.insert({ stat_func->name, FunctionInfo() }).first->second;
    func.name = stat_func->name;
    func.return_type = stat_func->return_info;

    for (auto stat_id : stat_func->statements)
    {
        compile_func_statement(func, parser.get_node<ASTStatement>(stat_id));
    }

    output_str.append("\tret\n");
}

void CCompiler::compile_func_statement(FunctionInfo& func, ASTStatement* statement)
{
    if (statement->statement_type == AST_STATEMENT_FUNC)
    {

    }
    else if (statement->statement_type == AST_STATEMENT_VAR)
    {
        compile_func_var(func, (ASTStatementVar*)statement);
    }
    else if (statement->statement_type == AST_STATEMENT_CONST)
    {

    }
}

void CCompiler::compile_func_var(FunctionInfo& func, ASTStatementVar* var)
{
    func.add_local(var->name, var->storage_info);
}
