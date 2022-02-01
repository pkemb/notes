#include <stdio.h>
/*
原来：
    STDOUT_FILENO  ───────>  终端
    fd             ───────>  test.txt

ret_fd = dup2(fd, STDOUT_FILENO);
(1) 关闭 STDOUT_FILENO
    STDOUT_FILENO            终端
    fd             ───────>  test.txt
(2) 
    STDOUT_FILENO  ─┐        终端
    fd             ─┴─────>  test.txt
(3) 返回 STDOUT_FILENO
*/


#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PATH    "test.txt"
#define WRITE_STR "write frome fd\n"

int main(int argc, char *argv[])
{
    int fd = open(PATH, O_RDWR | O_CREAT | O_TRUNC, 0660);
    if (fd < 0)
    {
        printf("open file %s error\n", PATH);
        return 1;
    }

    int ret_fd = dup2(fd, STDOUT_FILENO);
    printf("test out from printf\n");
    printf("fd = %d, ret_fd = %d\n", fd, ret_fd);
    write(fd, WRITE_STR, strlen(WRITE_STR));
    return 0;
}