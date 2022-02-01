#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int adjust_nice(int incr)
{
    errno = 0;
    int new_nice = nice(incr);

    printf("incr = %d\n", incr);
    if ((new_nice == -1) && errno != 0)
    {
        printf("adjust nice error, errno = %d, %s\n", errno, strerror(errno));
    }
    printf("nice = %d\n\n", nice(0));
    return new_nice;
}

int main()
{
    int nzero;
#if defined(NZERO)
    nzero = NZERO;
#elif defined(_SC_NZERO)
    nzero = sysconf(_SC_NZERO);
#else
#error NZERO undefined
#endif

    printf("NZERO = %d\n", nzero);
    
    adjust_nice(0);
    adjust_nice(100);
    adjust_nice(-5);
    adjust_nice(-100);
    return 0;
}