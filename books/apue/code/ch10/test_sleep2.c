#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

unsigned int sleep2(unsigned int);
static void sig_int(int signo);

int main(void)
{
    unsigned int unslept;

    if (signal(SIGINT, sig_int) == SIG_ERR)
    {
        printf("signal err = %s\n", strerror(errno));
        return 1;
    }

    unslept = sleep2(5);
    printf("sleep2 returned: %u\n", unslept);
    return 0;
}

static void sig_int(int signo)
{
    int i, j;
    volatile int k;

    printf("\n%s starting\n", __FUNCTION__);
    for (i=0; i < 30000000; i++)
        for (j=0; j < 30000000; j++)
            k += i*j;
    printf("\n%s finished\n", __FUNCTION__);
}