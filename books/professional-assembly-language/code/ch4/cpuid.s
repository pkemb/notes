# cpuid.s Sample program to extrace the processor Vendor ID

.section .data
output:
    .ascii "The processor Vendor ID is 'xxxxxxxxxxxx'\n"

.section .text
.global main
main:
    movl $0, %eax
    cpuid
    movl $output, %edi
    movl %ebx, 28(%edi)
    movl %edx, 32(%edi)
    movl %ecx, 36(%edi)
    # call write
    movl $4, %eax      # syscall num
    movl $1, %ebx      # file descript
    movl $output, %ecx # string
    movl $42, %edx     # length
    int $0x80
    # call exit
    movl $1, %eax
    movl $0, %ebx
    int $0x80
