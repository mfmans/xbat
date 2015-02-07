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

		if(command->argument_list->free != 0)					/* !!! �˴����ͷ������Ͳ��� */
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
		xError ("ָ���ĺ������ù淶 \"%s\" ��Ч�򲻱�֧�֡�", convention->value.p_char);
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
		jnz		label_argument_push_dword				; ѹջ˫�ֳ�����
		test	eax,	2
		jnz		label_argument_push_word				; ѹջ���ֳ�����
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
		push	label_return							; ѹջ���ص�ַ

		mov		eax,	[ebx + 12]						; eax	= $command->function_address
		jmp		eax										; ����Ŀ�꺯��


label_return:
		add		esp,	dword ptr [ebp + 12]			; ƽ���ջ $argument_size, ���ղ��� (cdecl)

		mov		ebx,	[ebp + 8]
		mov		dword ptr [ebx + 28],	eax				; ������ý���� $argument->result_return_value

		call	GetLastError							; GetLastError ( )

		mov		ebx,	[ebp + 8]
		mov		dword ptr [ebx + 32],	eax				; ���������Ϣ�� $argument->result_last_error

		xor		eax,	eax								; Ĭ�Ϸ��� 0

		cmp		ebp,	esp								; ������ǰ��� esp �Ƿ���ͬ
		jz		label_end

		mov		esp,	ebp								; ���ʧ��, ���� esp ����������
		mov		eax,	1								; ���ʧ��, ���� 1

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

	/* ֻ�е���Ҫ�����߻��ղ���ʱ�ż��������С */
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
			xError ("%s : �������غ��ջ��ƽ�⣬������ǵ��ù淶����������µġ�", command->function_name);
	}
	__except(exception = GetExceptionCode ( ), 1)
	{
		xError ("%s : ����ִ��ʱ�����쳣 (0x%08X)��", command->function_name, exception);
	}
}


void xCommandCallee (xCommandStruct *command, xTokenStruct *filename, xTokenStruct *function)
{
	HMODULE	module;

	command->function_name	= (char *) calloc (filename->size_actual + function->size_actual + 8, sizeof (char));

	if(xTokenFlagGetWchar (filename))
	{
		if(command->function_name == NULL)
			xError ("%ls!%s : �����������Ƹ���ʧ�ܡ�", filename->value.p_wchar, function->value.p_char);

		sprintf (command->function_name, "%ls!%s", filename->value.p_wchar, function->value.p_char);

		module	= LoadLibraryW (filename->value.p_wchar);
	}
	else
	{
		if(command->function_name == NULL)
			xError ("%s!%s : �����������Ƹ���ʧ�ܡ�", filename->value.p_char, function->value.p_char);

		sprintf (command->function_name, "%s!%s", filename->value.p_char, function->value.p_char);

		module	= LoadLibraryA (filename->value.p_char);
	}

	if(module == NULL)
		xError ("%s : ���ض�̬���ӿ��ļ�ʧ�ܡ�", command->function_name);

	if((command->function_address = (void *) GetProcAddress (module, function->value.p_char)) == NULL)
		xError ("%s : ���Һ�����ڵ�ַʧ�ܡ�", command->function_name);
}




/* ����һ�����ò��� */
static xCommandArgumentStruct * xCommandArgument (xCommandStruct *command)
{
	xCommandArgumentStruct *	argument	= (xCommandArgumentStruct *) calloc (1, sizeof (xCommandArgumentStruct));

	if(argument == NULL)
		xError ("%s : Ϊ������������ #%u ʧ�ܡ�", command->function_name, command->argument_count + 1);

	argument->next	= command->argument_list;

	command->argument_list	= argument;

	command->argument_count ++;

	return argument;
}


void xCommandArgumentVariable (xCommandStruct *command, xTokenStruct *name, xTokenStruct *cast)
{
	xCommandArgumentStruct *	argument	= xCommandArgument (command);

	char *	var		= name->value.p_char;

	/* ǿ������ת��, Ĭ��Ϊ�ַ����� */
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
			xError ("%s : Ϊ������������ #%u ʧ�ܣ�Ϊ�������� \"%s\" ָ������Ч������ \"%s\"��", command->function_name, command->argument_count, var, type);

		return;
	}

convert_string:
	argument->value	= (uintptr_t) xEnvReadString (var);
	argument->free	= 1;
	argument->size	= sizeof (uintptr_t);

	if(argument->value == 0)
		xError ("%s : Ϊ������������ #%u ʧ�ܣ��޷�Ϊ�������� \"%s\" ��ֵ����������", command->function_name, command->argument_count, var);
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
		xError ("%s : Ϊ�������� #%u �����ַ�������ʧ�ܡ�", command->function_name, command->argument_count);

	memcpy ((void *) argument->value, value->value.p_void, value->size_actual * ratio);

	argument->free	= 1;
	argument->size	= sizeof (uintptr_t);
}


void xCommandArgumentInteger (xCommandStruct *command, xTokenStruct *value)
{
	xCommandArgumentStruct *	argument	= xCommandArgument (command);

	/* ʹ�ö����С����ʵ�ʴ�С */
	argument->size	= value->size_align;

	argument->value	= (uintptr_t) value->value.v_uint;
}


void xCommandArgumentArrayBegin (xCommandStruct *command)
{
	if((command->buffer = malloc (sizeof (char))) == NULL)
		xError ("%s : Ϊ�������� #%u ������̬����ʧ�ܡ�", command->function_name, command->argument_count + 1);

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
			xError ("%s : �������� #%u �����������ʧ�ܡ�", command->function_name, command->argument_count + 1);

		/* ������ݵ�����β�� */
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
		/* "~ $var" ���� GetLastError ( ) ��� */
		value	= command->result_last_error;
	}
	else
	{
		unsigned int	index	= offset->value.v_uint;

		if(index == 0)
			/* "~0 $var" ������������ֵ */
			value	= command->result_return_value;
		else
		{
			unsigned int	i	= command->argument_count;

			xCommandArgumentStruct *	argument	= command->argument_list;

			if(index > command->argument_count)
				xError ("%s : �������� #%u ʧ�ܣ����������ڡ�", command->function_name, index);

			/* ���ڲ���˳���Ǵ������󱣴�, ����ʱ��Ҫ������� */
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
		/* ���� "$" ��ʾӦ�ó��򷵻�ֵ */
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
			xError ("%s : ����ת������ֵ�����͡�", command->function_name);

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
	xError ("%s : ������������ \"%s\" ʧ�ܡ�", command->function_name, var);
}


