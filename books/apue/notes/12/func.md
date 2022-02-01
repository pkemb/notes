<h1 id=file_func>
    第12章 线程控制
</h1>

[章节目录](../../README.md#title_ch12 "返回章节目录")
[笔记](notes.md "进入笔记")

---

<table>
    <tr>
        <th>序号</th><th>函数原型</th><th>头文件</th><th>说明</th>
    </tr>
    <tr>
        <td>1</td>
        <td>int pthread_attr_init(pthread_attr_t *addr);</td>
        <td>pthread.h</td>
        <td>初始化线程属性。</td>
    </tr>
    <tr>
        <td>2</td>
        <td>int pthread_attr_destroy(pthread_attr_t *addr);</td>
        <td>pthread.h</td>
        <td>反初始化线程属性。</td>
    </tr>
    <tr>
        <td>3</td>
        <td>int pthread_attr_getdetachstate(const pthread_addr_t *addr, int *detachstate);</td>
        <td>pthread.h</td>
        <td>获取detachstate</td>
    </tr>
    <tr>
        <td>4</td>
        <td>int pthread_attr_setdetachstate(pthread_addr_t *addr, int detachstate);</td>
        <td>pthread.h</td>
        <td>设置detachstate</td>
    </tr>
    <tr>
        <td>5</td>
        <td>int pthread_addr_getstack(const pthread_addr_t *addr, void **stackaddr, size_t *stacksize);</td>
        <td>pthread.h</td>
        <td>获取栈低地址和大小</td>
    </tr>
    <tr>
        <td>6</td>
        <td>int pthread_addr_setstack(pthread_addr_t *addr, void *stackaddr, size_t stacksize);</td>
        <td>pthread.h</td>
        <td>设置栈的低地址和大小。</td>
    </tr>
    <tr>
        <td>7</td>
        <td>int pthread_addr_getstacksize(const pthread_addr_t *addr, size_t *stacksize);</td>
        <td>pthread.h</td>
        <td>获取栈的大小</td>
    </tr>
    <tr>
        <td>8</td>
        <td>int pthread_addr_setstacksize(pthread_addr_t *addr, size_t stacksize);</td>
        <td>pthread.h</td>
        <td>设置栈的大小。</td>
    </tr>
    <tr>
        <td>9</td>
        <td>int pthread_addr_getguardsize(const pthread_addr_t *addr, size_t *guardsize);</td>
        <td>pthread.h</td>
        <td>获取guardsize</td>
    </tr>
    <tr>
        <td>10</td>
        <td>int pthread_addr_setguardsize(pthread_addr_t *addr, size_t guardsize);</td>
        <td>pthread.h</td>
        <td>设置guardsize</td>
    </tr>
    <tr>
        <td>序号</td>
        <td>函数原型</td>
        <td>头文件</td>
        <td>说明</td>
    </tr>
</table>