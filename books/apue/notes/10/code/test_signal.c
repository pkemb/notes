#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

void sig_chld(int signo)
{
    int status;
    pid_t pid;
    printf("signo = %d\n", signo);

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("%d exits, status = %d\n", pid, status);
    }
}

int main()
{
    pid_t pid = 0;
    void (*old_sig_chld)(int) = signal(SIGCHLD, sig_chld);
    if (old_sig_chld == SIG_ERR)
    {
        printf("signal error, errno = %d, %s\n", errno, strerror(errno));
        exit(errno);
    }
    else if (old_sig_chld == SIG_IGN)
    {
        printf("old SIGCHLD handler = SIG_IGN\n");
    }
    else if (old_sig_chld == SIG_DFL)
    {
        printf("old SIGCHLD handler = SIG_DFL\n");
    }
    else
    {
        printf("old SIGCHLD handler = %p\n", old_sig_chld);
    }

    pid = fork();
    if (pid < 0)
    {
        printf("fork error, errno = %d, %s", errno, strerror(errno));
        exit(errno);
    }

    if (pid == 0)
    {
        // child
        exit(0);
    }

    sleep(2);

    return 0;
}