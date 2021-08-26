# ARMv7-A/R汇编语言程序设计

模仿`Richard Blum`的《汇编语言程序设计》，将书中`IA-32`相关的内容，基于`ARMv7-A/R`架构改写。同样是基于`Linux`系统，不考虑裸机程序。内容结构主体上保持不变，会增加`ARMv7-A/R`架构特有的内容，删除`ARMv7-A/R`没有的内容。

# 什么是汇编语言

TODO。学习ARM指令编码后再改写。

* Chapter A5 ARM Instruction Set Encoding
* Chapter A6 Thumb Instruction Set Encoding
* Chapter A7 Advanced SIMD and Floating-point Instruction Encoding
* Chapter A9 The ThumbEE Instruction Set

# ARMv7-A和ARMv7-R平台

TODO。主要包括以下内容：
1. 处理器核心部分
   1. 控制单元
   2. 执行单元
   3. 寄存器
   4. 特殊寄存器
2. 关于ARM架构的其他内容
3. 基于ARM内核的处理器

* Chapter A1 Introduction to the ARM Architecture
* xxx

# 相关的工具

工具基本上与处理器架构无关，略。增加armcc编译器的说明。同样选择GNU工具链编译、调试示例程序。

# 参考资料

1. [ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition](https://developer.arm.com/documentation/ddi0406/latest)



