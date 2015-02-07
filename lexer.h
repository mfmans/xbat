/*
	[xbat]   /lexer.h , 150205   (C) 2005-2015 MF
*/


#ifndef _X_LEXER_H_
#define _X_LEXER_H_



typedef struct {
	char *			maker;				/* YYMARKER */
	enum YYCONDTYPE	condition;			/* YYSETCONDITION, YYGETCONDITION */

	char *	begin;			/* 完整字串起始 */
	char *	end;			/* 完整字串终止 (尾部预留 16B 空白) */

	char *	from;			/* 当前单词开始 */
	char *	to;				/* 当前单词结束 */
} xLexerStruct;




/* 分配词法分析器 */
xLexerStruct *	xLexerAlloc	(const char *command, const char *program);

/* 释放词法分析器 */
void	xLexerFree	(xLexerStruct *lexer);


/* 调用词法分析器, 返回一个 TOKEN */
xTokenStruct *	xLexerCall	(xLexerStruct *lexer, xCommandStruct *command);



#endif   /* _X_LEXER_H_ */