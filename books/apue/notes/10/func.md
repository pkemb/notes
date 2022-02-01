<h1 id=file_func>
    第10章 信号
</h1>

[章节目录](../../README.md#title_ch10 "返回章节目录")
[笔记](notes.md "进入笔记")

---

<table>
    <tr>
        <th>序号</th><th>函数原型</th><th>头文件</th><th>说明</th>
    </tr>
    <tr>
        <td>1</td>
        <td>void (*signal(int signo, void (*func)(int)))(int);</td>
        <td>signal.h</td>
        <td>设置信号处理程序。</td>
    </tr>
    <tr>
        <td>2</td>
        <td>int kill(pid_t pid, int signo);</td>
        <td>signal.g</td>
        <td>向进程或进程组发送信号。</td>
    </tr>
    <tr>
        <td>3</td>
        <td>int raise(int signo);</td>
        <td>signal.h</td>
        <td>向调用进程发送信号。</td>
    </tr>
    <tr>
        <td>4</td>
        <td>unsigned int alarm(unsigned int seconds);</td>
        <td>unistd.h</td>
        <td>设置闹钟时间，并返回之前的剩余时间。</td>
    </tr>
    <tr>
        <td>5</td>
        <td>int alarm(void);</td>
        <td>unistd.h</td>
        <td>使进程挂起直至捕捉到一个信号。</td>
    </tr>
    <tr>
        <td>6</td>
        <td>int sigemptyset(sigset_t *set);</td>
        <td>signal.h</td>
        <td>删除所有信号。</td>
    </tr>
    <tr>
        <td>7</td>
        <td>int sigfillset(sigset_t *set);</td>
        <td>signal.h</td>
        <td>添加所有信号。</td>
    </tr>
    <tr>
        <td>8</td>
        <td>int sigaddset(sigset_t *set, int signo);</td>
        <td>signal.h</td>
        <td>添加信号。</td>
    </tr>
    <tr>
        <td>9</td>
        <td>int sigdelset(sigset_t *set, int signo);</td>
        <td>signal.h</td>
        <td>删除信号。</td>
    </tr>
    <tr>
        <td>10</td>
        <td>int sigismember(sigset_t *set, int signo);</td>
        <td>signal.h</td>
        <td>测试信号。</td>
    </tr>
    <tr>
        <td>11</td>
        <td>int sigprocmask(int how, const sigset_t *set, sigset_t *oset);</td>
        <td>signal.h</td>
        <td>检测、更改进程的信号屏蔽字。</td>
    </tr>
    <tr>
        <td>12</td>
        <td>int sigpending(sigset_t *set);</td>
        <td>signal.h</td>
        <td>返回进程阻塞、并处于未决状态的信号集。</td>
    </tr>
    <tr>
        <td>13</td>
        <td>int sigaction(int signo, const struct sigaction *act, struct sigaction *oact);</td>
        <td>signal.h</td>
        <td>检查、修改与指定信号相关联的处理动作。</td>
    </tr>
    <tr>
        <td>14</td>
        <td>int sigsetjmp(sigjmp_buf env, int savemask);</td>
        <td>setjmp.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>15</td>
        <td>void siglongjmp(sigjmp_buf env, int val);</td>
        <td>setjmp.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>16</td>
        <td>int sigsuspend(const sigset_t *sigmask);</td>
        <td>signal.h</td>
        <td>等待信号发生，并恢复信号屏蔽字。</td>
    </tr>
    <tr>
        <td>17</td>
        <td>void abort();</td>
        <td>stdlib.h</td>
        <td>使调用进程异常终止。</td>
    </tr>
    <tr>
        <td>18</td>
        <td>unsigned int sleep(unsigned int seconds);</td>
        <td>unistd.h</td>
        <td>挂起调用进程，直到时间超时或被信号中断。</td>
    </tr>
    <tr>
        <td>19</td>
        <td>int nanosleep(const struct timespec *reqtp, struct timespec *remtp);</td>
        <td>time.h</td>
        <td>提供纳秒的时间进度。如果系统不支持，则会取整。</td>
    </tr>
    <tr>
        <td>20</td>
        <td>int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *reqtp, struct timespec *remtp);</td>
        <td>time.h</td>
        <td>针对特定时钟挂起进程。</td>
    </tr>
    <tr>
        <td>21</td>
        <td>int sigqueue(pid_t pid, int signo, union sigal value);</td>
        <td>signal.h</td>
        <td>向进程发送一个可排队的信号。</td>
    </tr>
    <tr>
        <td>22</td>
        <td>void psignal(int signo, char *msg);</td>
        <td>signal.h</td>
        <td>输出msg和信号说明到标准错误。</td>
    </tr>
    <tr>
        <td>23</td>
        <td>void psiginfo(const siginfo_t *info, const char *msg);</td>
        <td>signal.h</td>
        <td>输出msg和siginfo_t说明到标准错误。</td>
    </tr>
    <tr>
        <td>24</td>
        <td>char *strsignal(int signo);</td>
        <td>string.h</td>
        <td>获取信号的说明字符串。</td>
    </tr>
    <tr>
        <td>25</td>
        <td>int sig2str(int singo, char *str);</td>
        <td>signal.h</td>
        <td>将信号编号转换为不带SIG前缀的信号名。</td>
    </tr>
    <tr>
        <td>26</td>
        <td>int str2sig(const char *str, int *signop);</td>
        <td>signal.h</td>
        <td>将不带SIG前缀的信号名，或十进制信号编号的字符串，转换为信号编号。</td>
    </tr>
</table>
