#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

void test_access(char *pathname);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s pathname...\n", argv[0]);
        return 1;
    }

    int i = 0;
    for (i=1; i<argc; i++)
    {
        test_access(argv[i]);
    }
    return 0;
}

void test_access(char *pathname)
{
    if (pathname == NULL)
        return;

    printf("%s: ", pathname);

    if (access(pathname, F_OK) != 0)
    {
        printf("don't exits\n");
    }
    else
    {
        int permission = 0;
        if (access(pathname, R_OK) == 0)
        {
            printf("read, ");
            permission |= 0x4;
        }

        if (access(pathname, W_OK) == 0)
        {
            printf("write, ");
            permission |= 0x2;
        }

        if (access(pathname, X_OK) == 0)
        {
            printf("execute");
            permission |= 0x1;
        }

        if (permission == 0)
        {
            printf("not have any permission");
        }
        printf("\n");
            
    }
    printf("\n");    
}

