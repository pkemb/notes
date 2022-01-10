/*
 *  将kernel log重定位到指定console，只支持/dev/tty[1-6]
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char bytes[2] = {11, 0};

    if (argc == 2) {
        bytes[1] = atoi(argv[1]);
    } else {
        fprintf(stderr, "%s: need a single arg\n", argv[0]);
        exit(1);
    }
    if (ioctl(STDIN_FILENO, TIOCLINUX, bytes) < 0) {
        fprintf(stderr, "%s: ioctl(stdin, TIOCLINUX): %s\n",
                argv[0], strerror(errno));
        exit(1);
    }
    exit(0);
}
