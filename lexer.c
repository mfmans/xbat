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
#undef	ERROR				/* ���ظ�����, ��һ�ζ����� wingdi.h */

/* ʵ�ֲ��� */
#define BEGIN(c)			lexer->condition	= yyc##c;
#define BUILD(v)			callable	= xTokenBuild##v;		goto do_build;
#define ALLOC(v)			callable	= xTokenAlloc##v;		goto do_alloc;
#define MERGE(v)			callable	= xTokenMerge##v;		goto do_merge;
#define WHITE()				goto do_white;
#define ERROR()				goto do_error;

/* ���ɲ��� */
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


/* ʶ��һ�������ĵ��� */
do_build:
	{
		/* ���ʳ��� */
		size_t	length	= lexer->to - lexer->from;
		
		/* ���ʸ��� */
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


/* ʶ��ĳ�����ʵĿ�ʼ��־ */
do_alloc:
	{
		if(token = callable (NULL, NULL, token))
			token->text	= lexer->from;
		else
			return NULL;

		goto start;
	}


/* ʶ��ĳ�����ʵ��м䲿�� */
do_merge:
	{
		if((token = callable (lexer->from, lexer->to, token)) == NULL)
			return NULL;

		goto start;
	}


/* ʶ�𵽿հ� */
do_white:
	goto start;


/* �������� */
do_error:
	xError ("���� #%u : �ʷ������޷�ʶ�� \"%.30s\"��", command->public_count, lexer->from);
	
	return NULL;
}


