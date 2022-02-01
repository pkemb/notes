#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int child_process(void *arg);

int main()
{
    pid_t child = 0;

    printf("before fork\n");

    child = fork();
    if (child < 0)
    {
        printf("fork error, errno = %d\n", errno);
        exit(errno);
    }
    else if (child == 0)
    {
        // child process
        child_process(NULL);
        exit(0);
    }

    // parent
    printf("parent pid  = %d\n", getpid());
    printf("parent ppid = %d\n", getppid());
    return 0;
}

int child_process(void *arg)
{
    printf("child pid  = %d\n", getpid());
    printf("child ppid = %d\n", getppid());
    return 0;
}

/*
输出示例，注意第二种执行方式，“before fork”会输出两次。
./test_fork
before fork
parent pid  = 22151
child pid  = 22152
parent ppid = 10
child ppid = 22151

./test_fork > out.txt
cat out.txt
before fork
parent pid  = 22153
parent ppid = 10
before fork
child pid  = 22154
child ppid = 22153
*/
