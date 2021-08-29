#include <stdio.h>
#include <stdlib.h>

unsigned long long maxinum = 0;

int main()
{
    unsigned blocksize[] = {1024*1024, 1024, 1};
    int i, count;

    for (i=0; i<3; i++)
    {
        for (count=1; ; count++)
        {
            void *block = malloc(maxinum + blocksize[i]*count);
            if (block)
            {
                maxinum = maxinum + blocksize[i] * count;
                free(block);
            }
            else
            {
                break;
            }
            printf("maxinum malloc size = %lluB, %lluK, %lluM, %lluG\n", 
                    maxinum, 
                    maxinum/1024, 
                    maxinum/(1024*1024),
                    maxinum/(1024*1024*1024));
            printf("i = %d, count = %d\n", i, count);
        }
    }

    printf("maxinum malloc size = %llu bytes\n", maxinum);
    return 0;
}