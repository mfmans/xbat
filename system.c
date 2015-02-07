/*
	[xbat]   /system.c , 150206   (C) 2005-2015 MF
*/


#include "xbat.h"



/* 用于保存环境变量的缓冲区大小 */
#define ENV_BUFFER_SIZE		32768



/* 由格式化串创建字符串, 调用者释放内存 */
static char * xBuildFormatString (const char *format, va_list argument)
{
	char *	message;

	int	length	= vsnprintf (NULL, 0, format, argument);

	if((message = (char *) calloc (length + 1, sizeof (char))) == NULL)
		return NULL;

	vsprintf (message, format, argument);

	return message;
}




static char * xEnvRead (const char *name)
{
	char *	buffer	= (char *) calloc (ENV_BUFFER_SIZE, sizeof (char));

	if(buffer == NULL)
		return NULL;

	if(GetEnvironmentVariableA (name, buffer, ENV_BUFFER_SIZE) == 0)
	{
		free (buffer);

		return (char *) calloc (1, sizeof (wchar_t));
	}

	return buffer;
}


char * xEnvReadString (const char *name)
{
	return xEnvRead (name);
}


int xEnvReadInt (const char *name)
{
	char *	text	= xEnvRead (name);
	int		value	= 0;

	if(text == NULL)
		return 0;

	value	= (int) strtol (text, NULL, 0);

	free (text);

	return value;
}


unsigned int xEnvReadUint (const char *name)
{
	char *			text	= xEnvRead (name);
	unsigned int	value	= 0;

	if(text == NULL)
		return 0;

	value	= (unsigned int) strtoul (text, NULL, 0);

	free (text);

	return value;
}


int xEnvWrite (const char *name, const char *format, ...)
{
	int		result;
	char *	message;

	va_list	argument;

	va_start (argument, format);

	if((message = xBuildFormatString (format, argument)) == NULL)
		result	= 0;
	else
	{
		if(SetEnvironmentVariableA (name, message) == FALSE)
			result	= 0;
		else
			result	= 1;

		free (message);
	}

	va_end (argument);

	return result;
}




void xError (const char *format, ...)
{
	va_list	argument;

	/* 错误消息 */
	char *	message;
	/* 错误模式 */
	int		mode	= xEnvReadInt (X_CONFIG_ERROR_MODE);

	va_start (argument, format);

	if((message = xBuildFormatString (format, argument)) == NULL)
		message	= "应用程序遇到未知错误。";

	va_end (argument);

	switch(mode)
	{
		/* 命令提示符 */
		case 0:		fprintf (stderr, "%s\n", message);							break;
		/* 窗体 */
		case 1:		MessageBoxA (NULL, message, "", MB_ICONERROR | MB_OK);		break;
	}

	exit (-1);
}


