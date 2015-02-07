/*
	[xbat]   /token.h , 150205   (C) 2005-2015 MF
*/


#ifndef _X_TOKEN_H_
#define _X_TOKEN_H_



typedef struct {
	char *	text;					/* �ı� (�� lexer.c �����) */

	int		type;					/* ���� */
	int		flag;					/* ��־ */

	unsigned int	reserved;		/* ������С */

	unsigned int	size_align;		/* �����С, ���ڲ���ѹջ */
	unsigned int	size_actual;	/* ʵ�ʴ�С, �������� */

	union {
		void		* p_void;
		char		* p_char;
		wchar_t		* p_wchar;
		int32_t		  v_int;
		uint32_t	  v_uint;
	} value;
} xTokenStruct;




/* ��־��Ϣ */
#define X_TOKEN_FLAG_FREE			0x01			/* ��Ҫ�ͷ��ڴ� */
#define X_TOKEN_FLAG_WCHAR			0x02			/* ���ֽ� */




#define xTokenFlagGetFree(token)		(token->flag & X_TOKEN_FLAG_FREE)
#define xTokenFlagGetWchar(token)		(token->flag & X_TOKEN_FLAG_WCHAR)

#define xTokenFlagSetFree(token)		token->flag	|= X_TOKEN_FLAG_FREE
#define xTokenFlagSetWchar(token)		token->flag	|= X_TOKEN_FLAG_WCHAR




xTokenStruct *	xTokenNew	(int type);

void	xTokenDelete	(xTokenStruct *token);

xTokenStruct *	xTokenBuildString		(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenBuildOperator		(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenBuildInteger		(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenBuildIdentifier	(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenBuildFilename		(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenBuildVariable		(char *begin, char *end, xTokenStruct *token);

xTokenStruct *	xTokenAllocStringA		(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenAllocStringW		(char *begin, char *end, xTokenStruct *token);

xTokenStruct *	xTokenMergeStringEscape			(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenMergeStringOctonary		(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenMergeStringHexadecimal	(char *begin, char *end, xTokenStruct *token);
xTokenStruct *	xTokenMergeStringRaw			(char *begin, char *end, xTokenStruct *token);



#endif   /* _X_TOKEN_H_ */