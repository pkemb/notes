#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{
    char cmd[256] = {0};
    int status;
    char *ch;

    while (printf("cmd = "), fgets(cmd, 256, stdin) == cmd)
    {
        ch = strchr(cmd, '\n');
        if (ch)
            *ch = 0;
        //printf("cmd = %s\n", cmd);
        status = system(cmd);
        printf("system ret = 0x%x\n", status);
        if (status == -1)
        {
            printf("system error!!!\n");
        }
        else if (WIFEXITED(status))
        {
            printf("normal termination, exit code = %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("killed by signal, signal = %d\n", WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
            printf("stopped by signal, signal = %d\n", WSTOPSIG(status));
        }
        else
        {
            printf("unknown status\n");
        }
    }
    return 0;
}