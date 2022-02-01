#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "mydebug.h"

void print_stat(const struct stat *file_stat);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s pathname...\n", argv[0]);
        exit(1);
    }

    struct stat file_stat = {0};
    int ret = 0;
    int i = 0;

    for (i=1; i<argc; i++)
    {
        memset(&file_stat, 0, sizeof(file_stat));
        ret = lstat(argv[1], &file_stat);
        if (ret != 0)
        {
            printf("get file %s stat err, errno = %d\n", argv[1], errno);
            exit(errno);
        }

        print_stat(&file_stat);
    }

    return 0;
}

void print_stat(const struct stat *file_stat)
{
    PRINT_LNG(file_stat->st_ino);
}