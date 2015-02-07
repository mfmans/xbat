/*
	[xbat]   /token.c , 150206   (C) 2005-2015 MF
*/


#include "xbat.h"
#include "syntax/parser.lemon.h_"



/* 向 token 中保存字符串副本 */
static xTokenStruct * xTokenClone (xTokenStruct *token, const char *begin, const char *end)
{
	unsigned int	size	= end - begin;

	if(token == NULL)
		return NULL;

	if((token->value.p_void = calloc (size + 1, sizeof (char))) == NULL)
		return NULL;

	token->size_actual	= size;

	memcpy (token->value.p_void, begin, size);

	xTokenFlagSetFree (token);

	return token;
}


/* 向 token 中指定的缓冲区保存内容 */
static xTokenStruct * xTokenMerge (xTokenStruct *token, const void *p, unsigned int size)
{
	if(token == NULL)
		return NULL;

	if(token->reserved < (size + sizeof (char)))
	{
		token->reserved		= size + X_CONFIG_BUFFER_SIZE;

		token->value.p_void	= realloc (token->value.p_void, token->size_actual + token->reserved);

		if(token->value.p_void == NULL)
			return NULL;
	}

	memcpy (token->value.p_char + token->size_actual, p, size);

	token->reserved		-= size;
	token->size_actual	+= size;

	* (token->value.p_char + token->size_actual)	= '\0';

	return token;
}




xTokenStruct * xTokenNew (int type)
{
	xTokenStruct *	token	= (xTokenStruct *) calloc (1, sizeof (xTokenStruct));

	if(token != NULL)
		token->type	= type;

	return token;
}


void xTokenDelete (xTokenStruct *token)
{
	if(token == NULL)
		return;

	if(xTokenFlagGetFree (token))
		free (token->value.p_void);

	free (token);
}




xTokenStruct * xTokenBuildString (char *begin, char *end, xTokenStruct *token)
{
	if(token->size_actual == 0)
	{
		if((token->value.p_void = calloc (1, sizeof (wchar_t))) == NULL)
			return NULL;
	}
	else
	{
		/* 如果使用 L"", 返回 token 前将字符从 ansi 转换为 unicode */
		if(xTokenFlagGetWchar (token))
		{
			int	length_wchar	= MultiByteToWideChar (CP_ACP, 0, token->value.p_char, -1, NULL, 0);

			if(length_wchar > 0)
			{
				wchar_t *	text_wchar	= (wchar_t *) calloc (length_wchar + 1, sizeof (wchar_t));

				MultiByteToWideChar (CP_ACP, 0, token->value.p_char, -1, text_wchar, length_wchar);

				free (token->value.p_void);

				token->size_actual		= (unsigned int) length_wchar;
				token->value.p_wchar	= text_wchar;
			}
			else
				return NULL;
		}
	}

	xTokenFlagSetFree (token);
	
	return token;
}


xTokenStruct * xTokenBuildOperator (char *begin, char *end, xTokenStruct *token)
{
	switch(* begin)
	{
		case '!':	return xTokenNew (X_TOKEN_EXCLAMATION);
		case '~':	return xTokenNew (X_TOKEN_ASSIGN);
		case ',':	return xTokenNew (X_TOKEN_COMMA);
		case ':':	return xTokenNew (X_TOKEN_COLON);
		case ';':	return xTokenNew (X_TOKEN_SEMICOLON);
		case '(':	return xTokenNew (X_TOKEN_BRACKET_LEFT);
		case ')':	return xTokenNew (X_TOKEN_BRACKET_RIGHT);
		case '[':	return xTokenNew (X_TOKEN_SQUARE_BRACKET_LEFT);
		case ']':	return xTokenNew (X_TOKEN_SQUARE_BRACKET_RIGHT);

		default:	return NULL;
	}
}


xTokenStruct * xTokenBuildInteger (char *begin, char *end, xTokenStruct *token)
{
	token	= xTokenNew (X_TOKEN_INTEGER);

	if(token == NULL)
		return NULL;

	switch(* (end - 1))
	{
		/* 0W, 单字长 */
		case 'W':	token->size_actual	= 2;	token->size_align	= 2;	break;
		/* 0L, 双字长 */
		case 'L':	token->size_actual	= 4;	token->size_align	= 4;	break;
		/* 无后缀, 使用双字长作为对齐大小, 实际大小待后方识别 */
		default:	token->size_actual	= 0;	token->size_align	= 4;	goto convert;
	}

	* (end - 1)	= '\0';

convert:
	if(* begin == '-')
	{
		token->value.v_int	= (int) strtol (begin, NULL, 10);

		if(token->size_actual == 0)
		{
			if(token->value.v_int >= -128)
				token->size_actual	= 1;
			else if(token->value.v_int >= -32767)
				token->size_actual	= 2;
			else
				token->size_actual	= 4;
		}
	}
	else if(* (begin + 1) == 'x')
	{
		token->value.v_uint	= (unsigned int) strtol (begin + 2, NULL, 16);

		if(token->size_actual == 0)
		{
			unsigned int	length	= end - begin;

			if(length < 5)
				token->size_actual	= 1;
			else if(length < 7)
				token->size_actual	= 2;
			else
				token->size_actual	= 4;
		}
	}
	else
	{
		token->value.v_uint	= (unsigned int) strtol (begin, NULL, 10);

		if(token->size_actual == 0)
		{
			if(token->value.v_uint < 0x100)
				token->size_actual	= 1;
			else if(token->value.v_uint < 0x10000)
				token->size_actual	= 2;
			else
				token->size_actual	= 4;
		}
	}

	return token;
}


xTokenStruct * xTokenBuildIdentifier (char *begin, char *end, xTokenStruct *token)
{
	return xTokenClone (xTokenNew (X_TOKEN_IDENTIFIER), begin, end);
}


xTokenStruct * xTokenBuildFilename (char *begin, char *end, xTokenStruct *token)
{
	return xTokenClone (xTokenNew (X_TOKEN_FILENAME), begin, end);
}


xTokenStruct * xTokenBuildVariable (char *begin, char *end, xTokenStruct *token)
{
	return xTokenClone (xTokenNew (X_TOKEN_VARIABLE), begin + 1, end);
}



xTokenStruct * xTokenAllocStringA (char *begin, char *end, xTokenStruct *token)
{
	return xTokenNew (X_TOKEN_STRING);
}


xTokenStruct * xTokenAllocStringW (char *begin, char *end, xTokenStruct *token)
{
	if(token = xTokenNew (X_TOKEN_STRING))
		xTokenFlagSetWchar (token);

	return token;
}



xTokenStruct * xTokenMergeStringEscape (char *begin, char *end, xTokenStruct *token)
{
	char *	text;

	switch(* (begin + 1))
	{
		case '"':	text	= "\"";		break;
		case 'a':	text	= "\a";		break;
		case 'b':	text	= "\b";		break;
		case 'f':	text	= "\f";		break;
		case 'n':	text	= "\n";		break;
		case 'r':	text	= "\r";		break;
		case 't':	text	= "\t";		break;
		case 'v':	text	= "\v";		break;

		default:
			return NULL;
	}

	return xTokenMerge (token, text, 1);
}


xTokenStruct * xTokenMergeStringOctonary (char *begin, char *end, xTokenStruct *token)
{
	char	text	[8]	= {0};
	char	buffer	[4]	= {0};

	memcpy (text, begin + 1, end - begin - 1);

	buffer [0]	= (unsigned int) strtoul (text, NULL, 8);

	return xTokenMerge (token, buffer, 1);
}


xTokenStruct * xTokenMergeStringHexadecimal (char *begin, char *end, xTokenStruct *token)
{
	char	text	[8]	= {0};
	char	buffer	[4]	= {0};

	memcpy (text, begin + 2, end - begin - 2);

	buffer [0]	= (unsigned int) strtoul (text, NULL, 16);

	return xTokenMerge (token, buffer, 1);
}


xTokenStruct * xTokenMergeStringRaw (char *begin, char *end, xTokenStruct *token)
{
	return xTokenMerge (token, begin, 1);
}


