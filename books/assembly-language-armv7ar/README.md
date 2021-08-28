# ARMv7-A/R汇编语言程序设计

模仿`Richard Blum`的《汇编语言程序设计》，将书中`IA-32`相关的内容，基于`ARMv7-A/R`架构、`Cortex-A7`处理器核心改写。同样是基于`Linux`系统，不考虑裸机程序。内容结构主体上保持不变，会增加`ARMv7-A/R`架构特有的内容，删除`ARMv7-A/R`没有的内容。

# 0. 理解arm文档

在最最开始前，先要明白几个概念，这有助于我们查询ARM的文档。这里参考了ARM官方的[Understanding Arm documentation](https://developer.arm.com/documentation/102404/0200/Understanding-Arm-documentation)。这里明确解释了架构、处理器、SoC之间的关系。`ARMv7-A`是架构，描述了指令集、寄存器等内容，是思想上的指导性文档。`Cortex-A7`是基于`ARMv7-A`架构设计出来的一个处理器核心，是架构的落地实现。SoC则是基于处理器核心制造出来的IC，相比于处理器核心，主要是增加了外设、引脚等内容。

架构相关的内容，需要参考`Arm Architecture Reference Manual`。处理器相关的内容，需要参考`Technical Reference Manual (TRM)`。SoC相关的内容，需要从SoC制造商获取。从`Arm Cortex-A Processor Comparison Table`可以了解到所有`Cortex-A`处理器使用的架构。

* [ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition](https://developer.arm.com/documentation/ddi0406/latest)
* [Cortex-A7 MPCore Technical Reference Manual](https://developer.arm.com/documentation/ddi0464/d/)
* [Arm Cortex-A Processor Comparison Table](https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/Cortex-A%20R%20M%20datasheets/Arm%20Cortex-A%20Comparison%20Table_v4.pdf?revision=ff02efe8-e170-4cc0-ae66-9b1ef1dd3b8b)

# 1. 什么是汇编语言

TODO。学习ARM指令编码后再改写。

* Chapter A5 ARM Instruction Set Encoding
* Chapter A6 Thumb Instruction Set Encoding
* Chapter A7 Advanced SIMD and Floating-point Instruction Encoding
* Chapter A9 The ThumbEE Instruction Set

# 2. Cortex-A7处理器

TODO。主要包括以下内容：
1. 处理器核心框图
   1. 控制单元
   2. 执行单元
   3. 寄存器
   4. 特殊寄存器
2. 关于ARM架构的其他内容
3. 基于Cortex-A7的处理器

* Chapter A1 Introduction to the ARM Architecture
* xxx

## ARM处理器模式和寄存器

参考：
* ARM架构参考手册
  * B1.3 ARM processor modes and ARM core registers
  * Chapter B4 System Control Registers in a VMSA
implementation

### ARM处理器模式

| 处理器模式 | 特权等级 | 实现 | security state |
| - | - | - | - |
| usr | PL0 | Always | Both |
| fiq |
| irq |
| svc |
| mon |
| abt |
| hyp |
| und |
| sys |

### 寄存器

* R0 ~ R12
* stack pointer SP, R13
* link register LR, R14
* program counter PC, R15
* CPSR 状态寄存器，各个位表示的含义
* SPSR 保存的状态寄存器
* SCTLR 系统控制寄存器 VMSA，
* HSCTLR, Hyp System Control Register, Virtualization Extensions

# 3. 相关的工具

工具基本上与处理器架构无关，略。增加armcc编译器的说明。同样选择GNU工具链编译、调试示例程序。

GAS语法：
1. 寄存器名字：无需使用`$`字符修饰
2. 立即数：需要使用字符`#`
3. 使用ARM官方文档的语法。
4. [Why is GNU as syntax different between x86 and ARM?](https://stackoverflow.com/questions/43574163/why-is-gnu-as-syntax-different-between-x86-and-arm)
5. [ARM Developer Suite Assembler Guide](https://developer.arm.com/documentation/dui0068/latest/)
6. [ARM Compiler armasm User Guide](https://developer.arm.com/documentation/dui0473/m/dom1359731141352)
7. 9.4 ARM Dependent Features, https://doc.ecoscentric.com/gnutools/doc/as.pdf

# 4. 汇编语言程序范例

选择一个合适的demo程序演示。TODD。
* 调用系统调用，https://man7.org/linux/man-pages/man2/syscall.2.html
* 调用库函数

示例：
* [hello_syscall.s](code/sample/hello_syscall.s)
* [hello_glibc.s](code/sample/hello_glibc.s)

# 参考资料

1. [ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition](https://developer.arm.com/documentation/ddi0406/latest)
2. [ARM Processor Architecture](https://www.cs.ccu.edu.tw/~pahsiung/courses/ese/notes/ESD_03_ARM_Architecture.pdf)
3. https://developer.arm.com/architectures/learn-the-architecture/a-profile

