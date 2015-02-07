/*
	[xbat]   /system.h , 150206   (C) 2005-2015 MF
*/


#ifndef _X_SYSTEM_H_
#define _X_SYSTEM_H_



/* 获取环境变量 */
char *			xEnvReadString	(const char *name);
int				xEnvReadInt		(const char *name);
unsigned int	xEnvReadUint	(const char *name);

/* 设置环境变量 */
int		xEnvWrite	(const char *name, const char *format, ...);



void	xError	(const char *format, ...);



#endif   /* _X_SYSTEM_H_ */