# 汇编语言程序设计
# Professional Assembly Language
## Richard Blum

记录书籍上的一些排版错误。

## P110

原文：
> 带符号数只能使用-127到127的值

修改：根据上下文，对于8位带符号数，值的范围是-128~127。

## P121

页面中部的代码片段，原文如下：

```asm
    movl $100, $ecx
loop1:
    addl %cx, %eax
    decl %ecx
    jns loop
```

第1行，是`%ecx`。第3行，是`%ecx`。修正如下：

```asm
    movl $100, %ecx
loop1:
    addl %ecx, %eax
    decl %ecx
    jns loop
```

## P182

第8.4.1小节。原文：
> 清空寄存器的最高效的方式是使用`OR`指令对寄存器和它本身进行异或操作。

应该更改为：
> 清空寄存器的最高效的方式是使用`XOR`指令对寄存器和它本身进行异或操作。

## P201

9.3.3小节，将角度转换为弧度的代码片段，原代码：
> fsts degree1     # load the degrees value stored in memeory into ST0

应该修改为：
> flds degree1     # load the degrees value stored in memeory into ST0
