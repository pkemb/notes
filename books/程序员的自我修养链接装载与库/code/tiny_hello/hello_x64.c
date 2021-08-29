/*
 * 调试通过的平台：
 *      CentOS 7，3.10.0-1127.10.1.el7.x86_64
 */
char *str = "hello\n";

void print()
{
    asm("movq $6,     %%rdx \n\t" // str length
        "movq %[str], %%rsi \n\t" // str address
        "movq $1,     %%rdi \n\t" // file descriptor
        "movq $1,     %%rax \n\t" // syscall number
        "syscall            \n\t" // start syscall
        ::[str]"r"(str):"rdi","rsi","rdx");
}

void exit()
{
    asm("movq $42, %rdi \n\t" // exit number
        "movq $60, %rax \n\t" // syscall number
        "syscall \n\t");      // start syscall
}

void nomain()
{
    print();
    exit();
}
