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




/* 分配缓冲区大小 */
#define X_CONFIG_BUFFER_SIZE		32

/* 错误模式 (0=命令行, 1=窗口, 其它=禁用) */
#define X_CONFIG_ERROR_MODE			"__xbat_error_mode"
/* 调试模式 (0=关闭, 其它=开启) */
#define X_CONFIG_DEBUG_MODE			"__xbat_error_debug"




#endif   /* _X_H_ */