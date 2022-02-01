#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    pid_t pid = 0;
    int status = 0;

    pid = fork();
    if (pid < 0)
    {
        printf("fork error, ret = %d\n", pid);
        return 1;
    }

    if (pid == 0)
    {
        // child
        exit(1);
    }

    // parent
    // 等待指定退出的进程，不破坏进程的退出状态。
    if (waitid(P_PID, pid, NULL, WEXITED | WNOWAIT) < 0)
    {
        printf("waitid error\n");
        return 1;
    }

    if (wait(&status) != pid)
    {
        printf("wait error\n");
        return 1;
    }

    printf("%d exit, status = 0x%x\n", pid, status);
    return 0;
}