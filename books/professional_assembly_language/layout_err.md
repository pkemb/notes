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
