#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

typedef int(*child_func_t)(int argc, char *argv[]);

void pr_exit(int status);
int fork_demo(child_func_t func);

int child_abort(int argc, char *argv[]);
int child_sigfpe(int argc, char *argv[]);

void sig_chld(int sig)
{
    int status = 0;
    pid_t pid;

    // 如果有多个子进程几乎同时终止，由于信号不排队，
    // 此函数也可以正常处理。
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("child %d terminated\n", pid);
        pr_exit(status);
    }
}

int main()
{
    signal(SIGCHLD, sig_chld);

    fork_demo(child_abort);
    fork_demo(child_sigfpe);

    return 0;
}

void pr_exit(int status)
{
    if (WIFEXITED(status))
    {
        printf("normal termination, exit status = %d\n", 
                WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("abnormal termination, signal number = %d%s\n",
                WTERMSIG(status),
#ifdef WCOREDUMP
                WCOREDUMP(status) ? " (core file generated)" : "");
#else
                "");
#endif
    }
    else if (WIFSTOPPED(status))
    {
        printf("child stopped, signal number = %d\n",
                WSTOPSIG(status));
    }
}

int fork_demo(child_func_t func)
{
    pid_t pid = 0;

    pid = fork();
    if (pid < 0)
    {
        printf("fork error\n");
        return -1;
    }
    else if (pid == 0)
    {
        if (func == NULL)
        {
            exit(0);
        }
        else
        {
            exit(func(0, NULL));
        }
    }
    return 0;
}

int child_abort(int argc, char *argv[])
{
    abort();
    return 0;
}

int child_sigfpe(int argc, char *argv[])
{
    int num = 5;
    num /= 0;
    return 0;
}
