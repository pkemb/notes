#include <sys/stat.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    mode_t old_cmask = 0;
    /* close execute permission for group and others */
    mode_t new_cmask = S_IXOTH | S_IXGRP;

    old_cmask = umask(new_cmask);

    printf("old_cmask = %04o\n", old_cmask);
    printf("new cmask = %04o\n", new_cmask);
    return 0;
}