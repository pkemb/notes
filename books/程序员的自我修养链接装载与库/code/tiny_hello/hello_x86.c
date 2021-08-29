/*
 * 调试通过的平台：
 *      Dedora 10，2.6.27.5-117.fc10.i686
 */
char *str = "hello\n";

void print()
{
    asm("movl $6,     %%edx \n\t" // str length
        "movl %[str], %%ecx \n\t" // str address
        "movl $1,     %%ebx \n\t" // file descriptor
        "movl $4,     %%eax \n\t" // syscall number
        "int $0x80          \n\t" // start syscall
        ::[str]"r"(str):"edx","ecx","ebx");
}

void exit()
{
    asm("movl $42, %ebx \n\t"   // exit number
        "movl $1,  %eax \n\t"   // syscall number
        "int $0x80      \n\t"); // start syscall
}

void nomain()
{
    print();
    exit();
}
