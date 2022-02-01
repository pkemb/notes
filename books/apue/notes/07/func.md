<h1 id=file_func>
    第07章 进程环境
</h1>

[章节目录](../../README.md#title_ch07 "返回章节目录")
[笔记](notes.md "进入笔记")

----

<table>
    <tr>
        <th>序号</th><th>函数原型</th><th>头文件</th><th>说明</th>
    </tr>
    <tr>
        <td>1</td>
        <td>void exit(int status);</td>
        <td>stdlib.h</td>
        <td>ISO C</td>
    </tr>
    <tr>
        <td>2</td>
        <td>void _Exit(int status);</td>
        <td>stdlib.h</td>
        <td>POSIX.1</td>
    </tr>
    <tr>
        <td>3</td>
        <td>void _exit(int status);</td>
        <td>unistd.h</td>
        <td>POSIX.1</td>
    </tr>
    <tr>
        <td>4</td>
        <td>int atexit(void (*func)(void));</td>
        <td>stdlib.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>5</td>
        <td>void *malloc(size_t size);</td>
        <td>stdlib.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>6</td>
        <td>void *calloc(size_t nobj, size_t size);</td>
        <td>stdlib.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>7</td>
        <td>void *realloc(void *ptr, size_t newsize);</td>
        <td>stdlib.h</td>
        <td>newsize 是新存储区的长度，不是新、就存储区长度之差。</td>
    </tr>
    <tr>
        <td>8</td>
        <td>void free(void *ptr);</td>
        <td>stdlib.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>9</td>
        <td>char *getenv(const char *name);</td>
        <td>stdlib.h</td>
        <td>获取环境变量</td>
    </tr>
    <tr>
        <td>10</td>
        <td>int putenv(char *ptr);</td>
        <td>stdlib.h</td>
        <td>设置环境变量</td>
    </tr>
    <tr>
        <td>11</td>
        <td>int setenv(const char *name, const char*value, int rewrite);</td>
        <td>stdlib.h</td>
        <td>设置环境变量</td>
    </tr>
    <tr>
        <td>12</td>
        <td>int unsetenv(const char *name);</td>
        <td>stdlib.h</td>
        <td>删除环境变量name</td>
    </tr>
    <tr>
        <td>13</td>
        <td>int setjmp(jmp_buf env);</td>
        <td>setjmp.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>14</td>
        <td>void longjmp(jmp_buf env, int val);</td>
        <td>setjmp.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>15</td>
        <td>int getrlimit(int resource, struct rlimit *rlptr);</td>
        <td>sys/resource.h</td>
        <td>\</td>
    </tr>
    <tr>
        <td>16</td>
        <td>int setrlimit(int resource const struct rlimit *rlptr);</td>
        <td>sys/resource.h</td>
        <td>\</td>
    </tr>
</table>