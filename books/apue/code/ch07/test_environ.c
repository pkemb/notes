#include <stdio.h>

extern char **environ;

int main()
{
    int i = 0;

    for (i=0; environ[i] !=NULL; i++)
    {
        printf("environ[%d] = %s\n", i, environ[i]);
    }
    return 0;
}