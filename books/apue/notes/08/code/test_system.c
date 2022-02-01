#include <stdlib.h>
#include <stdio.h>

void test_system(const char *cmd)
{
    printf("cmd = %s\n", cmd);
    int status = system(cmd);
    printf("status = 0x%08x\n", status);
    printf("\n");
}

int main()
{
    test_system(NULL);
    test_system("date");
    test_system("nosuchcmd");
    test_system("exit 23");
    test_system("exit 260");

    return 0;
}