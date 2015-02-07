/*
	[xbat]   /lexer.h , 150205   (C) 2005-2015 MF
*/


#ifndef _X_LEXER_H_
#define _X_LEXER_H_



typedef struct {
	char *			maker;				/* YYMARKER */
	enum YYCONDTYPE	condition;			/* YYSETCONDITION, YYGETCONDITION */

	char *	begin;			/* �����ִ���ʼ */
	char *	end;			/* �����ִ���ֹ (β��Ԥ�� 16B �հ�) */

	char *	from;			/* ��ǰ���ʿ�ʼ */
	char *	to;				/* ��ǰ���ʽ��� */
} xLexerStruct;




/* ����ʷ������� */
xLexerStruct *	xLexerAlloc	(const char *command, const char *program);

/* �ͷŴʷ������� */
void	xLexerFree	(xLexerStruct *lexer);


/* ���ôʷ�������, ����һ�� TOKEN */
xTokenStruct *	xLexerCall	(xLexerStruct *lexer, xCommandStruct *command);



#endif   /* _X_LEXER_H_ */