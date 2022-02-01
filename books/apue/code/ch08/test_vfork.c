#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int globvar = 6;

int main()
{
    int   var = 88;
    pid_t pid = 0;

    printf("before vfork\n");
    pid = vfork();
    if (pid < 0)
    {
        printf("vfork error, errno = %d\n", errno);
        exit(errno);
    }
    else if(pid == 0)
    {
        // child
        globvar++;
        var++;
        _exit(0);     // not exit()
    }
    
    // parent
    printf("pid = %d, globvar = %d, var = %d\n", getpid(), globvar, var);
    exit(0);
}