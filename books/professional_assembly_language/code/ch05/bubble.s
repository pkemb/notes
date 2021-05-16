# 冒泡排序
#
# 与书籍第91页的冒泡排序相比
#   1. 数组长度自动计算
#   2. j 指令的使用不灵活

.section .data
values:
    .int 45, 34, 90, 78, 3, 9, 10
values_end:

.equ len, (values_end - values)/4 -1  # 自动计算数组长度

.section .text
.global main
main:
    movl $values, %edi  # 数组基地址
    movl $len, %ebx     # 数组长度
    movl $len, %ecx
loop:
    movl (%edi), %eax   # load value to eax
    cmp %eax, 4(%edi)   # 下一个值小于当前值则交换
    jge skip
    xchg %eax, 4(%edi)
    movl %eax, (%edi)
skip:
    addl $4, %edi       # 指向下一个数
    dec %ebx            # 内层循环计数器减 1
    cmp $0, %ebx
    jg loop

    dec %ecx            # 外层循环计数器减 1
    movl %ecx, %ebx     # 内层循环计数器赋值
    movl $values, %edi  # edi 指向第一个数
    cmp $0, %ecx
    jg loop

    movl $1, %eax       # 调用 exit()
    movl $0, %ebx
    int $0x80
