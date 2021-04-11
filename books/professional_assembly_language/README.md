# 汇编语言程序设计

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
