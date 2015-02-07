/*
	[xbat]   /xbat.h , 150205   (C) 2005-2015 MF
*/


#ifndef _X_H_
#define _X_H_


#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <locale.h>
#include <Windows.h>

#include "system.h"
#include "token.h"
#include "command.h"
#include "lexer.h"
#include "parser.h"




/* ���仺������С */
#define X_CONFIG_BUFFER_SIZE		32

/* ����ģʽ (0=������, 1=����, ����=����) */
#define X_CONFIG_ERROR_MODE			"__xbat_error_mode"
/* ����ģʽ (0=�ر�, ����=����) */
#define X_CONFIG_DEBUG_MODE			"__xbat_error_debug"




#endif   /* _X_H_ */