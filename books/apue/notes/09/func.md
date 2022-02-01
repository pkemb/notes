<h1 id=file_func>
    第09章 进程关系
</h1>

[章节目录](../../README.md#title_ch09 "返回章节目录")
[笔记](notes.md "进入笔记")

---

<table>
    <tr>
        <th>序号</th><th>函数原型</th><th>头文件</th><th>说明</th>
    </tr>
    <tr>
        <td>1</td>
        <td>pid_t getpgrp(void);</td>
        <td>unistd.h</td>
        <td>返回调用进程的进程组ID。</td>
    </tr>
    <tr>
        <td>2</td>
        <td>pid_t getpgid(pid_t pid);</td>
        <td>unistd.h</td>
        <td>返回进程组ID。</td>
    </tr>
    <tr>
        <td>3</td>
        <td>int setpgid(pid_t pid, pid_t pgid);</td>
        <td>unistd.h</td>
        <td>加入一个现有进程组，或创建一个新进程组。</td>
    </tr>
    <tr>
        <td>4</td>
        <td>pid_t setsid(void);</td>
        <td>unistd.h</td>
        <td>建立一个新会话。</td>
    </tr>
    <tr>
        <td>5</td>
        <td>pid_t getsid(pid_t pid);</td>
        <td>unistd.h</td>
        <td>获取会话首进程的进程组ID。</td>
    </tr>
    <tr>
        <td>6</td>
        <td>pid_t tcgetpgrp(int fd);</td>
        <td>unistd.h</td>
        <td>获取前台进程组ID。</td>
    </tr>
    <tr>
        <td>7</td>
        <td>int tcsetpgrp(int fd, pid_t pgrpid);</td>
        <td>unistd.h</td>
        <td>设置前台进程组ID。</td>
    </tr>
    <tr>
        <td>8</td>
        <td>pid_t tcgetsid(int fd);</td>
        <td>termios.h</td>
        <td>获取会话首进程的进程组ID。</td>
    </tr>
    <tr>
        <td>序号</td>
        <td>函数原型</td>
        <td>头文件</td>
        <td>说明</td>
    </tr>
</table>