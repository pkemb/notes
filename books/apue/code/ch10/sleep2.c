#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

static jmp_buf env_alrm;

static void sig_alrm(int signo)
{
    longjmp(env_alrm, 1);
}

unsigned int sleep2(unsigned int seconds)
{
    int pre_seconds;
    void (*old_sig_alrm)(int) = signal(SIGALRM, sig_alrm);
    if (old_sig_alrm == SIG_ERR)
        return seconds;
    
    if (setjmp(env_alrm) == 0)
    {
        pre_seconds = alarm(seconds);
        pause();
    }
    
    signal(SIGALRM, old_sig_alrm);
    if (pre_seconds > seconds)
        return alarm(pre_seconds - seconds);
    else
        return (alarm(0));
}