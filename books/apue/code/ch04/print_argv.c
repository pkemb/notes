/*
直接执行会打印：
argv[0] = ./print_argc

若创建一个符号链接指向 print_argc，例如 symlink_to -> print_argc。
然后执行 symlink_to，将会打印：
argv[0] = ./symlink_to

这就是 busybox 等程序识别不同命令的方式。
*/

#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;
    for (i=0; i<argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    return 0;
}