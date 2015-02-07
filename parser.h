/*
	[xbat]   /parser.h , 150206   (C) 2005-2015 MF
*/


#ifndef _X_PARSER_H_
#define _X_PARSER_H_



typedef void	xParserStruct;




/* 分配语法分析器 */
xParserStruct *	xParserAlloc	(void);

/* 释放语法分析器 */
void	xParserFree	(xParserStruct *parser);


/* 调用语法分析器 */
void	xParserCall	(xParserStruct *parser, xTokenStruct *token, xCommandStruct *command);



#endif   /* _X_PARSER_H_ */