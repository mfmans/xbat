@echo off


set	__xbat_error_mode=1


!	user32.dll!MessageBoxTimeoutA (0, "������ʾ����Դ�����رա�", "", 0x41, 0, 2000) ~0 $;


if %errorlevel% EQU 2 ( exit /B )


!	kernel32.dll	!Sleep (500);									^
	user32.dll		!PostMessageA (0xFFFF, 0x0112, 0xF170, 2);

