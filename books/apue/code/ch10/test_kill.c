#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int child_exit = 0;
pid_t child = 0;

void sigusr1_handle(int signo)
{
    printf("child received SIGUSR1\n");
    child_exit = 1;
}

int main()
{
    int status = 0;

    void(*old_handle)(int) = signal(SIGUSR1, sigusr1_handle);
    if (old_handle == SIG_ERR)
    {
        printf("install sigusr1 error, err = %s\n", strerror(errno));
        return 1;
    }

    child = fork();
    if (child < 0)
    {
        printf("fork ret = %d, err = %s\n", child, strerror(errno));
        return 1;
    }

    if (child == 0)
    {
        printf("child pid = %d\n", getpid());
        while(!child_exit)
        {
            sleep(1);
        }
        exit(0);
    }
    
    sleep(2);
    printf("send SIGUSR1 to %d\n", child);
    kill(child, SIGUSR1);
    if (waitpid(child, &status, 0) != child)
    {
        printf("wait error, err = %s\n", strerror(errno));
    }
    printf("child exit, status = %d\n", status);
    return 0;
}