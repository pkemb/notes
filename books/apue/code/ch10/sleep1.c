#include <signal.h>
#include <unistd.h>

static void sig_alrm(int signo)
{
    /* nothing to do, just return to wake up the pause. */
}

unisgned int sleep1(unsigned int seconds)
{
    if (signal(SIGALARM, sig_alrm) == SIG_ERR)
        return seconds;
    alarm(seconds);
    pause();
    return (alarm(0));
}