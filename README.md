# xbat
通过 xbat，您可以在 Windows BAT 文件中方便的调用 DLL 函数的功能。
You can call a DLL function easily in Windows BAT file by xbat.

xbat 实现了一组语法。您可以指定 DLL 文件、函数名、调用规范、参数，支持简易的数组，可以传递参数和结果到环境变量、应用程序退出代码。
XBAT implements its syntax. You can specify a DLL file, a function, calling convention, arguments. Simple array is supported. You can pass any argument or return value to an environment variable or program exit code.

xbat 使用 re2c + lemon 实现词法和语法分析。源代码使用 C 语言编写，内联部分汇编。
RE2C and Lemon are used as lexer and parser in xbat. This program was written in C (mainly) and assembler language (partly).
