# cpuid2.s View the CPUID Vendor ID string using C library calls

.section .data
output:
    .asciz "The processor Vendor ID is '%s'\n"

.section .bss
    .lcomm buffer, 12

.section .text
.global main
main:
    movl $0, %eax
    cpuid
    movl $buffer, %edi
    movl %ebx, (%edi)
    movl %edx, 4(%edi)
    movl %ecx, 8(%edi)
    # call printf
    push $buffer
    push $output
    call printf
    # call exit
    addl $8, %esp
    push $0
    call exit
