/*
	[xbat]   /lexer.c , 150205   (C) 2005-2015 MF
*/


#include "xbat.h"
#include "syntax/lexer.re2c.h_"



xLexerStruct * xLexerAlloc (const char *command, const char *program)
{
	xLexerStruct *	lexer;
	size_t	length;

	if(xEnvReadInt (X_CONFIG_DEBUG_MODE) != 0)
		MessageBoxA (NULL, command, program, 0);

	command	= strstr (command, program);

	if(command == NULL)
		return NULL;

	command	+= strlen (program);

	if(* command == '"')
		command ++;

	length	= strlen (command) + 32;

	if((lexer = (xLexerStruct *) calloc (1, sizeof (xLexerStruct))) == NULL)
		return NULL;
	if((lexer->begin = (char *) calloc (length, sizeof (char))) == NULL)
		return NULL;

	strcpy (lexer->begin, command);

	lexer->end	= lexer->begin + length;
	lexer->from	= lexer->begin;
	lexer->to	= lexer->begin;

	lexer->condition	= yycPUBLIC;			/* initial condition */

	return lexer;
}


void xLexerFree (xLexerStruct *lexer)
{
	if(lexer == NULL)
		return;

	free (lexer->begin);
	free (lexer);
}




#undef	BEGIN
#undef	BUILD
#undef	ALLOC
#undef	MERGE
#undef	WHITE
#undef	ERROR				/* 宏重复定义, 上一次定义在 wingdi.h */

/* 实现部分 */
#define BEGIN(c)			lexer->condition	= yyc##c;
#define BUILD(v)			callable	= xTokenBuild##v;		goto do_build;
#define ALLOC(v)			callable	= xTokenAlloc##v;		goto do_alloc;
#define MERGE(v)			callable	= xTokenMerge##v;		goto do_merge;
#define WHITE()				goto do_white;
#define ERROR()				goto do_error;

/* 生成部分 */
#define YYCTYPE				char
#define YYLIMIT				lexer->end
#define YYCURSOR			lexer->to
#define YYMARKER			lexer->maker
#define YYGETCONDITION()	lexer->condition
#define YYFILL(n)


xTokenStruct * xLexerCall (xLexerStruct *lexer, xCommandStruct *command)
{
	xTokenStruct *	token	= NULL;
	xTokenStruct *	(* callable)(char *, char *, xTokenStruct *)	= NULL;


start:
	if(* lexer->to == '\0')
		return NULL;

	lexer->from	= lexer->to;

	#include "syntax/lexer.re2c.c_"


/* 识别到一个完整的单词 */
do_build:
	{
		/* 单词长度 */
		size_t	length	= lexer->to - lexer->from;
		
		/* 单词副本 */
		char *	begin	= (char *) calloc (length + 1, sizeof (char));
		char *	end		= begin + length;

		if(begin == NULL)
			return NULL;

		memcpy (begin, lexer->from, length);

		token	= callable (begin, end, token);

		free (begin);

		if(token != NULL)
			token->text	= lexer->from;

		return token;
	}


/* 识别到某个单词的开始标志 */
do_alloc:
	{
		if(token = callable (NULL, NULL, token))
			token->text	= lexer->from;
		else
			return NULL;

		goto start;
	}


/* 识别到某个单词的中间部分 */
do_merge:
	{
		if((token = callable (lexer->from, lexer->to, token)) == NULL)
			return NULL;

		goto start;
	}


/* 识别到空白 */
do_white:
	goto start;


/* 遇到错误 */
do_error:
	xError ("命令 #%u : 词法错误，无法识别 \"%.30s\"。", command->public_count, lexer->from);
	
	return NULL;
}


