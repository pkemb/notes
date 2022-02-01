#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef int(*child_func_t)(int argc, char *argv[]);

void pr_exit(int status);
int wait_demo(child_func_t func);

int child_normal_exit(int argc, char *argv[]);
int child_exit_non_0(int argc, char *argv[]);
int child_abort(int argc, char *argv[]);
int child_sigfpe(int argc, char *argv[]);

int main()
{
    int status = 0;
    int ret = 0;

    ret = wait(&status);
    if (ret <= 0)
    {
        printf("wait error, ret = %d\n", ret);
    }

    wait_demo(child_normal_exit);
    wait_demo(child_exit_non_0);
    wait_demo(child_abort);
    wait_demo(child_sigfpe);

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

int wait_demo(child_func_t func)
{
    pid_t pid = 0;
    int status = 0;

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

    if (wait(&status) != pid)
    {
        printf("wait error\n");
    }
    pr_exit(status);
    return 0;
}

int child_normal_exit(int argc, char *argv[])
{
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

int child_exit_non_0(int argc, char *argv[])
{
    return 7;
}