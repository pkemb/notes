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

## 无条件分支

当程序遇到无条件分支指令时，指针指针自动跳转到另一个位置。无条件分支指令分为以下三种。不同的无条件分支指令有不同的特点，在不同的场合下使用。

* [跳转](#跳转)
* [调用](#调用)
* [中断](#中断)

### 跳转

跳转是汇编语言程序设计中最基本的分支类型，与C语言的`goto`语句是等同的。跳转对程序的性能有负面的影响。跳转指令的格式如下：

```asm
jmp location
```

`location`是要跳转到的程序地址，一般是汇编语言程序代码中的标签。遇到跳转指令时，指令指针寄存器`EIP`改变为紧跟在`location`后面的指令码的内存位置。在幕后，单一跳转汇编指令被分为三种情况，在编码层面是统一的，没有区别。
* 短跳转：跳转偏移量小于128字节
* 远跳转：在分段内存模式下，从一个段跳转到另一个段
* 近跳转：其余所有情况

### 调用

调用和跳转类型，调用保存发生跳转的位置，并具有在需要的时候返回这个位置的能力。汇编语言实现函数使用调用。调用指令分为两个部分，第一个是`call`指令，操作数`address`是程序代码中的标签，被转化为函数第一条指令的内存地址。第二个是`ret`指令，返回到紧跟着`call`指令后面的位置。返回地址通过堆栈来保存，当调用`call`指令时，`EIP`寄存器的值保存到堆栈，并用目标程序的地址更新`EIP`。调用完成之后，将保存到堆栈中的值恢复到`EIP`寄存器。

```asm
call address

ret
```

`call`指令的关键是如何把信息传递给函数，以及函数如何读取和存储这些信息。一般使用堆栈来保存信息，并保存函数的返回值。如[手动使用ESP和EBP寄存器](#手动使用ESP和EBP寄存器)小节所说，可以使用`EBP`间接访问堆栈。从而得到以下函数模板。更多有关函数的信息，将在第11章讨论。

```asm
function_label:
    pushl %ebp
    movl %esp, %ebp
    # some function code here
    movl %ebp, %esp
    popl %ebp
    ret
```

### 中断

中断是处理器中断当前指令码路径并切换到不同路径的方式。中断有两种方式：
* 硬件中断
* 软件中断

与函数类似，当中断发生时，当前正在执行的代码被暂停，转而去执行中断处理程序。中断处理程序执行完毕之后，返回到暂停的地方继续执行。硬件中断由硬件设备生成，是被动的、不可控的。软件中断是软件生成的，是主动的、可控的。

操作系统通过软件中断提供内核函数，从而可以使用内核提供的功能。例如Linux的0x80中断。注意，调试包含软件中断的程序是比较困难的，因为中断处理程序没有被编译到代码中。

## 条件分支

条件分支取决于执行分支时`EFLAGS`寄存器的状态，下面是`EFLAGS`寄存器与条件分支有关的位。每个条件跳转指令都检查特定的标志位以便确定是否符合进行跳转的条件。
* 进位标志（CF）
* 溢出标志（OF）
* 奇偶校验标志（PF）
* 符号标志（SF）
* 零标志（ZF）

### 条件跳转指令

条件跳转指令的格式如下，其中`xxx`是一到三个字符的条件代码，`address`是程序要跳转到的位置。

```asm
jxxx address
```

下表是所有的条件跳转指令。

| 指令 | 描述 | EFLAGS |
| -   | - | - |
| JA    | 如果大于（above），则跳转 | CF=0与ZF=0 |
| JAE   | 如果大于（above）或等于，则跳转 | CF=0 |
| JB    | 如果小于（below），则跳转 | CF = 1 |
| JBE   | 如果小于（below），或等于，则跳转 | CF=1或ZF=1 |
| JC    | 如果进位则跳转 | |
| JCXZ  | 如果CX寄存器为0则跳转 | |
| JECXZ | 如果ECX寄存器为0则跳转 | |
| JE    | 如果相等则跳转 | ZF=1 |
| JG    | 如果大于（greater），则跳转 | ZF=0与SF=OF |
| JGE   | 如果大于（greater）或等于，则跳转 | SF=OF |
| JL    | 如果小于（less），则跳转 | SF<>OF |
| JLE   | 如果小于（less）或等于，则跳转 | ZF=1|
| JNA   | 如果不大于（above），则跳转 | CF=1或ZF=1 |
| JNAE  | 如果不大于（above）或等于，则跳转 | CF=1 |
| JNB   | 如果不小于（below），则跳转 | CF=0 |
| JNBE  | 如果不小于（below）或等于，则跳转 | CF=0或ZF=0 |
| JNC   | 如果无进位，则跳转 | CF=0 |
| JNE   | 如果不等于，则跳转 | ZF=0 |
| JNG   | 如果不大于（greater），则跳转 | ZF=1或SF<>OF |
| JNGE  | 如果不大于（greater）或等于，则跳转 | SF<>OF |
| JNL   | 如果不小于（less），则跳转 | SF=OF |
| JNLE  | 如果不小于（less）或等于，则跳转 | ZF=0或SF=OF |
| JNO   | 如果不溢出，则跳转 | OF=0 |
| JNP   | 如果不奇偶校验，则跳转 | PF=0 |
| JNS   | 如果无符号，则跳转 | SF=0 |
| JNZ   | 如果非零，则跳转 | ZF=0 |
| JO    | 如果溢出，则跳转 | OF=1 |
| JP    | 如果奇偶校验，则跳转 | PF=1 |
| JPE   | 如果偶校验，则跳转 | PF=1 |
| JPO   | 如果奇校验，则跳转 | PF=0 |
| JS    | 如果带符号，则跳转 | SF=1 |
| JZ    | 如果为零，则跳转 | ZF=1 |

说明：
* 部分条件跳转指令是等价的。
* 对于无符号数，使用above/below；对于有符号数，使用greater/less。
* 条件跳转指令只支持短跳转和近跳转，不支持跨段的远跳转。远跳转需要手动编写比较和跳转代码。

### 比较指令

`cmp`指令会比较两个值，并且相应地设置`EFLAGS`寄存器。`cmp`指令的格式如下：

```asm
cmp operand1, operand2
```

> 注意，`cmp`指令用后一个数减去前一个数，但不会更改操作数的值。顺序和Intel手册是相反的。

例子：

```asm
cmp $20, %ebx    ; 比较EBX寄存器和立即数20
cmp data, %ebx   ; 比较EBX寄存器和内存data
cmp (%edi), %ebx ; 比较EBX寄存器和EDI寄存器指向的内存单元
```

### 使用标志位的范例

#### 使用零标志

如果零标志被置1（两个操作数相等），`JE`和`JZ`指令就跳转到分支。零标志可以由`cmp`或数学指令设置。

```asm
movl $10, %edi
loop:
    # some code there
    dec %edi  # EDI寄存器的值减少1
    jz out    # 如果为0则跳转到out标签
    jmp loop
out:
```

#### 使用溢出标志

溢出标志仅针对符号数字，当带符号值对于包含它的数据元素来说太大了，溢出标志被设置为1。

```asm
movb $0x7f, %bl
addb $10, %bl   # 对8位带符号数来说，值的范围是-128 ~ 127。
jb overhere
# some code here
overhere:
```

#### 使用奇偶校验标志

奇偶标志位表明数学运算答案中应该为1的位的数目。如果结果中被设置为1的位的数目是偶数，则奇偶标志位置1，否则置0。

#### 使用符号标志

符号标志使用在带符号数中，用于表示寄存器中包含的值的符号的改变（即符号发生变化时置1）。例如在遍历数组时，可以监控索引从0到-1的变化。例如：[signtest.s](code/ch06/signtest.s)。

#### 使用进位标志

进位标志用在数学表达式中，表示无符号数发生溢出（有符号数使用溢出标志）。向上溢出（结果大于可以表示的最大值）和向下溢出（结果小于0）均会设置进位标志。和溢出标志不同，`dec`和`inc`指令不影响进位标志。

```asm
movl $0xffffffff, %ebx
inc %ebx                # 进位标志不会置位

movl $0xffffffff, %ebx
addl $1, %ebx           # 进位标志置位

movl $2, %eax
subl $4, %eax           # eax = eax - 4，进位标志置位
```

以下指令可以用于设置进位标志。

| 指令 | 描述 |
| - | - |
| CLC | 清空进位标志（设置为0） |
| CMS | 对进位标志求反 |
| STC | 设置进位标志（设置为1） |

## 循环

循环操作重复地执行，直到满足特定条件。

### 循环指令

循环指令使用`ECX`寄存器作为计数器并且随着循环指令的指令自动递减`ECX`的值。递减到0时，不会设置`EFLAGS`寄存器。

| 指令 | 描述 |
| - | - |
| LOOP | 循环直到ECX寄存器为0 |
| LOOPE / LOOPZ | 循环直到ECX寄存器为0，或者没有设置ZF标志（相等/为零时循环） |
| LOOPNE / LOOPNZ | 循环直到ECX寄存器为0，或者设置ZF标志（不相等/不为零时循环） |

循环指令的格式如下。循环指令只支持8位偏移，只能进行短跳转。

```asm
loop address
```

循环开始之前，必须在`ECX`寄存器设置迭代次数。

```asm
    movl $100, %ecx
loop1:
    # some code here
    loop loop1
```

### 循环范例

```asm
# 循环指令示例
.section .data
output:
    .asciz "The value is: %d\n"
.section .text
.global main
main:
    movl $100, %ecx   # 初始化循环计数器
    movl $0, %eax
loop1:
    addl %ecx, %eax   # 将ECX的值累加到EAX
    loop loop1        # 先将ECX减1，再判断是否为0。不为0则跳转到loop1
    pushl %eax
    pushl $output
    call printf
    addl $8, %esp
    movl $1, %eax
    movl $0, %ebx
    int $0x80
```

### 防止loop灾难

`loop`指令先递减`ECX`寄存器，再检查`ECX`寄存器的值。如果`ECX`寄存器为0时执行`loop`指令，那么循环会在寄存器溢出时退出。所以，在执行`loop`指令之前，需要检查`ECX`是否为0，可以使用指令`jcxz`来完成。

```asm
# 循环指令示例
.section .data
output:
    .asciz "The value is: %d\n"
.section .text
.global main
main:
    movl $0, %ecx     # 循环计数器初始值是0
    movl $0, %eax
    jcxz done         # 如果ecx为0，则跳转到done标签
loop1:
    addl %ecx, %eax   # 将ECX的值累加到EAX
    loop loop1        # 先将ECX减1，再判断是否为0。不为0则跳转到loop1
done:
    pushl %eax
    pushl $output
    call printf
    addl $8, %esp
    movl $1, %eax
    movl $0, %ebx
    int $0x80
```

## 模仿高级条件分支

通过反汇编C语言代码，学习如何使用汇编完成条件分支。

### if语句

```asm
if:
    <计算条件>
    jxx else   # 条件为false时跳转到else分支
    <条件为true的代码>
    jmp end
else:
    <条件为false的代码>
end:
```

如果条件比较复杂，建议使用多个跳转指令，每个指令计算条件的一个部分。

### for循环

```asm
for:
    <计算条件>
    jxx forcode  # 条件为true时，跳转到循环体
    jmp end
forcode:
    <for循环体>
    <递增计数器>
    jmp for
end:
```

## 优化分支指令

分支指令可能会导致处理器的指令缓存失效，从而降低程序的性能。

### 分支预测

在遇到分支指令时，处理器的乱序引擎会预测要处理的下一条指令。

1. 无条件分支：很容易确定下一条指令。但是如果跳转距离过远，下一条指令不在缓存中，那么会清空整个缓存区并重新加载指令，严重影响性能。
2. 条件分支：用分支预测算法猜测采取的分支。分支预测算法的主要规则有：
   * 假设会采取向后分支（EIP寄存器的值减少）
   * 假设不会采取向前分支（跟在跳转指令后面的代码最可能被执行）
   * 以前曾经采用过的分支会再次采用，优先级高于前两条。

### 优化技巧

1. 消除分支
2. 编写可预测分支的代码
   1. 最可能执行的代码放在向后分支
   2. 最可能执行的代码放在向前分支的顺序语句中（紧跟着跳转指令的指令）
   ```asm
    loop:
        <可能性高的向后代码>
    jnz loop
        <可能性低一些的顺序执行代码>

    jmp end
        <可能性高一些的顺序执行代码>
    end:
        <可能性低一些的向前代码>
   ```
3. 尽可能展开小循环

# 第七章 使用数字

在汇编语言程序设计中，存储在内存或寄存器中的值可以被解释为多种不同的数据类型。在编码过程中，确保实用正确的指令、以正确的方式解释存储的数据。

## 数字数据类型

在汇编语言程序设计中，有多种表示数字的数据类型，常见的有：
* [无符号整数](#无符号整数)
* [带符号整数](#带符号整数)
* 二进制编码的十进制（BCD）
* 打包的二进制编码的十进制
* 单精度浮点数
* 双精度浮点数
* 双精度扩展浮点数

奔腾处理器SIMD扩展添加的高级数字类型：
* 64位打包整数
* 128位打包整数
* 128位打包单精度浮点数
* 128位打包双精度浮点数

下面会给出每种数据类型，并给出实用范例。

## 整数

汇编语言最基本的数字形式是整数，可以表示一个很大范围内的全部值。

### 标准整数长度

IA-32平台支持4种不同的整数长度：
* 字节，byte，8位
* 字，word，16位
* 双字，doubleword，32位
* 四字，quadword，64位

对于超过一个字节的整数，在内存中以小端格式存储，但在寄存器中以大端格式存储。内存和寄存器相互传输数据中，会自动转换成目标格式。

> 小端：低位存储在低地址。大端：高位存储在低地址。
> 假设连续的内存单元依次存储了0x12、0x34、0x56、0x78。即0x12在低地址，0x78在高地址。
> 小端解释为：0x78563412。
> 大端解释为：0x12345678。

### 无符号整数

组成整数的字节的值直接表示整数值。不同长度的无符号整数有不同的长度范围。

| 位 | 整数范围 |
| - | - |
| 8 | 0 ~ 255 |
| 16 | 0 ~ 65535 |
| 32 | 0 ~ 4294967295 |
| 64 | 0 ~ 18446744073709551615 |

### 带符号整数

有3种方法在计算机中描述负值：
* 带符号数值
  * 将组成带符号整数的位分为符号位和数值位，字节的最大有效位用于表示值的符号，正数是0，符数是1。其余位用二进制表示数字的数值。
  * 有两个0，+0 和 -0。
  * 不能按照无符号整数的方法进行加减
* 反码
  * 无符号整数的相反代码生成相应的负值。例如00000010（十进制2）的反码是11111101（十进制-2）。
  * 0 有两种表示方法
  * 无法进行标准二进制运算
* 补码
  * 反码加1即可得到补码。例如00000010（十进制2）的补码是11111110（十进制-2）。
  * 0只有一种表示。
  * 可以使用标准二进制运算。

计算机使用补码来表示带符号数。下表列出了不同长度带符号数的范围。

| 位 | 最小和最大带符号值 |
| - | - |
| 8 | -128 ~ 127 |
| 16 | -32768 ~ 32767 |
| 32 | -2147483648 ~ 2147483647 |
| 64 | -9223372036854775808 ~ 9223372036854775807 |

### 使用带符号整数

内存和寄存器只是存储二进制值。将二进制值解释成什么内容，是由程序来控制的。

### 扩展整数

将值传送给大一些的位置时（例如字传送给双字），需要扩展整数。

对于无符号整数来说，高位需要清0。使用指令`MOVZX`可以一步完成传输数据和清0。`source`可以是8为或16位的寄存器或内存位置，`destination`可以是16位或32位的寄存器。

```asm
movzx source, destination
```

考虑如下代码段，`CL`寄存器的值会传送给`EBX`寄存器的低8位，同时`EBX`寄存器的高24位会被清0。
```asm
movzx %cl, %ebx
```

对于符号整数来说，高位需要扩展为符号位的值。如果符号位是0，则高位填0；如果符号位是1，则高位填1。使用指令`MOVSX`可以完成此项工作，指令格式与`MOVZX`类似。考虑如下代码段，`BX`寄存器存储了符号数`-79`。使用指令`MOVSX`传输给`EAX`寄存器时，`EAX`寄存器的高16位会被1填充，保证`EAX`寄存器的值依旧是`-79`。
```asm
movw $-79, %bx
movsx %bx, %eax
```

### 在GNU汇编器中定义整数

汇编语言可以在指令中使用立即数，也可以在数据段中定义整数，这部分内容在[定义数据元素](#定义数据元素)一节中介绍了。考虑如下代码段，在数据段预留了32位大小的内存空间，并用初始值`0xFFFFFFFD`填充。标签`data1`既可以被解释为一个32位带符号整数`-3`，也可以被解释为两个16位带符号整数`-1`和`-3`，也可以被解释为4个8位带符号整数`-1`、`-1`、`-1`和`-3`。

```asm
.section .data
data1:
    .int -3
```

## SIMD整数

### MMX整数

### 传送MMX整数

### SSE整数

### 传送SSE整数

略。

## 二进制编码的十进制

Binary Coded Deimal, 即BCD码，经常用于简化对使用十进制数字的设备的处理，例如计时器。

### BCD是什么

按照二进制格式对十进制进行编码。每个BCD值都是一个无符号8位整数，值的范围是0~9，大于9的值被认为是非法的。包含BCD值的字节组合在一起表示十进制的数位。最低字节保存十进制的个位值，下一个较高字节保存十进制的十位值，依次类推。例如十进制214表示为BCD值`00000010 00000001 00000100`。

打包的BCD使用一个字节表示两个BCD值。例如十进制1489表示为BCD值`00010100 10001001`。但是无法表示带符号的BCD整数。

### FPU BCD值

FPU寄存器可以用于在FPU之内进行BCD数学运算操作。FPU有8个80位（10字节）寄存器（ST0 ~ ST7）。使用低位的9个字节存储打包的BCD值，最高字节的最高一位用来表示符号，0是正，1是负。最高字节的其余7位不使用。

在内存中创建80位打包的BCD值，传送给FPU寄存器后会自动转换为扩展的双精度浮点数。在FPU中，对数据进行的任何数学操作都是按照浮点格式进行的。从FPU寄存器获取结果时，浮点值被自动转换为80位打包BCD格式。

### 传送BCD值

指令`fbld`把80位打包BCD值加载到FPU寄存器中，指令`fbstp`从FPU寄存器获取这些值。格式如下，其中`source`和`destination`是80位的内存位置。

```asm
fbld source
fbstp destination
```

> FPU寄存器的行为类似于堆栈，可以把值压入和弹出FPU寄存器池。ST0引用位于堆栈顶部的寄存器。当值被压入FPU寄存器堆栈时，它被放在ST0寄存器，ST0中原来的值被加载到ST1中。

```asm
.section .data
data1:
    .byte 0x34, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
data2:
    .int 2
.section .text
.global main
main:
    nop
    fbld data1   # 将80位打包BCD值加载到ST0寄存器
    fimul data2  # ST0 = ST0 * data2
    fbstp data1  # 将ST0寄存器的值存放到data1
    movl $1, %eax
    movl $0, %ebx
    int $0x80
```
