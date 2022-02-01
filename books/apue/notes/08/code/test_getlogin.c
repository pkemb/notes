#include <unistd.h>
#include <stdio.h>

int main()
{
    char *name = getlogin();
    printf("name = %s\n", name);
    return 0;
}