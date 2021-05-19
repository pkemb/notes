# 汇编语言程序设计

书籍简介：

书籍主页（代码下载）：[Professional Assembly Language](https://www.wiley.com/en-us/Professional+Assembly+Language-p-9780764579011)

# 第一章 什么是汇编语言

## 处理器指令

芯片内部定义的二进制代码。

## 高级语言

high-level language，使用简单的术语创建功能。高级语言分为解释型和编译型。编译型语言需要经过编译和链接之后，才能运行。而解释型语言由解释器读取并解释。相比于汇编语言，高级语言具有可移植性和标准化的特点。

## 汇编语言

为了更高的效率，汇编语言允许程序员直接使用指令码编写程序，汇编语言程序使用助记符表示指令码，汇编器负责将助记符转化为原始的指令码。汇编语言程序由三个部分组成：
* 指令，或称为操作码助记符
* 数据段：如何定义并引用数据
* 命令：伪指令，辅助编程，不产生机器码

# 第二章 IA-32 平台

了解编程的环境，有利于编写汇编语言程序。

## IA-32处理器的核心部分

处理器的主要部件有：
* [控制单元](#控制单元)
* 执行单元
* 寄存器
* 标志

### 控制单元

控制处理器在什么时候进行什么操作，主要工作是实现四个基本功能：
* 从内存获取指令
* 对指令进行解码
* 从内存获取数据
* 写回数据（如有必要）

以下高级特性可加快程序的执行速度：
* 指令预取和解码：指令放置在缓存中，加快获取指令的速度
* 分支预测
* 乱序执行：改变指令的执行顺序
* 退役

### 执行单元

执行单元用于执行指令，由一个或多个运算逻辑单元（ALU）组成。ALU被设计为处理不同数据类型的数学操作。

### 寄存器

处理器内部的存储单元，用于储存需要处理的数据。访问寄存器比访问内存快很多。寄存器分为以下类别：
* 通用寄存器，32位：EAX、EBX、ECX、EDX、EDI、ESI、ESP、EBP
* 段寄存器，16位：CS、DS、SS、ES、FS、GS
* 指令指针寄存器，跟踪要执行的下一条指令：EIP
* 控制寄存器：CR0、CR1、CR2、CR3、CR4

### 标志

即标志寄存器`EFLAGS`，可以用于确定指令是否执行成功。标志可以分为`状态标志`、`控制标志`、`系统标志`。

状态标志，表明处理器进行的数学操作的结果。

| 标志 | 位 | 名称 |
| - | - | - |
| CF | 0 | 进位标志 |
| PF | 2 | 奇偶校验标志 |
| AF | 4 | 辅助进位标志 |
| ZF | 6 | 零标志 |
| SF | 7 | 符号标志 |
| OF | 11 | 溢出标志 |

控制标志，控制处理器的特定行为。
* DF：方向标志，为1时，字符串指令自动递减内存。为0时，字符串指令自动递增内存。

系统标志用于控制操作系统级别的操作。

| 标志 | 位 | 名称 |
| - | - | - |
| TF | 8 | 陷进标志 |
| IF | 9 | 中断使能标志 |
| IOPL | 12和13 | IO特权级别标志 |
| NT | 14 | 嵌套任务标志 |
| RF | 16 | 恢复标志 |
| VM | 17 | 虚拟8086模式标志 |
| AC | 18 | 对准检查标志 |
| VIF | 19 | 虚拟中断标志 |
| VIP | 20 | 虚拟中断挂起标志 |
| ID | 21 | 识别标志 |

## IA-32的高级特性

### x87浮点单元

### 多媒体扩展

### 流化SIMD扩展

Streaming SIMD extension，SSE。

### 超线程

超线程使单一IA-32处理器能够同时处理多个程序执行线程。单个物理处理器包含两个或多个逻辑处理器，每个逻辑处理器拥有寄存器的完整集合，所有的逻辑处理器共享相同的执行单元。

## IA-32处理器系列

主要有Intel系列和AMD系列。

# 第二章（补充） ARMv8 平台

ARMv8引入的AArch64架构，以及后续的ARMv9。

## 参考资料
* [Introducing the Arm architecture](https://developer.arm.com/documentation/102404/latest)
* [AArch64 Exception model](https://developer.arm.com/documentation/102412/latest)
* [AArch64 Instruction Set Architecture](https://developer.arm.com/documentation/102374/latest/)
* [AArch64 memory management](https://developer.arm.com/documentation/101811/latest)
* [AArch64 memory model](https://developer.arm.com/documentation/102376/latest/)
* [Understanding the Armv8.x and Armv9.x extensions](https://developer.arm.com/documentation/102378/latest)
* [AArch64 self-hosted debug](https://developer.arm.com/documentation/102120/0100)
* [AArch64 external debug](https://developer.arm.com/documentation/102196/latest)

# 第三章 相关的工具

介绍创建汇编语言程序所必需的软件工具。

## 开发工具

* 汇编器：把汇编语言源代码转换为处理器的指令码。
* 链接器
* 调试器
* 编译器
* 目标代码反汇编器
* 简档器：确定函数的执行时间

## GNU汇编器

主要使用GNU工具链中的汇编器。

### 安装汇编器

GNU汇编器不在单独的包中发布，和GNU binutils包中的其他开发软件捆绑在一起。

安装方法：
* RedHat系列：yum -y install binutils
* Debian系列：apt install -y binutils
* 编译安装，源代码链接：[binutils](https://ftp.gnu.org/gnu/binutils/)。安装方法略。

### 使用汇编器

* [2.14 pdf](http://www.zap.org.au/elec2041-cdrom/gnutools/doc/gnu-assembler.pdf)
* [2.30 pdf](https://doc.ecoscentric.com/gnutools/doc/as.pdf)
* [2.36 html](https://sourceware.org/binutils/docs-2.36/as/index.html)

### 关于操作码语法

| | AT&T | Intel |
| - | - | - |
| 立即数 | $4 | 4 |
| 寄存器 | %eax | eax |
| 操作数顺序 | mnemonic	source, destination | mnemonic	destination, source |
| 数据长度 | movl $test, %eax | mov eax, dword ptr test |
| 长调用和跳转 | ljmp $section, $offset | jmp section:offset |

参考资料：
* https://csiflabs.cs.ucdavis.edu/~ssdavis/50/att-syntax.htm
* http://web.mit.edu/rhel-doc/3/rhel-as-en-3/i386-syntax.html

## GNU链接器

GNU链接器ld用于把目标代码文件链接位可执行程序文件或库文件。

使用手册：
* [2.30 pdf](https://doc.ecoscentric.com/gnutools/doc/ld.pdf)
* [2.36 html](https://sourceware.org/binutils/docs-2.36/ld/index.html)

## GNU编译器

GNU Compiler Collection，gcc。

### 下载和安装gcc

### 使用gcc

官方手册：https://gcc.gnu.org/onlinedocs/

常用选项：
* -c 只编译或汇编，不链接
* -S 只编译，不汇编
* -E 只预处理，不编译
* -o 指定输出文件名，默认输出文件名为a.out
* -g 生成调试信息
* -pg 生成gprof需要的额外代码
* -O 指定优化等级0/1/2
* -W 设置警告消息级别
* -I 指定include目录
* -L 指定库文件目录
* -l 链接指定库

## GNU调试器程序

gdb。

### 下载和按转GDB

### 使用GDB

官方手册：https://sourceware.org/gdb/current/onlinedocs/gdb.pdf

常用的交互指令：
* break 设置断点
* watch 设置监视点
* info 查看寄存器、堆栈或内存
* x 检查内存位置
* print 显示变量值
* run 开始允许程序
* list 列出指定的函数或行
* next 执行下一条指令，不会进入子函数内部
* step 执行下一条指令，进入子函数内部
* cont 从停止的位置继续执行程序
* until 运行程序，直到到达指定的行

## GNU objdump 程序

参考《程序员的自我修养——链接装载与库》。

官方手册：https://sourceware.org/binutils/docs-2.36/binutils/objdump.html

## GNU简档器程序

gprof，用于分析程序的执行。

官方手册：https://sourceware.org/binutils/docs-2.36/gprof/index.html

## 完整的汇编开发系统

推荐使用Linux操作系统，并安装上述的工具。安装过程略。

# 第4章 汇编语言程序范例

学习GNU汇编器的基本汇编语言程序模板。

## 程序的组成

汇编语言程序由不同的段组成，每个段都有不同的目的。最常见的三个段是：
* 数据段 .data
* bss段 .bss
* 文本段 .text

文本段必须要有，数据段和bss段是可选的。数据段声明带有初始值的数据元素，bss段声明使用0初始化的数据元素。

### 定义段

GNU汇编器使用`.section`命令声明段，只使用一个参数，即段的类型。一般情况下，按照.data、.bss、.text的顺序依次定义。

### 定义起始点

默认情况下，`ld`链接器使用符号`_start`作为程序的起点。也可以使用`-e`参数指定新的起始点。程序入口点需要被外部的程序引用，这使用`.global`命令完成。

汇编语言程序的模板如下：
```asm
.section .data
# initialized data here

.section .bss
# uninitialized data here

.section .text
.global _start  # 声明外部程序可以访问的程序标签
_start:
# instrction code goes here
```

## 创建简单程序

### cpuid 指令

请参考[Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 2](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf)第290页。

### 范例程序

[code/ch4/cpuid.s](code/ch4/cpuid.s)

说明：
* 默认使用`main`作为入口点
* 在`WSL`上使用`gcc`编译失败，需要加上`-no-pie`选项，即`gcc -o cpuid cpuid.s -no-pid`。
* [code/ch4/Makefile](code/ch4/Makefile)默认使用`as`，`make USEGCC=1`则使用`gcc`。

## 调试程序

实操环节，主要掌握以下基本技能：
* 设置断点，break指令
* 运行程序，run和cont指令
* 单步运行指令，next和step指令
* 数据查看指令，info、print和x指令

## 在汇编语言中使用C库函数

程序示例：[code/ch4/cpuid2_x86.s](code/ch4/cpuid2_x86.s)

注意：
* 第5行是`.asciz`指令，会在字符串的最后加上0。
* 第16行，%edi的括号一定要加
* 这段代码只能在32位系统上运行
* 通过栈向C库函数传递参数，从右到左依次压栈
* 注意传递给链接器的`-lc`和`-dynamic-linker /lib/ld-linux.so.2`选项

# 第五章 传送数据

本章讨论如何处理数据，以及完成此任务的最佳方式。

## 定义数据元素

介绍在data段和bss段定义数据的方式。

### 数据段

数据段用来定义可读写的数据区域，指令码可以引用这些数据。定义数据需要一个`标签`和一个`指令`。`标签`是引用数据元素所使用的标记，代表一个内存位置。`汇编器指令`定义为数据元素保留多少个字节，这取决于定义的数据的类型和数量。声明命令后，必须定义一个或多个默认值。

| 指令 | 数据类型 |
| - | - |
| .ascii | 文本字符串 |
| .asciz | 以空字符结尾的文本字符串 |
| .byte | 字节值 |
| .double | 双精度浮点数 |
| .float | 单精度浮点数 |
| .int | 32位整数 |
| .long | 32位整数，和.int一样 |
| .octa | 16字节整数 |
| .quad | 8字节整数 |
| .short | 16位整数 |
| .single | 单精度浮点数 |
| .fill | 声明指定长度的内存区域 |

数据定义实例：
```asm
.section .data
ouput:
    .asciz "The Vendor ID is %s\n"  # 以\0结尾的字符串
pi:
    .float 3.1415  # 单精度浮点数
height:
    .int 54  # 32位整数
length:
    .int 62, 35, 47  # 定义多个32位整数，在内存中按照相对位置依次排列
buffer:
    .fill 128  # 申请128个字节并用0填充
```

> 注意：引用数据时的长度，和定义数据时的长度要一致，否者无法获取到正确的数据。

### 定义静态符号

`.equ`命令用于把常量值设置为可以在文本段中使用的符号，数据符号值在程序中是不能改动的。
```asm
.equ LINUX_SYS_CALL, 0x80
```

为了引用静态数据元素，需要在标签名称前面加`$`。
```asm
movl $LINUX_SYS_CALL, %eax
```

### bss段

与数据段相比，bss段只要声明所需要保留的内存数量即可，不需要声明数据类型。

| 命令 | 描述 |
| - | - |
| .comm | 声明未初始化的数据的通用内存区域 |
| .lcomm | 声明未初始化的数据的本地通用内存区域，不会从本地汇编代码之外进行访问 |

例子：
```asm
.section .bss
.lcomm buffer, 100  # 为 buffer 预留100个字节的内存区域
```

> bss段在可执行文件中不存在，但是会占用内存空间。data段即存在可执行文件中，也会占用内存。

## 传送数据元素

处理数据的第一个步骤是在内存和寄存器之间传送。

### MOV指令格式

MOV指令的基本格式：
```asm
mov source, destination
```
> GNU汇编器使用的AT&T语法，源操作数和目标操作数与Intel手册的顺序是相反的。

GNU汇编器还需要在指令中指定传送数据的长度，`mov`指令可以演变为：
* movl 传送32位
* movw 传送16位
* movb 传送8位

源操作数和目标操作数可以是内存位置、立即数、寄存器等。由于一些限制，`mov`指令的源和目标操作数的组合如下：

![](pic/MOV指令传送规则.png)

### 把立即数传送到寄存器和内存

立即数是在指令码语句中直接指定的，并且在运行时不能改动。立即数前需要加美元符号`$`，立即数可以是十进制或十六进制。

```asm
movl $0, %eax
movl $0x100, %ebx  # 将十六进制0x100赋值给EBX寄存器
movl $100, heigh   # 将十进制100赋值给内存位置heigh
```

### 在寄存器之间传送数据

普通寄存器（EAX、EBX、ECX、EDA、EDI、ESI、EBP和ESP）的值可以传送给内存，或接收来自内存或立即数的内容。而专用寄存器（控制、调试、段寄存器）的值只能传送给通用寄存器，或接收来自通用寄存器的内容。

> 在寄存器间传送数据要保持长度一致。

```asm
movl %eax, %ecx  # 32位数据从 EAX 寄存器传送到ECX寄存器
movw %ax, %cx    # 16位数据从 AX 寄存器传送到CX寄存器
```

### 在内存和寄存器之间传送数据

#### 把数据值从内存传送到寄存器

将`value`标签指定的`内存位置的数据值`传送到EAX，`movl`表示传送4个字节。

```asm
movl value, %eax
```

#### 把数据值从寄存器传送到内存

把`ECX`寄存器中存储的4字节数据传送给`value`标签指定的内存位置。

```asm
movl %ecx, value
```

#### 使用变址的内存位置

通过基址+偏移，引用数组中的数据。内存位置由下列因素确定：
* 基址
* 偏移地址
* 数据元素的长度
* 索引

表达式的格式：`base_address (offset_address, index, size)`。获取的数据值位于`base_address + offset_address + index * size`。如果任何值为0，则可以忽略它们，但仍需要逗号作为占位符。`offset_address`和`index`必须是寄存器。

```asm
.section .data
values:
    .int 10, 15, 20, 25, 30，35

.section .text
    movl $2, %edi
    movl values( , %edi, 4), %eax # 将数组的第3个值，即20，传送到EAX寄存器
```

> 通过更改`offset_address`或`index`的值，可以做到遍历数组。

#### 使用寄存器间接寻址

寄存器存储的是内存地址。通过内存地址获取内存位置中的数据称为间接寻址。通过在标签前面加美元符号`$`获取内存地址（仅适用`.data`段定义的标签）。

```asm
movl $value, %edi # 将value标签引用的地址传送到EDI寄存器
```

使用寄存器间接寻址内存。如果EDI没有加括号，表示把EBX寄存器的内容传送到EDI寄存器。如果加了括号，表示将EBA寄存器的内容传送到EDI指向的`内存`。可以在括号前加数字表示偏移。

```asm
movl %ebx, (%edi)
movl %ebx, 4(%edi)
movl %ebx, -4(%edi)
```

## 条件传送指令

`CMOV`指令仅在特定的条件下传送数据。相比于`JMP`指令，对程序优化可能起到一定的作用。`CMOV`指令的格式如下：

```asm
cmovx source, destination
```

其中`x`是一个或两个字母的代码，表示触发传送操作的条件。条件取决于`EFLAGS`寄存器的当前值。

| ELFAGS位 | 名称 | 描述 |
| - | - | - |
| CF | 进位（Carry）标志 | 数学表达式产生了进位或者借位 |
| OF | 溢出（Overflow）标志 | 整数值过大或者过小 |
| PF | 奇偶校验（Parity）标志 | 寄存器包含数学操作照成的错误数据 |
| SF | 符号（Sign）标志 | 指出结果为正还是负 |
| ZF | 零（Zero）标志 | 数学操作的结果为零 |

| 无符号条件传送指令 | 描述 | EFLAGS标志 |
| - | - | - |
| CMOVA / CMOVNBE | 大于 / 不小于或等于 | （CF或ZF) = 0 ，无进位或借位，结果不为0 |
| CMOVAE / CMOVNB | 大于或者等于 / 不小于 | | CF = 0，无进位或借位 |
| CMOVNC | 无进位 | CF = 0 |
| CMOVB / CMOVNAE | 小于 / 不大于或等于 | CF = 1 |
| CMOVC | 进位 | CF = 1 |
| CMOVBE / CMOVNA | 小于或者等于 / 不大于 | (CF或ZF) = 1 |
| CMOVE / CMOVZ | 等于 / 零 | ZF = 1 |
| CMOVNE / CMOVNZ | 不等于 / 不为零 | ZF = 0 |
| CMOVP / CMOVPE | 奇偶校验 / 偶校验 | PF = 1 |
| CMOVNP / CMOVPO | 非奇偶校验 / 奇校验 | PF = 0 |

| 符号条件传送指令 | 描述 | EFLAGS标志 |
| - | - | - |
| CMOVGE / CMOVNL | 大于或者等于 / 不小于 | (SF xor OF) = 0 |
| CMOVL / CMOVNGE | 小于 / 不大于或者等于 | (SF xor OF) = 1 |
| CMOVO | 溢出 | OF = 1 |
| CMOVNO | 未溢出 | OF = 0 |
| CMOVS | 带符号（负） | SF = 1 |
| CMOVNS | 无符号（非负） | SF = 0 |

示例代码：如果ECX寄存器的值大于EBX，则将ECX的值传送到EBX寄存器。

```asm
movl value, %ecx
cmp %ebx, %ecx
cmova %ecx, %ebc
```

> `CMP`指令从第二个寄存器减去第一个寄存器并设置`EFLAGS`寄存器。注意，这个顺序和Intel手册的顺序相反。

## 交换指令

交换指令用于交换数据元素的位置。如果使用`MOV`指令，则需要三条指令和一个中间寄存器，交换指令可以一步完成。

| 指令 | 指令格式 | 描述 |
| - | - | - |
| XCHG | xchg operand1, operand2 | 在两个寄存器或者寄存器和内存位置之间交换值 |
| BSWAP | bswap operand1 | 反转一个32位寄存器中的字节顺序 |
| XADD | xadd source, destination | 交换两个寄存器或者内存位置和寄存器的值，把两个值相加的结果存储在目标操作数。源操作数只能是寄存器，目标操作数可以是寄存器或内存位置。 |
| CMPXCHG | cmpxchg source, destination | if(source == eax) destination = source else eax = destination。没看懂这个指令的应用场景。|
| CMBXCHG8B | | |

交换指令的典型应用场景是排序算法，[bubble.s](code/ch05/bubble.s)是冒泡排序的实现。

## 堆栈

讲解堆栈和用于访问堆栈的指令。

### 堆栈如何工作

* 先入后出，后入先出。
* 栈底是内存区域的末尾位置，`ESP`寄存器始终指向栈顶。
* 压入数据时，`ESP`寄存器的值减少。即栈的地址向下增长。
* 只能从栈顶获取数据（某些技巧可以从中间获取）。

### 压入和弹出数据

压入和弹出的指令如下：

```asm
pushx source      # 压入栈
popx destination  # 弹出栈
```

其中`x`可以是`l`（长字，32位）或`w`（字，16位）。

`push`操作的数据元素如下：
* 16/32位寄存器
* 16/32位内存值
* 16位段寄存器
* 8/16/32位立即数

`pop`操作的数据元素如下：
* 16/32位寄存器
* 16位段寄存器
* 16/32位内存位置

> 数据元素的长度要和指令一致。一般来说，`push`和`pop`指令总是成对出现的。

### 压入和弹出所有寄存器

| 指令 | 描述 |
| - | - |
| pusha / popa | 压入或弹出所有16位通用寄存器 |
| pushad / popad | 压入或弹出所有32位通用寄存器 |
| pushf / popf | 压入或弹出EFLAGS寄存器的低16位 |
| pushfd / popfd | 压入或弹出ELFAGS寄存器的低32位 |

`pusha`的压入顺序是`DI`、`SI`、`BP`、`BX`、`DX`、`CX`、`AX`，`popa`弹出的顺序相反。`pushad`、`popad`的顺序类似。

### 手动使用ESP和EBP寄存器

在汇编语言函数中，经常会把`ESP`寄存器的值复制给`EBP`寄存器，`EBP`指针指向函数的工作堆栈空间的基址。访问存储在堆栈中的参数的指令相对于`EBP`指针引用这些参数。

## 优化内存访问

* 尽可能避免使用内存，最好把变量保存到寄存器中。
* 在内存中连续的顺序访问有利于提高缓存的命中率。
* 定义数据时对齐边界，`gas`汇编器支持`.align`命令。

# 第六章 控制执行流程

本章介绍用于进行跳转和循环的不同汇编语言指令。这两类指令都会改变指令指针。

## 指令指针

指令指针始终指向应该执行的下一条指令，它按照顺序的方式处理应用程序中编写的指令码。由于流水线、乱序引擎等技术的发明，确定`下一条指令`可能是困难的。

> 乱序执行简单来说，就是乱序引擎重排序微操作，发送给退役单元，退役单元执行并监控结果，在适当的时候发送给执行单元，然后从退役单元中删除相关微操作。

当指令指针在程序指令中移动时，`EIP`寄存器会递增，递增的长度是指令的长度。不能通过`MOV`指令更改`EIP`寄存器的值，可以通过分支指令改动。分支指令可以分为无条件分支和条件分支。
