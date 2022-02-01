#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ucontext.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/syscall.h>

int test_sigaction(void);

int main()
{
    test_sigaction();
    kill(getpid(), SIGILL);
    raise(SIGINT);
    raise(SIGSEGV);
    raise(SIGTERM);
    raise(SIGFPE);
    raise(SIGPIPE);
    raise(SIGABRT);
    return 0;
}

struct sigreginfo_t
{
    int                is_register;
    struct sigaction  *old_sa;
    void (*sighandler)(int, siginfo_t *, void*);
};

static struct sigaction old_int   = {0};
static struct sigaction old_term  = {0};
static struct sigaction old_ill   = {0};
static struct sigaction old_segv  = {0};
static struct sigaction old_fpe   = {0};
static struct sigaction old_pipe  = {0};
static struct sigaction old_abrt  = {0};
static struct sigaction old_trap  = {0};
static struct sigaction old_chld  = {0};

static void sighandler_int(int signo, siginfo_t *info, void *context);
static void sighandler_term(int signo, siginfo_t *info, void *context);
static void sighandler_ill(int signo, siginfo_t *info, void *context);
static void sighandler_segv(int signo, siginfo_t *info, void *context);
static void sighandler_fpe(int signo, siginfo_t *info, void *context);
static void sighandler_pipe(int signo, siginfo_t *info, void *context);
static void sighandler_abrt(int signo, siginfo_t *info, void *context);
static void sighandler_trap(int signo, siginfo_t *info, void *context);
static void sighandler_chld(int signo, siginfo_t *info, void *context);

static char *si_code2str(int signo, int si_code);

static struct sigreginfo_t sigregarray[] = 
{
    [SIGINT] = 
    {
        .is_register   = 1,
        .old_sa        = &old_int,
        .sighandler    = sighandler_int,
    },
    [SIGTERM] = 
    {
        .is_register   = 1,
        .old_sa        = &old_term,
        .sighandler    = sighandler_term,
    },
    [SIGILL] = 
    {
        .is_register   = 1,
        .old_sa        = &old_ill,
        .sighandler    = sighandler_ill,
    },
    [SIGSEGV] = 
    {
        .is_register   = 1,
        .old_sa        = &old_segv,
        .sighandler    = sighandler_segv,
    },
    [SIGFPE] = 
    {
        .is_register   = 1,
        .old_sa        = &old_fpe,
        .sighandler    = sighandler_fpe,
    },
    [SIGPIPE] = 
    {
        .is_register   = 1,
        .old_sa        = &old_pipe,
        .sighandler    = sighandler_pipe,
    },
    [SIGABRT] = 
    {
        .is_register   = 1,
        .old_sa        = &old_abrt,
        .sighandler    = sighandler_abrt,
    },
    [SIGTRAP] = 
    {
        .is_register   = 1,
        .old_sa        = &old_trap,
        .sighandler    = sighandler_trap,
    },
    [SIGCHLD] = 
    {
        .is_register   = 1,
        .old_sa        = &old_chld,
        .sighandler    = sighandler_chld,
    },
};
static int sigregarray_len = sizeof(sigregarray)/sizeof(struct sigreginfo_t);

int test_sigaction(void)
{
    struct sigaction sa = {0};
    int ret = 0;
    int sig = 0;

    sa.sa_flags    |= SA_SIGINFO;
    //sa.sa_flags    |= SA_RESTART;
    sigemptyset(&sa.sa_mask);

    for (sig = 0; sig < sigregarray_len; sig++)
    {
        if (sigregarray[sig].is_register)
        {
            sa.sa_sigaction = sigregarray[sig].sighandler;
            ret = sigaction(sig, &sa, sigregarray[sig].old_sa);
            if (ret != 0)
                return ret;
        }
    }

    return 0;
}

static void sighandler_int(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGINT)
        return;

    ucontext_t *uc = (ucontext_t *)context;
    printf("Interrupt from keyboard\n");
    printf("si_code = %s\n", si_code2str(SIGINT, info->si_code));
    tcdrain(1);
}

static void sighandler_term(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGTERM)
        return;

    ucontext_t *uc = (ucontext_t *)context;
    printf("Terminaion signal\n");
    printf("si_code = %s\n", si_code2str(SIGTERM, info->si_code));
    tcdrain(1);
}

static void sighandler_ill(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGILL)
        return;

    ucontext_t *uc = (ucontext_t *)context;
    printf("Illegal Instruction\n");
    printf("si_code = %s\n", si_code2str(SIGILL, info->si_code));
    tcdrain(1);
}

static void sighandler_segv(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGSEGV)
        return;

    ucontext_t *uc = (ucontext_t *)context;
    printf("Invalid memory reference!\n");
    printf("si_code = %s\n", si_code2str(SIGSEGV, info->si_code));
    tcdrain(1);
}

static void sighandler_fpe(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGFPE)
        return;
    
    printf("Termination signal\n");
    printf("si_code = %s\n", si_code2str(SIGFPE, info->si_code));
    tcdrain(1);
}

static void sighandler_pipe(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGPIPE)
        return;
    
    printf("SIGPIPE signal\n");
    printf("si_code = %s\n", si_code2str(SIGPIPE, info->si_code));
    tcdrain(1);
}

static void sighandler_abrt(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGABRT)
        return;
    
    printf("Abort signal\n");
    printf("si_code = %s\n", si_code2str(SIGABRT, info->si_code));
    tcdrain(1);
}

static void sighandler_trap(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGTRAP)
        return;
    
    printf("SIGTRAP signal\n");
    printf("si_code = %s\n", si_code2str(SIGTRAP, info->si_code));
    tcdrain(1);
}

static void sighandler_chld(int signo, siginfo_t *info, void *context)
{
    if (signo != SIGCHLD)
        return;
    
    printf("SIGCHLD signal\n");
    printf("si_code = %s\n", si_code2str(SIGCHLD, info->si_code));
    tcdrain(1);
}

static char *si_code2str(int signo, int si_code)
{
    #define BUF_LEN     64
    #define CASE_SI_CODE(__si_code) \
        case __si_code: ret = #__si_code; break
    
    char *ret = "";
    static char buf[BUF_LEN] = {0};
    memset(buf, 0, BUF_LEN);
    switch (signo)
    {
        case SIGILL:
        {
            switch (si_code)
            {
                #ifdef ILL_ILLOPC
                CASE_SI_CODE(ILL_ILLOPC);  /*非法操作码 */
                #endif
                #ifdef ILL_ILLOPN
                CASE_SI_CODE(ILL_ILLOPN);  /* 非法操作数 */
                #endif
                #ifdef ILL_ILLADR
                CASE_SI_CODE(ILL_ILLADR);  /* 非法地址模式 */
                #endif
                #ifdef ILL_ILLTRP
                CASE_SI_CODE(ILL_ILLTRP);  /* 非法陷入 */
                #endif
                #ifdef ILL_PRVOPC
                CASE_SI_CODE(ILL_PRVOPC);  /* 特权操作码 */
                #endif
                #ifdef ILL_PRVREG
                CASE_SI_CODE(ILL_PRVREG);  /* 特权寄存器 */
                #endif
                #ifdef ILL_COPROC
                CASE_SI_CODE(ILL_COPROC);  /* 协处理器出错 */
                #endif
                #ifdef ILL_BADSTK
                CASE_SI_CODE(ILL_BADSTK);  /* 内部栈出错 */
                #endif
                default: ret = ""; break;
            }
            break;
        }
        case SIGFPE:
        {
            switch (si_code)
            {
                #ifdef FPE_INTDIV
                CASE_SI_CODE(FPE_INTDIV); /* 整数除以0 */
                #endif
                #ifdef FPE_INTOVF
                CASE_SI_CODE(FPE_INTOVF); /* 整数溢出 */
                #endif
                #ifdef FPE_FLTDIV
                CASE_SI_CODE(FPE_FLTDIV); /* 浮点除以0 */
                #endif
                #ifdef FPE_FLTOVF
                CASE_SI_CODE(FPE_FLTOVF); /* 浮点向上溢出 */
                #endif
                #ifdef FPE_FLTUND
                CASE_SI_CODE(FPE_FLTUND); /* 浮点向下溢出 */
                #endif
                #ifdef FPE_FLTRES
                CASE_SI_CODE(FPE_FLTRES); /* 浮点不精确结果 */
                #endif
                #ifdef FPE_FLTINV
                CASE_SI_CODE(FPE_FLTINV); /* 无效浮点操作 */
                #endif
                #ifdef FPE_FLTSUB
                CASE_SI_CODE(FPE_FLTSUB); /* 下标超出范围 */
                #endif
                default: ret = ""; break;
            }
            break;
        }
        case SIGSEGV:
        {
            switch (si_code)
            {
                #ifdef SEGV_MAPERR
                CASE_SI_CODE(SEGV_MAPERR); /* 地址不映射至对象 */
                #endif
                #ifdef SEGV_ACCERR
                CASE_SI_CODE(SEGV_ACCERR); /* 对于映射对象的无效权限 */
                #endif
                default: ret = ""; break;
            }
            break;
        }
        case SIGBUS:
        {
            switch (si_code)
            {
                #ifdef BUS_ADRALN
                CASE_SI_CODE(BUS_ADRALN);  /* 无效地址对齐 */
                #endif
                #ifdef BUS_ADRERR
                CASE_SI_CODE(BUS_ADRERR);  /* 不存在的物理地址 */
                #endif
                #ifdef BUS_OBJERR
                CASE_SI_CODE(BUS_OBJERR);  /* 对象特定硬件错误 */
                #endif
                default: ret = ""; break;
            }
            break;
        }
        case SIGTRAP:
        {
            switch (si_code)
            {
                #ifdef TRAP_BRKPT
                CASE_SI_CODE(TRAP_BRKPT);  /* 进程断点陷入 */
                #endif
                #ifdef TRAP_TRACE
                CASE_SI_CODE(TRAP_TRACE);  /* 进程跟踪陷入 */
                #endif
                default: ret = ""; break;
            }
            break;
        }
        case SIGCHLD:
        {
            switch (si_code)
            {
                #ifdef CLD_EXITED
                CASE_SI_CODE(CLD_EXITED);  /* 子进程已终止 */
                #endif
                #ifdef CLD_KILLED
                CASE_SI_CODE(CLD_KILLED);  /* 子进程已异常终止（无core）*/
                #endif
                #ifdef CLD_DUMPED
                CASE_SI_CODE(CLD_DUMPED);  /* 子进程已异常终止（有core）*/
                #endif
                #ifdef CLD_TRAPPED
                CASE_SI_CODE(CLD_TRAPPED); /* 被跟踪的子进程已陷入 */
                #endif
                #ifdef CLD_STOPPED
                CASE_SI_CODE(CLD_STOPPED); /* 子进程已停止 */
                #endif
                #ifdef CLD_CONTINUED
                CASE_SI_CODE(CLD_CONTINUED); /* 停止的子进程已继续 */
                #endif
            }
            break;
        }
        default: ret = ""; break;
    }

    if (strncmp(ret, "", 1) == 0)
    {
        switch(si_code)
        {
            #ifdef SI_USER
            CASE_SI_CODE(SI_USER);     /* kill发送的信号 */
            #endif
            #ifdef SI_QUEUE
            CASE_SI_CODE(SI_QUEUE);    /* sigqueue发送的信号 */
            #endif
            #ifdef SI_TIMER
            CASE_SI_CODE(SI_TIMER);    /* timer_settime 设置的定时器超时（实时扩展）*/
            #endif
            #ifdef SI_ASYNCIO
            CASE_SI_CODE(SI_ASYNCIO);  /* 异步I/O请求完成 */
            #endif
            #ifdef SI_MESGQ
            CASE_SI_CODE(SI_MESGQ);    /* 一条消息到达消息队列（实施扩展）*/
            #endif
            default: 
                snprintf(buf, BUF_LEN, "0x%x", si_code);
                ret = buf; 
                break;
        }
    }

    return ret;
}