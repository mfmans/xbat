/*
	[xbat]   /command.c , 150206   (C) 2005-2015 MF
*/


#include "xbat.h"
#include "syntax/parser.lemon.h_"



#define stricmp	_stricmp



void xCommandBegin (xCommandStruct *command)
{
	unsigned int	public_count	= command->public_count;
	int				public_return	= command->public_return;

	memset (command, 0, sizeof (xCommandStruct));

	command->public_count	= public_count + 1;
	command->public_return	= public_return;
}


void xCommandEnd (xCommandStruct *command)
{
	free (command->function_name);

	while(command->argument_list != NULL)
	{
		xCommandArgumentStruct *	next	= command->argument_list->next;

		if(command->argument_list->free != 0)					/* !!! 此处不释放数组型参数 */
			free ((void *) command->argument_list->value);

		free (command->argument_list);

		command->argument_list	= next;
	}
}




void xCommandConvention (xCommandStruct *command, xTokenStruct *convention)
{
	char *	text	= convention->value.p_char;

	if(* text == '_')	text ++;
	if(* text == '_')	text ++;

	if(stricmp (text, "cdecl") == 0)
		command->argument_free	= 1;
	else if(stricmp (text, "stdcall") == 0)
		command->argument_free	= 0;
	else if(stricmp (text, "winapi") == 0)
		command->argument_free	= 0;
	else
		xError ("指定的函数调用规范 \"%s\" 无效或不被支持。", convention->value.p_char);
}


static __declspec(naked) int __stdcall xCommandCallInvoke (void *command, unsigned int argument_size)
{
	__asm
	{
		push	ebp
		mov		ebp,	esp								; [ebp + 0]		= old ebp
														; [ebp + 4]		= return address
														; [ebp + 8]		= $command
														; [ebp + 12]	= $argument_size


		mov		ebx,	[ebp + 8]						; ebx	= $command
		mov		edx,	[ebx + 16]						; edx	= $argument	= $command->argument_list


label_argument:
		test	edx,	edx
		jz		label_call								; if($argument == NULL)	goto call

		mov		eax,	[edx + 4]						; eax	= $argument_size	= $argument->size

		test	eax,	4
		jnz		label_argument_push_dword				; 压栈双字长参数
		test	eax,	2
		jnz		label_argument_push_word				; 压栈单字长参数
		jmp		label_argument_next

label_argument_push_dword:
		push	dword ptr [edx]
		jmp		label_argument_next
label_argument_push_word:
		push	word ptr [edx]
		jmp		label_argument_next
label_argument_next:
		mov		edx,	[edx + 12]
		jmp		label_argument


label_call:
		push	label_return							; 压栈返回地址

		mov		eax,	[ebx + 12]						; eax	= $command->function_address
		jmp		eax										; 调用目标函数


label_return:
		add		esp,	dword ptr [ebp + 12]			; 平衡堆栈 $argument_size, 回收参数 (cdecl)

		mov		ebx,	[ebp + 8]
		mov		dword ptr [ebx + 28],	eax				; 保存调用结果到 $argument->result_return_value

		call	GetLastError							; GetLastError ( )

		mov		ebx,	[ebp + 8]
		mov		dword ptr [ebx + 32],	eax				; 保存错误信息到 $argument->result_last_error

		xor		eax,	eax								; 默认返回 0

		cmp		ebp,	esp								; 检测调用前后的 esp 是否相同
		jz		label_end

		mov		esp,	ebp								; 检测失败, 纠正 esp 以免程序崩溃
		mov		eax,	1								; 检测失败, 返回 1

label_end:
		pop		ebp
		ret		8
	}
}


void xCommandCall (xCommandStruct *command)
{
	unsigned long	exception	= 0;

	unsigned int	argument_size	= 0;

	xCommandArgumentStruct *	argument	= command->argument_list;

	/* 只有当需要调用者回收参数时才计算参数大小 */
	if(command->argument_free != 0)
	{
		while(argument != NULL)
		{
			argument_size	+= argument->size;

			argument	= argument->next;
		}
	}

	__try
	{
		if(xCommandCallInvoke (command, argument_size) == 1)
			xError ("%s : 函数返回后堆栈不平衡，这可能是调用规范或参数错误导致的。", command->function_name);
	}
	__except(exception = GetExceptionCode ( ), 1)
	{
		xError ("%s : 函数执行时发生异常 (0x%08X)。", command->function_name, exception);
	}
}


void xCommandCallee (xCommandStruct *command, xTokenStruct *filename, xTokenStruct *function)
{
	HMODULE	module;

	command->function_name	= (char *) calloc (filename->size_actual + function->size_actual + 8, sizeof (char));

	if(xTokenFlagGetWchar (filename))
	{
		if(command->function_name == NULL)
			xError ("%ls!%s : 创建函数名称副本失败。", filename->value.p_wchar, function->value.p_char);

		sprintf (command->function_name, "%ls!%s", filename->value.p_wchar, function->value.p_char);

		module	= LoadLibraryW (filename->value.p_wchar);
	}
	else
	{
		if(command->function_name == NULL)
			xError ("%s!%s : 创建函数名称副本失败。", filename->value.p_char, function->value.p_char);

		sprintf (command->function_name, "%s!%s", filename->value.p_char, function->value.p_char);

		module	= LoadLibraryA (filename->value.p_char);
	}

	if(module == NULL)
		xError ("%s : 加载动态链接库文件失败。", command->function_name);

	if((command->function_address = (void *) GetProcAddress (module, function->value.p_char)) == NULL)
		xError ("%s : 查找函数入口地址失败。", command->function_name);
}




/* 创建一个调用参数 */
static xCommandArgumentStruct * xCommandArgument (xCommandStruct *command)
{
	xCommandArgumentStruct *	argument	= (xCommandArgumentStruct *) calloc (1, sizeof (xCommandArgumentStruct));

	if(argument == NULL)
		xError ("%s : 为函数创建参数 #%u 失败。", command->function_name, command->argument_count + 1);

	argument->next	= command->argument_list;

	command->argument_list	= argument;

	command->argument_count ++;

	return argument;
}


void xCommandArgumentVariable (xCommandStruct *command, xTokenStruct *name, xTokenStruct *cast)
{
	xCommandArgumentStruct *	argument	= xCommandArgument (command);

	char *	var		= name->value.p_char;

	/* 强制类型转换, 默认为字符串型 */
	if(cast != NULL)
	{
		char *	type	= cast->value.p_char;

		if(strcmp (type, "int32") == 0)
		{
			argument->value	= (uintptr_t) ((int32_t) xEnvReadInt (var));
			argument->size	= 4;
		}
		else if(strcmp (type, "int16") == 0)
		{
			argument->value	= (uintptr_t) ((int16_t) xEnvReadInt (var));
			argument->size	= 2;
		}
		else if(strcmp (type, "uint32") == 0)
		{
			argument->value	= (uintptr_t) ((uint32_t) xEnvReadUint (var));
			argument->size	= 4;
		}
		else if(strcmp (type, "uint16") == 0)
		{
			argument->value	= (uintptr_t) ((uint16_t) xEnvReadUint (var));
			argument->size	= 2;
		}
		else if(strcmp (type, "string") == 0)
			goto convert_string;
		else
			xError ("%s : 为函数创建参数 #%u 失败，为环境变量 \"%s\" 指定了无效的类型 \"%s\"。", command->function_name, command->argument_count, var, type);

		return;
	}

convert_string:
	argument->value	= (uintptr_t) xEnvReadString (var);
	argument->free	= 1;
	argument->size	= sizeof (uintptr_t);

	if(argument->value == 0)
		xError ("%s : 为函数创建参数 #%u 失败，无法为环境变量 \"%s\" 的值创建副本。", command->function_name, command->argument_count, var);
}


void xCommandArgumentString (xCommandStruct *command, xTokenStruct *value)
{
	int	ratio;

	xCommandArgumentStruct *	argument	= xCommandArgument (command);

	if(xTokenFlagSetWchar (value))
		ratio	= sizeof (wchar_t);
	else
		ratio	= sizeof (char);

	if((argument->value = (uintptr_t) calloc (value->size_actual + 1, ratio)) == 0)
		xError ("%s : 为函数参数 #%u 创建字符串副本失败。", command->function_name, command->argument_count);

	memcpy ((void *) argument->value, value->value.p_void, value->size_actual * ratio);

	argument->free	= 1;
	argument->size	= sizeof (uintptr_t);
}


void xCommandArgumentInteger (xCommandStruct *command, xTokenStruct *value)
{
	xCommandArgumentStruct *	argument	= xCommandArgument (command);

	/* 使用对齐大小而非实际大小 */
	argument->size	= value->size_align;

	argument->value	= (uintptr_t) value->value.v_uint;
}


void xCommandArgumentArrayBegin (xCommandStruct *command)
{
	if((command->buffer = malloc (sizeof (char))) == NULL)
		xError ("%s : 为函数参数 #%u 创建动态数组失败。", command->function_name, command->argument_count + 1);

	command->buffer_size	= 0;
}


void xCommandArgumentArrayEnd (xCommandStruct *command)
{
	xCommandArgumentStruct *	argument	= xCommandArgument (command);

	argument->size	= sizeof (uintptr_t);
	argument->value	= (uintptr_t) command->buffer;
}


void xCommandArgumentArrayAdd (xCommandStruct *command, xTokenStruct *value, xTokenStruct *duplicate)
{
	unsigned int	i;
	unsigned int	count	= 1;

	if(duplicate != NULL)
		count	= duplicate->value.v_uint;

	for(i = 0; i < count; i ++)
	{
		if((command->buffer = realloc (command->buffer, command->buffer_size + value->size_actual)) == NULL)
			xError ("%s : 向函数参数 #%u 添加数组数据失败。", command->function_name, command->argument_count + 1);

		/* 添加数据到数组尾部 */
		memcpy (((char *) command->buffer) + command->buffer_size, & value->value, value->size_actual);

		command->buffer_size	+= value->size_actual;
	}
}




void xCommandAssign (xCommandStruct *command, xTokenStruct *name, xTokenStruct *offset, xTokenStruct *cast)
{
	uintptr_t	value;

	int		result	= 0;
	char *	var		= name->value.p_char;

	if(offset == NULL)
	{
		/* "~ $var" 导出 GetLastError ( ) 结果 */
		value	= command->result_last_error;
	}
	else
	{
		unsigned int	index	= offset->value.v_uint;

		if(index == 0)
			/* "~0 $var" 导出函数返回值 */
			value	= command->result_return_value;
		else
		{
			unsigned int	i	= command->argument_count;

			xCommandArgumentStruct *	argument	= command->argument_list;

			if(index > command->argument_count)
				xError ("%s : 导出参数 #%u 失败，参数不存在。", command->function_name, index);

			/* 由于参数顺序是从右向左保存, 查找时需要逆序查找 */
			while(i > index)
			{
				if((argument = argument->next) == NULL)
					goto failed;

				i --;
			}

			value	= argument->value;
		}
	}

	if(cast == NULL)
	{
		/* 变量 "$" 表示应用程序返回值 */
		if(* var == '\0')
		{
			command->public_return	= value;

			result	= 1;
		}
		else
			result	= xEnvWrite (var, "%d", value);
	}
	else
	{
		char *	type	= cast->value.p_char;

		if(* var == '\0')
			xError ("%s : 不能转换返回值的类型。", command->function_name);

		__try
		{
			if(strcmp (type, "int") == 0)
				result	= xEnvWrite (var, "%d", value);
			else if(strcmp (type, "uint") == 0)
				result	= xEnvWrite (var, "%u", value);
			else if(strcmp (type, "string") == 0)
				result	= xEnvWrite (var, "%s", value);
			else if(strcmp (type, "wstring") == 0)
				result	= xEnvWrite (var, "%ls", value);
		}
		__except (1)
		{
			result	= 0;
		}
	}

	if(result == 0)
		goto failed;

	return;

failed:
	xError ("%s : 导出环境变量 \"%s\" 失败。", command->function_name, var);
}


