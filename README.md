# xbat
通过 xbat，您可以在 Windows BAT 文件中方便的调用 DLL 函数的功能。<br />
You can call a DLL function easily in Windows BAT file by XBAT.

xbat 实现了一组语法。您可以指定 DLL 文件、函数名、调用规范、参数，支持简易的数组，可以传递参数和结果到环境变量、应用程序退出代码。<br />
XBAT implements its syntax. You can specify a DLL file, a function, calling convention, arguments. Simple array is supported. You can pass any argument or return value to an environment variable or program exit code.

xbat 使用 re2c + lemon 实现词法和语法分析。源代码使用 C 语言编写，内联部分汇编。<br />
RE2C and Lemon are used as lexer and parser in XBAT. This program was written in C (mainly) and assembler language (partly).


### 用法 Usage
*   构建工程，生成 !.exe，并将其放在合适的地方。<br />
Build this project and put the executable file 1.exe into somewhere proper.
*   编写 BAT 文件，执行之。<br />
Write your own BAT file and execute it.


### 语法 Syntax
    !  winapi kernel32.dll!GetCommandLineA () ~0 (uint) $message;     ^
       user32.dll!MessageBoxA (0, $message, [65 : 2, 0x42, 0x4443, 0L], 0x10) ~4 $;

**!**
代表 !.exe，即 xbat 主程序文件名。
This means "!.exe". It is the filename of XBAT main program.

**stdcall**
指定调用方式，支持 cdecl、stdcall（默认）、winapi（等价于 stdcall）。
This specify the calling convention. You can choose "cdecl", "stdcall" (default), "winapi" (means stdcall).

**kernel32.dll!GetCommandLineA**
指定调用 kernel32.dll 中的 GetCommandLineA。
This specify what you are calling is "GetCommandLineA" in "kernel32.dll".

**~0 (uint) $message**
将第 0 个参数作为无符号整数导出到变量 message，第 0 个参数是函数返回值。支持类型 (uint) (int) (string)，默认为字符串类型。
This export argument #0 to variable message as an unsigned integer. Argument #0 is return value. "(uint)", "(int)", "(string)" are supported. "(string)" is default.

**[65 : 2, 0x42, 0x4443, 0L]**
生成数组：
This generate an array
[0x41, 0x41, 0x42, 0x43, 0x44, 0x00, 0x00, 0x00, 0x00]

**~4 $**
导出参数 4（0x10）的值到变量 $，变量 $ 代表应用程序退出代码。
This export argument #4 (0x10) to variable $. Variable $ is exit code.
