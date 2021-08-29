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

## P247

`图11-6`左边的代码片段，`movl`语句写反了。原代码片段：
```asm
function:
    pushl %ebp
    movl %ebp, %esp
    sub $8, %esp
    .
    .
```

应该修改为：
```asm
function:
    pushl %ebp
    movl %esp, %ebp
    sub $8, %esp
    .
    .
```

## P259

本页开头的程序，

```asm
    call printf
    addl $12, %esp      # 这里只用跳过两个参数，改为 addl $8, $esp
    addl $4, %ebp
    loop loop1          # 这里改为jmp loop1更合理，但是使用loop指令也能正常工作，不过很难看懂
                        # 因为printf函数会导致ecx寄存器变为0
                        # 0减1后变为-1，不等于0，loop指令还是会跳转到loop1标签
endit:
    pushl $0
    call exit
```

P302

第13.3.6小节，声明替换的名称不需要百分号。原文：
```asm
%[name]"constraint"(variable)
```

更改为：
```asm
[name]"constraint"(variable)
```