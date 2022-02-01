#include <signal.h>
#include <stdio.h>

static volatile sig_atomic_t sigflag;
static sigset_t newmask;
static sigset_t oldmask;
static sigset_t zeromask;

static void sig_usr(int signo)
{
    sigflag = 1;
}

void TELL_WAIT(void)
{
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
    {
        printf("signal(SIGUSR1) error\n");
        return;
    }
    if (signal(SIGUSR2, sig_usr)==SIG_ERR)
    {
        printf("signal(SIGUSR2) error\n");
        return;
    }

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    // block SIGUSR1 SIGUSR2
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    {
        printf("SIG_BLOCK error\n");
        return;
    }
}

void TELL_PARENT(pid_t pid)
{
    kill(pid, SIGUSR2);
}

void WAIT_PARENT(void)
{
    while (sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;

    // reset sigmask to original value
    // 不太明白，这里为什么要设置为 oldmask。
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        printf("SIG_SETMASK error\n");
    }
}

void TELL_CHILD(pid_t pid)
{
    kill(pid, SIGUSR1);
}

void WAIT_CHILD(void)
{
    while (sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;

    // reset sigmask to original value
    // 不太明白，这里为什么要设置为 oldmask。
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        printf("SIG_SETMASK error\n");
    }
}
