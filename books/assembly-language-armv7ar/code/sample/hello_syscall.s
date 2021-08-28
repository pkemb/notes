.section .data
hello:
    .asciz	"hello, arm\n"

.section .text
    .global	main
main:
    mov r0, #1      // 文件描述符，1是标准输出
    ldr r1, =hello  // 字符串首地址
    mov r2, #12     // 字符串长度
    mov r7, #4      // write()的调用号是4，通过r7寄存器传递
    swi #0x0        // 触发系统调用

    mov r0, #42     // 退出码
    mov r7, #1      // exit()的调用号是7
    swi #0x0        // 触发系统调用
