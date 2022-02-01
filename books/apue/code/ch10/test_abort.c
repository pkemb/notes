/*
验证：SIGABRT的处理程序不返回，而是调用siglongjmp，调用进程是否会异常终止。
结果：不会异常终止，正常终止。
*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

static sigjmp_buf jmpbuf;
static volatile sig_atomic_t canjump = 0;

void sig_abrt(int signo);

int main()
{
    int ret = 0;

    if (signal(SIGABRT, sig_abrt) == SIG_ERR)
    {
        printf("signal(SIGABRT) error\n");
        exit(0);
    }

    ret = sigsetjmp(jmpbuf, 1);
    if (ret == 0)
    {
        canjump = 1;
        printf("before call abort, sigsetjmp ret = %d\n", ret);
        abort();
    }
    printf("after call abort, sigsetjmp ret = %d\n", ret);
    return 0;
}

void sig_abrt(int signo)
{
    if (canjump == 0)
        return;
    
    siglongjmp(jmpbuf, 1);
}