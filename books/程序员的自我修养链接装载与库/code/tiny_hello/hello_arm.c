/*
 * 调试通过的平台：
 *      树莓派3b，4.19.110-300.el7.armv7hl
 */
char *str = "hello\n";

void print()
{
    asm("mov r2, #6     \n\t" // str length
        "mov r1, %[str] \n\t" // str address
        "mov r0, #1     \n\t" // file descriptor
        "mov r7, #4     \n\t" // syscall number
        "swi #0x80      \n\t" // start syscall
        ::[str]"r"(str):"r0","r1","r2");
}

void exit()
{
    asm("mov r0, #42 \n\t"   // exit number
        "mov r7, #1  \n\t"   // syscall number
        "swi #0x80   \n\t"); // start syscall
}

void nomain()
{
    print();
    exit();
}
