# 退出码
# 1   string2 < string1
# 255 string2 > string1
# 0   string2 = string1
.section .data
string1:
	.ascii "test"
length1:
	.int 4
string2:
	.ascii "test1"
length2:
	.int 5
.section .text
.global main
main:
	leal string1, %esi
	leal string2, %edi  # string2 - string1
	movl length1, %ecx
	movl length2, %eax
	cmpl %ecx, %eax
	ja longer
	xchg %ecx, %eax     # 将较小的长度存储在ECX寄存器
longer:
	cld
	repe cmpsb          # 比较字符串
	je equal            # 相同长度部分字符串相等
	jg greater          # string2 大于 string1
less:                   # string2 小于 string1
	movl $1, %eax
	movl $1, %ebx
	int $0x80
greater:                # string2 大于 string1
	movl $1, %eax
	movl $255, %ebx
	int $0x80
equal:
	movl length1, %ecx
	movl length2, %eax
	cmpl %ecx, %eax    # 长度较长的字符串大
	jg greater         # string2长度长
	jl less            # string1长度长
	movl $1, %eax      # 长度相同，字符串相等
	movl $0, %ebx
	int $0x80
