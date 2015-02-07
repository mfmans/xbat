/*
	[xbat]   /command.h , 150206   (C) 2005-2015 MF
*/


#ifndef _X_COMMAND_H_
#define _X_COMMAND_H_



/* 调用参数 */
typedef struct xCommandArgumentStruct {
	uintptr_t	value;				/* 实际用于压栈的值 */
	uintptr_t	size;				/* 压栈的值的大小 (用于指定双字/字) */
	uintptr_t	free;				/* 压栈的值是否为动态分配的内存, 需要释放 */

	struct xCommandArgumentStruct *	next;
} xCommandArgumentStruct;


typedef struct {
	unsigned int	public_count;				/* 已处理命令数 */
	int				public_return;				/* main ( ) 返回值 */

	char *	function_name;						/* 函数名 */
	void *	function_address;					/* 函数地址 */

	xCommandArgumentStruct *	argument_list;	/* 参数表 */
	int				argument_free;				/* 需要调用者释放参数 */
	unsigned int	argument_count;				/* 参数个数 */

	uintptr_t	result_return_value;			/* 返回值 */
	uintptr_t	result_last_error;				/* 最后错误值 */

	void *			buffer;						/* 缓冲区 */
	unsigned int	buffer_size;				/* 缓冲区大小 */
} xCommandStruct;




void	xCommandBegin	(xCommandStruct *command);
void	xCommandEnd		(xCommandStruct *command);

/* 设置调用规范 */
void	xCommandConvention	(xCommandStruct *command, xTokenStruct *convention);
/* 调用函数 */
void	xCommandCall		(xCommandStruct *command);
/* 设置调用目标 */
void	xCommandCallee		(xCommandStruct *command, xTokenStruct *filename, xTokenStruct *function);

/* 添加变量型参数 */
void	xCommandArgumentVariable	(xCommandStruct *command, xTokenStruct *name, xTokenStruct *cast);
/* 添加字符串型参数 */
void	xCommandArgumentString		(xCommandStruct *command, xTokenStruct *value);
/* 添加整数型参数 */
void	xCommandArgumentInteger		(xCommandStruct *command, xTokenStruct *value);

/* 数组型参数 */
void	xCommandArgumentArrayBegin	(xCommandStruct *command);
void	xCommandArgumentArrayEnd	(xCommandStruct *command);
void	xCommandArgumentArrayAdd	(xCommandStruct *command, xTokenStruct *value, xTokenStruct *duplicate);

/* 结果赋值 */
void	xCommandAssign		(xCommandStruct *command, xTokenStruct *name, xTokenStruct *offset, xTokenStruct *cast);



#endif   /* _X_COMMAND_H_ */