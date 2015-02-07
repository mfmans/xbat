/*
	[xbat]   /command.h , 150206   (C) 2005-2015 MF
*/


#ifndef _X_COMMAND_H_
#define _X_COMMAND_H_



/* ���ò��� */
typedef struct xCommandArgumentStruct {
	uintptr_t	value;				/* ʵ������ѹջ��ֵ */
	uintptr_t	size;				/* ѹջ��ֵ�Ĵ�С (����ָ��˫��/��) */
	uintptr_t	free;				/* ѹջ��ֵ�Ƿ�Ϊ��̬������ڴ�, ��Ҫ�ͷ� */

	struct xCommandArgumentStruct *	next;
} xCommandArgumentStruct;


typedef struct {
	unsigned int	public_count;				/* �Ѵ��������� */
	int				public_return;				/* main ( ) ����ֵ */

	char *	function_name;						/* ������ */
	void *	function_address;					/* ������ַ */

	xCommandArgumentStruct *	argument_list;	/* ������ */
	int				argument_free;				/* ��Ҫ�������ͷŲ��� */
	unsigned int	argument_count;				/* �������� */

	uintptr_t	result_return_value;			/* ����ֵ */
	uintptr_t	result_last_error;				/* ������ֵ */

	void *			buffer;						/* ������ */
	unsigned int	buffer_size;				/* ��������С */
} xCommandStruct;




void	xCommandBegin	(xCommandStruct *command);
void	xCommandEnd		(xCommandStruct *command);

/* ���õ��ù淶 */
void	xCommandConvention	(xCommandStruct *command, xTokenStruct *convention);
/* ���ú��� */
void	xCommandCall		(xCommandStruct *command);
/* ���õ���Ŀ�� */
void	xCommandCallee		(xCommandStruct *command, xTokenStruct *filename, xTokenStruct *function);

/* ��ӱ����Ͳ��� */
void	xCommandArgumentVariable	(xCommandStruct *command, xTokenStruct *name, xTokenStruct *cast);
/* ����ַ����Ͳ��� */
void	xCommandArgumentString		(xCommandStruct *command, xTokenStruct *value);
/* ��������Ͳ��� */
void	xCommandArgumentInteger		(xCommandStruct *command, xTokenStruct *value);

/* �����Ͳ��� */
void	xCommandArgumentArrayBegin	(xCommandStruct *command);
void	xCommandArgumentArrayEnd	(xCommandStruct *command);
void	xCommandArgumentArrayAdd	(xCommandStruct *command, xTokenStruct *value, xTokenStruct *duplicate);

/* �����ֵ */
void	xCommandAssign		(xCommandStruct *command, xTokenStruct *name, xTokenStruct *offset, xTokenStruct *cast);



#endif   /* _X_COMMAND_H_ */