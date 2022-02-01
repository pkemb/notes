#include <stdio.h>
#include <signal.h>
#include <string.h>

void test_sys_siglist(void);
void test_psignal(void);
void test_psiginfo(void);
void test_strsignal(void);
void test_sig2str(void);
void test_str2sig(void);

#define __INVOKE(func, func_name)               \
        do {                                    \
            printf("test %s\n", func_name + 5); \
            func();                             \
            printf("\n");                       \
        }while(0)

#define INVOKE(func)    __INVOKE(func, #func)

int main()
{
    INVOKE(test_sys_siglist);
    INVOKE(test_psignal);
    INVOKE(test_psiginfo);
    INVOKE(test_strsignal);
    INVOKE(test_sig2str);
    INVOKE(test_str2sig);
    return 0;
}

void test_sys_siglist(void)
{
    extern char *sys_siglist[];
    printf("sys_siglist[SIGCHLD] = %s\n", sys_siglist[SIGCHLD]);
}

void test_psignal(void)
{
    psignal(SIGSEGV, "SIGSEGV");
}
void test_psiginfo(void)
{

}

void test_strsignal(void)
{
    printf("strsignal(SIGILL) = %s\n", strsignal(SIGILL));
}

void test_sig2str(void)
{

}

void test_str2sig(void)
{

}