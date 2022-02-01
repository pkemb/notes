<h1 id=file_notes>
    第08章 进程控制
</h1>

[章节目录](../../README.md#title_ch08 "返回章节目录")
[函数表](func.md "进入函数表")

---

<h2 id=ch_8.2>
    进程标识
</h2>

<table>
    <tr>
        <th>标识</th><th>获取函数</th><th>说明</th>
    </tr>
    <tr>
        <td>进程ID</td>
        <td>pid_t getpid(void);</td>
        <td>非负，唯一，可复用。<br>
            ID为0的进程通常是调度进程，运行在内核，是内核的一部分。<br>
            ID为1的进程通常是init进程，负责在自举内核后启动一个UNIX系统。<br>
            init进程决不会终止，是一个普通的用户进程。</td>
    </tr>
    <tr>
        <td>父进程ID</td>
        <td>pid_t getppid(void);</td>
        <td>调用进程的父进程ID。</td>
    </tr>
    <tr>
        <td>实际用户ID</td>
        <td>uid_t getuid(void);</td>
        <td>调用进程的实际用户ID</td>
    </tr>
    <tr>
        <td>有效用户ID</td>
        <td>uid_t geteuid(void);</td>
        <td>调用进程的有效用户ID。</td>
    </tr>
    <tr>
        <td>实际组ID</td>
        <td>gid_t getgid(void);</td>
        <td>调用进程的实际组ID。</td>
    </tr>
    <tr>
        <td>有效组ID</td>
        <td>gid_t getegid(void);</td>
        <td>调用进程的有效组ID。</td>
    </tr>
</table>

注：
* 使用以上函数需要包含头文件 unistd.h
* 都没有出错返回。

示例代码：<a href="code/test_getpid.c">test_getpid.c</a>

<h2 id=ch_8.3>
    函数 fork
</h2>

```c
pid_t fork(void);

头文件：unistd.h
功能：创建一个新进程。
返回值：父进程返回子进程的ID，子进程返回0。若出错返回-1。
```

* 子进程和父进程继续制定fork()之后的指令。
* 子进程是父进程的副本。
> 子进程和父进程共享正文段；子进程获得父进程数据空间、堆和栈的副本（不是共享），采用写时复制（CoW）的技术。
* 子进程和父进程的执行先后顺序是不确定的。如需同步，需要某种形式的进程间通信。
* 父进程所有打开的文件描述符都被复制到子进程中，并且共享同一个文件表项。所以文件偏移量也是共享的。

*注意理解书中的例程，fork与 I/O 函数之间的交互关系。*
* 在fork之前，输出一串字符（含换行符）到缓冲区。
* 如果是行缓冲，会立即flush，不会复制到子进程。
* 如果是全缓冲，会复制一份到子进程。那么，父进程和子进程都会输出一份。

fort的两种常见用法：
1) 一个父进程希望复制自己，使父进程和子进程同时执行不同的代码段。如网络服务进程。
2) 一个进程要执行一个不同的程序，子进程在 fork 返回后立即执行 exec。如shell。

子进程继承了父进程的绝大部分属性，但有以下不同：
* 子进程的 tms_utims tms_stime tms_cutime tms_ustime设置为0。
* 子进程不继承父进程设置的文件锁。
* 子进程的未处理闹钟被清楚。
* 子进程的未处理信号集设置为空集。

示例代码：<a href="code/test_fork.c">test_fork.c</a>

<h2 id=ch_8.4>
    函数vfork
</h2>

* 调用序列与 fork() 相同，但是语义不同。
* SUSv4中删除了此函数，可移植的程序不应该使用这个函数。
* vfork函数用于创建一个新进程，目的是 exec 一个新程序。
* vfork 不将父进程的地址空间完全复制到子进程中。
* vfork 保证子进程先运行，子进程调用exec或exit后，父进程才可以运行。
* 子进程在调用exec或exit之前，子进程运行在父进程的地址空间。
* 注意书中的例子，调用 _exit 而不是 exit。

<h2 id=ch_8.5>
    函数exit
</h2>

进程的5种正常退出方式：
1) main函数中调用 return。
2) 调用 exit 函数。此函数由 ISO C定义，其操作包括调用终止处理程序（atexit登记）、冲洗标准IO流。但是不处理文件描述符、多进程以及作业控制。
3) 调用 _exit 或 _Exit 函数。_Exit 由 ISO C说明，目的是为了提供一种无需运行终止处理程序或信号处理程序而终止的方法。是否冲洗标准IO，取决于实现。UNIX系统中，_Exit 与 _exit 是同义的，不冲洗。_exit 由 exit 调用，处理UNIX特定的细节。
4) 进程的最后一个线程在其启动例程中执行 return 语句。但是线程的返回值不作为进程的返回值。
5) 进程的最后一个线程调用 pthread_exit 函数。

3种异常退出:
1) 调用 abort，产生 SIGABRT 信号。
2) 进程接收到某些信号时。
3) 最后一个线程对“取消”（cancellation）请求作出响应。

无论进程以何种方式退出，最终都会执行内核中的一段代码，为相应进程关闭所有打开的描述符，释放使用的存储器。

子进程通知父进程，它是如何终止的：
* 正常终止：退出状态作为参数传递给exit函数。最后调用 _exit 时，内核将退出状态转换成终止状态。
* 异常终止：内核（不是进程）产生一个指示其异常终止原因的终止状态。
* 父进程可通过 wait 或 waitpid 获取终止状态。
> * 退出状态：传递给3个终止函数的参数，或 main 的返回值。
> * 终止状态：异常终止时，内核（不是进程）产生一个指示其异常终止原因的终止状态。
* 父进程可以对终止的子进程做一些善后处理。
> 一个已终止、但是其父进程尚未对其进行善后处理（获取终止子进程的有关信息、释放它仍占用的资源）的进程被称为僵尸进程。（内核会在进程退出时关闭打开的文件、释放使用的存储区）

父进程先于子进程退出：
* 如果父进程在子进程之前退出，则子进程将由init进程收养。
> 收养的过程：如果一个进程退出，则内核检查所有进程，判断其是否为退出进程的子进程。如果是，将父进程该为 init。
> init 的子进程不会变成僵尸进程。

<h2 id=ch_8.6>
    函数wait和waitpid
</h2>

当一个进程终止时（正常或异常）时，内核会向其父进程发送`SIGCHLD`信号，父进程可以忽略，或者执行信号处理程序。默认情况下是忽略。

```c
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

头文件：sys/wait.h
返回值：成功返回进程ID，出错返回0或-1。
功能：获取子进程的终止状态。
形参说明：
    state：子进程的终止状态。如果不关心，可以设置为NULL。
    pid：
    options：
```

调用 wait 或 waitpid 时，可能会发生：
* wait等待在其调用后第一个终止的进程，并返回进程ID。
* 所有子进程都还在运行，则阻塞。
> 由于接收到SIGCHLD信号而调用wait，会立即返回。如果在随机时间点调用，进程可能会阻塞。
* 如果一个子进程终止，正等待父进程获取其终止状态，则取得该子进程的终止状态并立即返回。
* 如果没有子进程，立即出错返回（-1）。

wait和 waitpid的不同：
* 在一个进程终止前，wait使调用者阻塞；waitpid有一个选项，可使调用者不阻塞。
* waitpid可以不等待在其调用后第一个终止的进程。它有若干个选项，可以控制它等待的进程。

进程的终止状态：
* 通过参数 status 返回.
* 如果不关心进程的终止状态，可将此参数设为NULL。
* status 每位的含义，由实现定义。可以用以下的宏来查看：

<table>
    <tr>
        <th>宏</th><th>说明</th>
    </tr>
    <tr>
        <td>WIFEXITED(status)</td>
        <td>若为正常终止子进程返回的状态，则为真。对于这种情况可执行 WEXITSTATUS(status)，获取子进程传送给exit或_exit参数的低8位。</td>
    </tr>
    <tr>
        <td>WIFSIGNALED(status)</td>
        <td>若为异常终止子进程返回的状态，则为真（接到一个不捕捉的信号）。对于这种情况，WTERMSIG(status)，获取使子进程终止的信号编号。<br>另外，有些实现（非SUS）定义宏WCOREDUMP(status)，若已产生终止进程的core文件，则它返回真。</td>
    </tr>
    <tr>
        <td>WIFSTOPPED(status)</td>
        <td>若为当前暂停子进程的返回的状态，则为真。对于这种情况，可执行WSTOPSIG(status)，获取子进程暂停的信号编号。</td>
    </tr>
    <tr>
        <td>WIFCONTINUED(status)</td>
        <td>若在作业控制暂停后已经继续的子进程返回了状态，则为真（POSIX.1的XSI扩展；仅用于waitpid）。</td>
    </tr>
</table>

waitpid相对于wait，可以等待一个特定的进程。waitpid函数中pid参数的作用：

<table>
    <tr>
        <th>取值</th><th>作用</th>
    </tr>
    <tr>
        <td>pid == -1</td>
        <td>等待任一子进程。此种情况下，waitpid与wait等效。</td>
    </tr>
    <tr>
        <td>pid > 0</td>
        <td>等待进程ID与pid相等的子进程。</td>
    </tr>
    <tr>
        <td>pid == 0</td>
        <td>等待组进程ID等于调用进程组ID的任一子进程。</td>
    </tr>
    <tr>
        <td>pid < -1</td>
        <td>等待组进程ID等于pid绝对值的任一进程。</td>
    </tr>
</table>

注：如果指定的进程或进程组不存在，或pid指定的进程不是调用进程的子进程，可能出错。

options参数：

<table>
    <tr><th>常量</th><th>说明</th></tr>
    <tr>
        <td>WCONTINUED</td>
        <td>若实现作业控制，那么由pid指定的任一子进程在停止后已经继续，但其状态尚未报告，则返回其状态。</td>
    </tr>
    <tr>
        <td>WNOHANG</td>
        <td>若由pid指定的子进程并不是立即可用的，则waitpid不阻塞，此时其返回值为0。</td>
    </tr>
    <tr>
        <td>WUNTRACED</td>
        <td>若某实现支持作业控制，而由pid指定的任一子进程已处于停止状态，并且其状态自停止以来还未报告过，则返回其状态。WIFSTOPPED宏确定返回值是否对应于一个停止的子进程。</td>
    </tr>
</table>

使父进程变为 init 进程的技巧：调用两次 fork。在 first child 调用 fork，则 second child 的父进程变为 init 。

示例代码：<a href=code/test_wait.c>test_wait.c</a> <a href=code/test_waitpid.c>test_waitpid.c</a>

<h2 id=ch_8.7>
    函数 waitid
</h2>

类似于waitpid，但是提供了更多的灵活性。

```c
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

头文件：sys/wait.h
返回值：成功返回0，出错返回-1。(不是返回子进程ID)
功能：获取进程终止状态。
形参说明：
    idtype 决定等待的进程，与 id 配合使用。
    infop 包含了造成子进程状态改变有关信号的详细信息。
```

idtype的取值如下表：

<table>
    <tr><th>常量</th><th>说明</th></tr>
    <tr>
        <td>P_PID</td>
        <td>等待一特定进程，id包含要等待子进程的进程ID。</td>
    </tr>
    <tr>
        <td>P_PGID</td>
        <td>等待一特定进程组中的任一子进程，id包含要等待子进程的进程组ID。</td>
    </tr>
    <tr>
        <td>P_ALL</td>
        <td>等待任一子进程，忽略id。</td>
    </tr>
</table>

options参数是下表参数按位或运算得到：

<table>
    <tr><th>常量</th><th>说明</th></tr>
    <tr>
        <td>WCONTINUED</td>
        <td>等待一进程，它以前曾被停止，此后又继续，但其状态尚未报告。</td>
    </tr>
    <tr>
        <td>WEXITED</td>
        <td>等待已退出的进程。</td>
    </tr>
    <tr>
        <td>WNOHANG</td>
        <td>如无可用的子进程退出状态，立即返回而非阻塞。</td>
    </tr>
    <tr>
        <td>WNOWAIT</td>
        <td>不破坏子进程退出状态，该子进程退出状态可由后续的wait、waitid或waitpid调用取得。</td>
    </tr>
    <tr>
        <td>WSTOPPED</td>
        <td>等待一进程，它已经停止，但其状态尚未报告。</td>
    </tr>
</table>

*注：WCONTINUED、WEXITED、WATOPPED之一必须指定。*

示例代码：<a href="code/test_waitid.c">test_waitid.c</a>

<h2 id=ch_8.8>
    函数 wait3 和 wait4
</h2>

比 wait / waitpid / waitid 的功能多一个：可以返回`终止进程`及其`所有子进程`使用的资源概况。资源统计信息包括 用户CPU时间总量，系统CPU时间总量，缺页次数，接收到信号的次数等。

```c
pid_t wait3(int *status, int options, struct rusage *rusage);
pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage);

头文件：sys/types.h
       sys/wait.h
       sys/time.h
       sys/resource.h
返回值：成功返回进程id，出错返回-1。
形参说明：
    pid：参考 waitpid()。
    status：参考wait()或waitpid()。
    options：参考 waitpid()或waitid()。
    rusage：进程的资源使用概况。
```

wait3可以对标 wait，wait4可以对标 waitpid。

```c
struct rusage {
    struct timeval ru_utime; /* user time used */
    struct timeval ru_stime; /* system time used */
    long ru_maxrss;
    #define ru_first ru_ixrss
    long ru_ixrss; /* XXX: 0 */
    long ru_idrss; /* XXX: sum of rm_asrss */
    long ru_isrss; /* XXX: 0 */
    long ru_minflt; /* any page faults not requiring I/O */
    long ru_majflt; /* any page faults requiring I/O */
    long ru_nswap; /* swaps */
    long ru_inblock; /* block input operations */
    long ru_oublock; /* block output operations */
    long ru_msgsnd; /* messages sent */
    long ru_msgrcv; /* messages received */
    long ru_nsignals; /* signals received */
    long ru_nvcsw; /* voluntary context switches */
    long ru_nivcsw; /* involuntary ” */
    #define ru_last ru_nivcsw
};
```

<h2 id=ch_8.9>
    竞争条件
</h2>

竞争条件：当多个进程都企图对共享数据进行某种处理，而最后的结果又取决于`进程的运行顺序`。

例如，fork之后，以某种逻辑显示或隐式地依赖于父进程先运行，还是子进程先运行。

为了避免竞争条件，在多个进程之间需要有某种形式的信号发送和接收的方法。在UNIX中，可以使用信号机制。

<h2 id=ch_8.10>
    函数exec
</h2>

* 用来执行另外一个程序，新程序从其main函数开始执行。
* exec只是用磁盘上的一个新程序替换了当前的正文段、数据段、堆段和栈段。
* 因为exec不创建新进程，所以进程ID不会发生变化。

```c
#include <unistd.h>

int execl(const char *pathname, const char *arg0, ... /* (char*)0 */);
int execv(const char *pathname, char *const argv[]);
int execle(const char *pathname, const char *arg, ... /* (char *)0, char *const envp[] */);
int execve(const char *pathname, char *const argv[], char *const envp[]);
int execlp(const char *filename, const char *arg0, ... /* (char*)0 */);
int execvp(const char *filename, char *const argv[]);
int fexecve(int fd, char *const argv[], char *const envp[]);

返回值：出错返回-1，成功 不返回 。
```

七个函数之间的区别：
* 前四个函数取路径名作为参数，后两个函数取文件名作为参数，最后一个取文件描述符作为参数。
> * 如果filename中包含/，则视为路径名。否则在PATH指定的目录中查找。
* 参数表的传递不同，l 表示 list，v 表示 vector。
> * execl/execlp/execle要求将新程序的每个命令行参数都说明为一个单独的参数，参数表以空指针结尾。
> * execv/execve/execvp/fexecve要求先构造一个指向各参数的指针数组，该数组地址作为参数。
* 向新程序传递环境表的方式不同。
> * 以e结尾的函数execle/execve/fexecve可以传递一个指针数组，指定环境变量。
> * 其他的函数，execl/execv/execlp/execvp，继承调用进程的 environ 变量。

利用函数中的字母记忆：
* p 取filename作为参数，并且用PATH环境变量寻找可执行文件。
* l 取一个参数表，与 v 互斥。
* v 取一个向量。
* e 取 envp[] 数组，而不使用当前环境。

新程序从调用进程继承的属性
* 进程ID和父进程ID。
* 实际用户ID和实际组ID
* 附属组ID，进程组ID
* 回话ID，控制终端
* 闹钟尚预留的时间
* 当前工作目录，根目录
* 文件模式创建屏蔽字
* 文件锁
* 进程信号屏蔽
* 未处理信号
* 资源限制
* nice值
* tms_utime tms_stime tms_cutime tms_cstime

对文件描述符的处理：若设置了FD_CLOEXEC标志位，则关闭；否则打开。默认保持描述符打开。

进程的有效ID：取决于程序文件的SUID是否设置。如果设置了，有效ID变为程序文件所有者ID。否则不变。有效组ID类似。

示例代码：<a href="code/test_exec.c">test_exec.c</a>

<h2 id=ch_8.11>
    更改用户ID和更改组ID
</h2>

三种ID：
* 实际用户/组ID  ruid rgid
* 有效用户/组ID  euid egid
* 保存的设置用户/组ID  resuid resgid

```c
#include <unistd.h>
int setuid(uid_t uid);
int setgid(gid_t gid);

返回值：成功返回0，出错返回-1。
```

更改用户ID的规则：
* 若进程具有root权限，setuid将`实际用户ID`、`有效用户ID`、`保存的设置用户ID`都设置为`uid`。
* 若进程没有root权限，但是uid等于`实际用户ID`或`保存的设置用户ID`，则将有效用户ID设置为uid。
> 将实际用户ID或保存的设置用户ID传递给有效用户ID。
* 若都不满足，则将errno设置为 EPERM，并返回-1。
* 保存的设置用户ID是exec从有效用户ID复制的。
* 程序文件设置了SUID位时，exec会将有效ID更改为文件的UID。否则euid不变。

总结：
* 实际用户ID
  * root通过setuid()设置为任意ID。
* 有效用户ID
  * root通过setuid设置为任意ID。
  * 普通用户从实际用户ID，或保存的设置用户ID获取。
  * 程序文件设置了SUID位时，exec设置为程序文件的UID。
* 保存的设置用户ID
  * root通过setuid()设置为任意ID。
  * exec从有效用户ID复制。

```c
#include <unistd.h>

int setreuid(uid_t ruid, uid_t euid);
int setregid(gid_t rgid, gid_t egid);
返回值：成功返回0，出错返回-1。
```

规则：一个非特权用户总是能够交换实际用户ID和有效用户ID。

```c
#include <unistd.h>

int seteuid(uid_t uid);
int setegid(gid_t gid);
返回值：成功返回0，出错返回-1。
```

<h2 id=ch_8.11>
    解释器文件
</h2>

解释器文件，interpreter file，文本文件，起始行的格式如下：
```
#! pathname [option argument]
```

* 解释器文件：文本文件，起始行以`#!`开头。
* 解释器：解释器文件第一行的 pathname，通常为绝对路径。
* exec系统调用，会识别解释器文件。
* exec实际上执行的是解释器(pathname)，而不是解释器文件。

例子：假设解释器文件`interpreter.file`的文件内容为：
```
#! /path/to/interpreter arg1 arg2
...
```
按以下命令执行解释器文件：
```
interpreter.file arga argb
```
相当于执行以下命令：
```
/path/to/interpreter arg1 arg2 interpreter.file arga argb
```

部分解释器提供`-f`选项，表示从文件读取程序。例如 awk。

解释器的优缺点：
* 内核增加了额外的开销。
* 可以隐藏程序文件是脚本的事实。
* 解释器文件在效率方面提供了好处。
* 解释器文件让我们可以使用`/bin/sh`以外的其他shell来编写shell脚本。

<h2 id=ch_8.13>
    函数 system
</h2>

```c
#include <stdlib.h>

int system(const char *cmdstrings);
```

可以很方便的在程序中执行一个命令字符串。其对操作系统的依赖很强。

system 在其实现中调用了 fork exec waitpid，因此有三种返回值：
* fork失败或waitpid返回出EINTR之外的出错，则system返回-1，并且设置errno。
* 如果exec失败，则其返回值如同shell执行exit(127)。
* 三个函数都执行成功，那么system的返回值是shell的终止状态。

使用 system 的优点：system进行了所需的各种出错处理以及各种信号处理。

一个安全漏洞：设置了SUID的程序，调用了system函数，其权限在fork和exec之后仍被保持下来。
* 以特殊权限运行的程序，应当使用 fork exec，并且在fork之后，exec之前改回普通权限。
* SUID和SGID的程序，绝对不能调用system函数。
```
假如普通用户user执行程序文件suid_test，所有者是root，suid置位。
此时：
ruid: user
euid: root

假如suid_test调用system函数执行程序program，默认情况下，exec不改变ruid和euid，所以特权会被继承下来。
```

示例代码：<a href="code/test_system.c">test_system.c</a>

<h2 id=ch_8.14>
    进程会计
</h2>

*待填坑，实在没看懂说啥。*

<h2 id=ch_8.15>
    用户标识
</h2>

```c
#include <unistd.h>

char *getlogin(void);

功能：获取运行该程序用户的登录名。
      如果进程没有连接到用户登录时所用的终端，则函数会失败。
```

示例代码：<a href="code/test_getlogin.c">test_getlogin.c</a>

<h2 id=ch_8.16>
    进程调度
</h2>

nice值越小，优先级越高。nice取值在-NZERO ~ NZERO-1之间。默认取值是0。NZERO的典型取值是20。

```c
#include <unistd.h>
int nice(int incr);

返回值：若成功，返回新的nice值。出错，返回-1。
功能：incr 被增加到调用进程的 nice 值上。
      如果incr过大或者过小，将会被自动调整到合理取值。
      具有root权限才可以降低nice值。
      所以普通进程降低nice值的操作是不可逆的（Linux）。
```

注意：由于-1可能是调用成功的返回值。所以，调用之前要清除errno。如果返回-1，同时检查errno的取值是否为0。

```c
#include <sys/resource.h>

int getpriority(int which, id_t who);
返回值：成功返回 -NZERO ~ NZERO-1；出错返回-1。
形参说明：
    which参数控制who参数是如何解释的。
    who参数选择感兴趣的一个或多个进程。

    which 的可能取值有 PRIO_PROCESS PRIO_PGRP PRIO_USER。

int setpriority(int which, id_t who, int value);
返回值：成功返回0，出错返回-1。
功能：为进程、进程组和属于特定用户ID的所有进程设置优先级。
形参说明：
    which：与getpriority相同。
    who：与getpriority相同。
    value：新的nice值，而非增量。
```

子进程是否继承父进程的nice值取决于实现。Linux子进程从父进程中继承nice值。

测试代码：<a href="code/test_nice.c">test_nice.c</a>

<h2 id=ch_8.17>
    进程时间
</h2>

* 墙上时钟时间：又称为时钟时间，是进程运行的时间总量。
* 用户CPU时间：执行用户指令所用的时间量。
* 系统CPU时间：执行内核程序所用的时间量。

```c
#include <sys/times.h>

clock_t times(struct tms *buf);

功能：获取进程自己，以及终止子进程的三个时间。
返回值：成功：返回流逝的墙上时钟时间；
       出错：返回-1。

struct tms {
    clock_t tms_utime;      /* user CPU time */
    clock_t tms_stime;      /* system CPU time */
    clock_t tms_cutime;     /* user CPU time, terminated children */
    clock_t tms_cstime;     /* systme CPU time, terminated children */
};
```

说明：
* 返回的时间均以时钟滴答数为单位，都用_SC_CLK_TCK转换成秒数。
> 由  sysconf 函数返回的每秒时钟滴答数。
* 返回的时间不能使用其绝对值，需使用其相对值。
> 调用times保存返回值。过一段时间再次调用times。新返回值减去就返回值即可。

TODO: 可以尝试阅读`time`命令的实现。

---

[章节目录](../../README.md#title_ch08 "返回章节目录")
[函数表](func.md "进入函数表")
