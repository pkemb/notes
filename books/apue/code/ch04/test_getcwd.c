#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

#ifndef PATH_MAX
#define PATH_MAX    1024
#endif

int main(int argc, char *argv[])
{
    char path[PATH_MAX] = {0};

    if (getcwd(path, PATH_MAX) == NULL)
    {
        printf("getcwd err, errno = %d\n", errno);
    }
    else
    {
        printf("%s\n", path);
    }
    return 0;
}