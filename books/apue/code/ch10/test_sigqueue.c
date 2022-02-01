#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <ucontext.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void handler_usr1(int sig, siginfo_t *info, void *context)
{
    printf("SIGUSR1\n");
    printf("si_value.sival_int = %d\n", info->si_value.sival_int);
}

int main()
{
    struct sigaction act = {0};
    sigset_t block, old, pending;
    union sigval value;

    act.sa_flags |= SA_SIGINFO;
    act.sa_sigaction = handler_usr1;

    sigemptyset(&block);
    sigemptyset(&old);
    sigaddset(&block, SIGUSR1);

    printf("start pending SIGUSR1\n");
    if (sigprocmask(SIG_BLOCK, &block, &old) != 0)
    {
        printf("sigprocmask error\n");
        return 1;
    }

    if (sigaction(SIGUSR1, &act, NULL) != 0)
    {
        printf("sigaction error\n");
        return 1;
    }

    printf("send SIGUSR1 twice\n");
    int i = 0;
    for (i=0; i<2; i++)
    {
        value.sival_int = i+1;
        if (sigqueue(getpid(), SIGUSR1, value) != 0)
        {
            printf("sigqueue error, errno = %s\n", strerror(errno));
            return 1;
        }
        sleep(1);
    }

    sigemptyset(&pending);
    if (sigpending(&pending) != 0)
    {
        printf("sigpending error\n");
        return 1;
    }

    if (sigismember(&pending, SIGUSR1))
    {
        printf("SIGUSR1 is pending\n");
    }

    printf("end SIGUSR1 pending\n");
    sigprocmask(SIG_SETMASK, &old, NULL);

    return 0;
}