/*
	[xbat]   /parser.h , 150206   (C) 2005-2015 MF
*/


#ifndef _X_PARSER_H_
#define _X_PARSER_H_



typedef void	xParserStruct;




/* �����﷨������ */
xParserStruct *	xParserAlloc	(void);

/* �ͷ��﷨������ */
void	xParserFree	(xParserStruct *parser);


/* �����﷨������ */
void	xParserCall	(xParserStruct *parser, xTokenStruct *token, xCommandStruct *command);



#endif   /* _X_PARSER_H_ */