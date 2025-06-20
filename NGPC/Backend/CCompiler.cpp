/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Backend/CCompiler.h"
#include "ErrorManager.h"


bool CCompiler::compile_file(const char* file_path, const char* output_path)
{
    parser.process(file_path);
    if (ErrorManager::is_panic_mode)
    {
        return false;
    }

	return true;
}
