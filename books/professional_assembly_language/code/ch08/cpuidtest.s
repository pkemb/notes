# 如果能设置或清零
.section .data
output_cpuid:
	.asciz "This processor supports the CPUID instruction\n"
output_nocpuid:
	.asciz "This processor does not support the CPUID instruction\n"

.section .text
.global main
main:
	nop
	pushfl
	popl %eax
	movl %eax, %edx         # edx记录原始值
	xor $0x00200000, %eax   # ID位取反，其余位不变
	pushl %eax
	popfl                   # 设置EFLAGS寄存器

	pushfl
	popl %eax               # 再次获取EFLAGS寄存器值
	xor %edx, %eax          # 如果支持，由于ID位已经取反，xor操作导致EAX寄存器中ID位为1，其余位均为0
	                        # 如果不支持，ID位取反设置无效，xor操作导致EAX寄存器为0
	test $0x00200000, %eax

	jnz cpuid
	pushl $output_nocpuid
	call printf
	jmp done
cpuid:
	pushl $output_cpuid
	call printf
done:
	pushl $0
	call exit
