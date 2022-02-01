#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: %s environment...\n", argv[0]);
        return 1;
    }

    int i = 0;
    char *value = NULL;
    for (i=1; i<argc; i++)
    {
        value = getenv(argv[i]);
        printf("%s=", argv[i]);
        if (value == NULL)
        {
            printf("\n");
        }
        else
        {
            printf("%s\n", value);
        }
    }

    return 0;
}