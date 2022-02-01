#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#define PROMPT  "PK% "
#define CMD_BUF 1024

#if DEBUG
#define LOG(fmt, args...)    printf(fmt, args)
#else
#define LOG(fmt, args...)
#endif

int child_process(char *cmd);

int main(int argc, char *argv[])
{
    pid_t child = 0;
    int status = 0;
    char cmd[CMD_BUF] = {0};

    while (1)
    {
        printf("%s", PROMPT);
        if (fgets(cmd, CMD_BUF, stdin) == NULL)
        {
            LOG("fget return null, pid = %d\n", getpid());
            fflush(stdin);
            return 1;
        }

        char *c = strchr(cmd, '\n');
        if (c)
        {
            c = 0;
        }
        else
        {
            printf("cmd is too long\n");
            continue;
        }

        child = fork();
        if (child < 0)
        {
            LOG("fork error, errno = %d, %s\n", errno, strerror(errno));
            exit(1);
        }

        if (child == 0)
        {
            exit(child_process(cmd));
        }

        // parent
        if (child != waitpid(child, &status, 0))
        {
            LOG("wait error, errno = %d, %s\n", errno, strerror(errno));
            exit(1);
        }
        LOG("child status = 0x%x\n", status);
    }
    

    return 0;
}

int child_process(char *cmd)
{
    return execl("/bin/sh", "sh", "-c", cmd, NULL);
}