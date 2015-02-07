/*
	[xbat]   /token.h , 150205   (C) 2005-2015 MF
*/


#ifndef _X_TOKEN_H_
#define _X_TOKEN_H_



typedef struct {
	char *	text;					/* 文本 (在 lexer.c 中填充) */

	int		type;					/* 类型 */
	int		flag;					/* 标志 */

	unsigned int	reserved;		/* 保留大小 */

	unsigned int	size_align;		/* 对齐大小, 用于参数压栈 */
	unsigned int	size_actual;	/* 实际大小, 用于数组 */

	union {
		void		* p_void;
		char		* p_char;
		wchar_t		* p_wchar;
		int32_t		  v_int;
		uint32_t	  v_uint;
	} value;
} xTokenStruct;




/* 标志信息 */
#define X_TOKEN_FLAG_FREE			0x01			/* 需要释放内存 */
#define X_TOKEN_FLAG_WCHAR			0x02			/* 宽字节 */




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