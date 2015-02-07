/*
	[xbat]   /parser.c , 150206   (C) 2005-2015 MF
*/


#include "xbat.h"



#undef	assert
#define	assert(expr)	do {	if((int) (expr) == 0)	xError ("应用程序遇到未知错误。");	} while(0)

#define	NDEBUG

#include "syntax/parser.lemon.c_"




xParserStruct * xParserAlloc (void)
{
	return (xParserStruct *) xParserInternalAlloc (malloc);
}


void xParserFree (xParserStruct *parser)
{
	if(parser != NULL)
		xParserInternalFree (parser, free);
}




void xParserCall (xParserStruct *parser, xTokenStruct *token, xCommandStruct *command)
{
	if(token == NULL)
		xParserInternal (parser, 0,           NULL,  command);
	else
		xParserInternal (parser, token->type, token, command);
}


