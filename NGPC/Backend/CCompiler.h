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
	CParser parser;

	bool compile_file(const char* file_path, const char* output_path);
};