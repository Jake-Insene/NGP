/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Frontend/CParser.h"


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
		advance();
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
