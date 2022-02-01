<h1 id=file_notes>
    第12章 线程控制
</h1>

[章节目录](../../README.md#title_ch12 "返回章节目录")
[函数表](func.md "进入函数表")

---

<h2 id=ch_12.2>
    线程限制
</h2>

为了增强应用程序在不同的操作系统实现之间的可移植性，SUS定义了一些与线程操作有关的限制：

<table>
    <tr><th>限制名称</th><th>描述</th><th>name参数</th></tr>
    <tr>
        <td>PTHREAD_DESTRUCTOR_ITERATIONS</td>
        <td>线程退出时操作系统实现试图销毁线程特定数据的最大次数。</td>
        <td>_SC_THREAD_DESTRUCTOR_ITERATIONS</td>
    </tr>
    <tr>
        <td>PTHREAD_KEYS_MAX</td>
        <td>进程可以创建的键的最大数目。</td>
        <td>_SC_THREAD_KEYS_MAX</td>
    </tr>
    <tr>
        <td>PTHREAD_STACK_MIN</td>
        <td>一个线程的栈可用的最小字节数</td>
        <td>_SC_THREAD_STACK_MIN</td>
    </tr>
    <tr>
        <td>PTHREAD_THREADS_MAX</td>
        <td>进程可以创建的最大线程数</td>
        <td>_SC_THREAD_THREADS_MAX</td>
    </tr>
</table>

<h2 id=ch_12.3>
    线程属性
</h2>

pthread通过设置每个对象关联的不同属性来细调对象的行为。通常，管理这些属性的接口都遵循相同的模式：
* `对象`与它自己类型的`属性对象`进行关联。一个`属性对象`可以代表多个属性。`属性对象`对应用程序来说是`不透明`的。
* 初始化函数：把属性对象设置为默认值。
* 销毁函数：销毁属性对象，释放初始化函数申请的资源。
* get函数：每个属性都有一个从属性对象中获取属性值的函数。
* set函数：每个属性都有一个设置属性值的函数。

线程属性：pthread_attr_t

线程属性操作原语：

```c
int pthread_attr_init(pthread_attr_t *attr);
功能：初始化线程属性，attr包含了操作系统实现支持的所有线程属性的默认值

int pthread_attr_destroy(pthread_attr_t *attr);
功能：反初始化线程属性，如果init有动态内存分配，还会负责释放它们。
如果返回失败，可能会有少量的内存泄漏，并且没有任何的补救措施。
```

线程属性对象支持的属性：
* [detachstate](#detachstate) 线程的分离状态属性
* [stackaddr](#stack) 线程栈的最低地址
* [stacksize](#stack) 线程栈的最小长度
* [guardsize](#guardsize) 线程栈末尾的警戒缓冲区大小（字节数）

<h3 id=detachstate>
    detachstate线程属性
</h3>

分离线程：操作系统在线程退出时回收它所占用的资源，无法获取线程的退出状态。

```c
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
功能：获取线程属性的分离状态属性。

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
功能：设置线程属性的分离状态属性。
PTHREAD_CREATE_DETACHED：以分离状态启动线程
PTHREAD_CREATE_JOINABLE：正常启动线程，可以获取线程的终止状态。
```

<h3 id=stack>
    线程栈属性
</h3>

线程栈属性分为栈的最低地址和栈的大小。

对于支持POSIX的系统来说，可能不支持栈属性。可在编译时测试宏_POSIX_THREAD_ATTR_STACKADDR和_POSIX_THREAD_ATTR_STACKSIZE。或运行时使用sysconf函数，_SC_THREAD_ATTR_STACKADDR、_SC_THREAD_ATTR_STACKSIZE。

```c
int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);
功能：获取栈的最低地址和大小。

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
功能：设置栈的最低地址和大小。

注：
1. stackaddr 是最低地址，但不一定是开始地址。
2. 设置stackaddr属性会导致guardsize属性失效。

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
功能：获取栈的大小

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
功能：设置栈的大小。

注：
1. 只设置栈的大小，可以不用自己处理栈的分配。
2. stacksize不能小于PTHREAD_STACK_MIN。
```

<h3 id=guardsize>
    guardsize线程属性
</h3>

guardsize控制线程末尾之后用以避免栈溢出的扩展内存的大小
* 默认值是系统页的大小
* 可以设置为0
* 如果修改了线程属性stackaddr，会使得此属性失效。
* 修改guardsize，系统可能会取为页大小的整数倍。
* 栈溢出到警戒区域时，会收到信号。

```c
int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
功能：获取guardsize

int pthread_attr_setguardsize(const pthread_attr_t *attr, size_t guardsize);
功能：设置guardsize。系统可能会取为页大小的整数倍。
```

---

[章节目录](../../README.md#title_ch12 "返回章节目录")
[函数表](func.md "进入函数表")