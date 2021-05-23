# 使用符号标志(SF)的示例

.section .data
value:
    .int 21, 15, 34, 11, 6, 50, 32, 80, 10, 2
value_end:
output:
    .asziz "The value is: %d\n"

.que len, (value_end - value)/4 - 1

.section .text
.global main
main:
    movl $len, %edi
loop:
    pushl value(, %edi, 4)
    pushl $output
    call printf
    add $8, %esp   # 清空栈中的数据
    dec %edi
    jns loop       # 如果符号标志为0，则跳转。当edi由0变为-1时，符号标志置位。
    movl $1, %eax
    movl $0, %ebx
    int $0x80
