# XBAT
通过 XBAT，您可以在 Windows BAT 文件中方便的调用 DLL 函数的功能。<br />
You can call a DLL function easily in Windows BAT file by XBAT.

XBAT 实现了一组语法。您可以指定 DLL 文件、函数名、调用规范、参数，支持简易的数组，可以传递参数和结果到环境变量、应用程序退出代码。<br />
XBAT implements its syntax. You can specify a DLL file, a function, calling convention and arguments. Simple array is supported. You can pass any argument and return value to an environment variable or program exit code.

XBAT 使用 RE2C + Lemon 实现词法和语法分析。源代码使用 C 语言编写，内联部分汇编。<br />
RE2C and Lemon are used as lexer and parser in XBAT. XBAT was written in C (mainly) and assembler language (partly).


## 用法
*   构建工程，生成 !.exe，并将其放在合适的地方。
*   编写 BAT 文件。请注意将 BAT 文件放置在一个可以执行 !.exe 的环境中。


## 示例

    !  kernel32.dll!GetCommandLineA () ~0 (uint) $message;     ^
       user32.dll!MessageBoxA (0, (uint32) $message, [65 : 2, 0x42, 0x4443, 0L], 0x10) ~4 $;     ^
       __cdecl msvcr100.dll!printf ($message);


**!**<br />
即为 "!.exe"。您可能需要使用 "@!" 或 "@echo off" 抑制 STDOUT 输出。

**kernel32.dll!GetCommandLineA () ~0 (uint) $message;**<br />
调用 kernel32.dll 中的 GetCommandLineA 函数，0 个参数，使用默认的调用规范。执行完毕后，将第 0 个参数（即函数返回值）使用无符号整数类型表示，并导出到环境变量 *message* 中。

**user32.dll!MessageBoxA (0, (uint32) $message, [65 : 2, 0x42, 0x4443, 0L], 0x10) ~4 $;**<br />
调用 user32.dll 中的 MessageBoxA 函数，4 个参数，使用默认的调用规范。第 1 个参数为 32 位整数 0；第 2 个参数为环境变量 *message* 的值，这个值在使用前先被转换为 32 位无符号整数；第 3 个参数是一个数组 [0x41, 0x41, 0x42, 0x43, 0x44, 0x00, 0x00, 0x00, 0x00]；第 4 个参数是一个 32 位整数值 0x10。执行完毕后，将第 4 个参数（即 0x10）设置为应用程序退出代码，结束后可用 %errorlevel% 取得。

**__cdecl msvcr100.dll!printf ($message);**<br />
调用 msvcr100.dll 中的 printf 函数，1 个参数，使用 __cdecl 调用规范。第 1 个参数为环境变量 *message* 的值。


## 语法

    ! [winapi|cdecl|stdcall]{1} file_name{2}!function_name{3} ([argument_1 [, argument_2[, ...]]]{4})  [assign_1[, assign_2[, ...]]]{5} ;{6}


{1} 部分为调用规范，可省略。可设置为 "winapi"、"\_winapi"、"\_\_winapi"、"cdecl"、"\_cdecl"、"\_\_cdecl"、"stdcall"、"\_stdcall"、"\_\_stdcall"，不区分大小写。默认为 stdcall。其中 winapi 为 stdcall 的别名。

{2} 部分为文件名，如果文件名符合 *[0-9a-zA-Z\_]+"."('dll'|'exe')*，可以省去引号，否则需要引号包括。

{3} 部分为函数名，函数名需要符合 *[a-zA-Z\_]+[0-9a-zA-Z\_]\**。

{4} 部分为参数列表，允许 0 个参数。支持以下类型的参数：

*   整数，如："-5" 和 "-5L" 表示 32 位带符号整数，"-5W" 表示 16 位带符号整数；"1000" 和 "1000L" 表示 32 位无符号整数, "1000W" 表示 16 位无符号整数；"0xFF" 和 "0xFFL" 表示 32 位无符号整数，"0xFFW" 表示 16 位无符号整数。尾缀 "L" 代表 "Long"，"W" 代表 "Word"。
*   字符串，支持如 "我是 AN\x53\111 字符串\n" 和 L"我是 Unicode 字符串"。
*   数组，使用 [value_1, value_2 : count_2] 方式定义，其中 value 和 count 都必须是整数。与直接作为参数的整数不同，作为 value 的整数如果不带尾缀，长度将自动识别，如 0xFF 被识别为 8 位无符号整数。
*   变量，使用 "$var" 表示，可加上强制类型转换运算符如 "(uint32) $var"。支持的强制类型转换运算符有 "(int32)"、"(int16)"、"(uint32)"、"(uint16)"、"(string)"，可参见下方关于变量的详细叙述。

{5} 部分为赋值列表，可省略。

*   "~ (type) $var"，将 GetLastError ( ) 返回值转换为 type 类型并导出到环境变量 *var*。
*   "~x (type) $var"，将第 x 个参数的值转换为 type 类型并导出到环境变量 *var*。其中 x 是一个整数，不能大于参数个数，0 表示函数返回值。

以上格式中，"(type)" 可选，允许设置为 "(int)"、"(uint)"、"(string)"、"(wstring)"，可参见下方关于变量的详细叙述。如果导出的变量为 "$"，代表设置应用程序退出代码，此时不允许使用强制类型转换。

{6} 部分为半角分号，表示本次调用结束，**不允许省略**。

#### 关于变量：

所谓变量，是指环境变量，**存储的是字符串类型的数据**。读取变量时，如果不进行强制类型转换，直接使用字符串值作为参数，否则将依据字符串值进行相应转换。导出变量时，如果不进行强制类型转换，将会把需要导出的参数值视为带符号整数，使用 "%d" 格式输出，否则视情况使用 "%u"、"%d"、"%s"、"%ls" 进行输出。

注意，XBAT 一般是作为 cmd.exe 的子进程进行执行，运行时继承父进程的环境变量，不能改变父进程的环境变量。
