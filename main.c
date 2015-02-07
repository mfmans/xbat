/*
	[xbat]   /main.c , 150206   (C) 2005-2015 MF
*/


#include "xbat.h"



int main (int argc, char *argv[])
{
	xCommandStruct	command;

	xLexerStruct	* lexer		= xLexerAlloc  (GetCommandLineA ( ), argv [0]);
	xParserStruct	* parser	= xParserAlloc ( );

	command.public_count	= 0;
	command.public_return	= 0;

	setlocale (LC_ALL, "");

	if(lexer && parser)
	{
		xTokenStruct *	token	= NULL;

		while(token = xLexerCall (lexer, & command))
			xParserCall (parser, token, & command);

		xParserCall (parser, NULL, & command);
	}
	else
		command.public_return	= -1;

	xLexerFree  (lexer );
	xParserFree (parser);

	return command.public_return;
}


