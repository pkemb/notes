.section .data
hello:
    .asciz	"hello, arm\n"

.section .text
    .global	main
main:
    ldr r0, =hello
    push {r0}
    blx printf
    sub sp, sp, #4

    mov r0, #42
    push {r0}
    blx exit
