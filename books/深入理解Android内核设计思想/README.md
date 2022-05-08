# 深入理解Android内核设计思想

# 第04章 操作系统基础

## 操作系统内存管理基础

* 虚拟内存
  * 逻辑地址
  * 线性地址
  * 物理地址
* 内存保护
* 内存分配与回收：native层与Java层
* mmap
* CoW技术

## Android中的Low Memory Killer

* Linux Kernel中的OOM Killer
  * /proc/\<pid\>/oom_score 数值越低，越晚杀死
  * /proc/\<pid\>/oom_adj   OOM权重
  * oom_score 根据 oom_adj、消耗的内存、占用的CPU时间等因素实时计算出来
  * shrinker https://tinylab.org/lwn-550463/
* Android中的OOM Killer
  * 实现了不同等级的Killer。TODO：源码分析
  * drivers/staging/android/lowmemorykiller.c
  * /sys/module/lowmemorykiller/parameters/adj
  * /sys/module/lowmemorykiller/parameters/minfree

## Android匿名共享内存

在Linux共享内存的基础之上进行的封装，并加入了更多的特性。可以将指定的物理内存分别映射到各个进程自己的虚拟地址空间中，从而便捷地实现进程间地内存共享。

* ashmem设备
  * 设备文件：/dev/ashmem
  * 设备驱动代码：drivers/staging/android/ashmem.c
  * misc字符设备，关键函数：ashmem_open()、ashmem_mmap()、ashmem_ioctl()
* 使用流程
  * 进程A打开/dev/ashmem，设置name和size，然后调用mmap()映射内存
  * 通过binder将进程A的文件描述符发送给进程B
    * binder驱动会在进程A找到文件描述符对应的`struct file`
    * 在进程B找一个空闲的文件描述符并绑定到`struct file`，然后返回B进程的文件描述符给接收者
  * 进程B调用mmap()在自己的虚拟地址空间映射同一块内存

## JNI

Java Native Interface，允许运行于JVM的Java程序去调用（反向亦然）本地代码（C、C++或汇编编写的程序）的编程框架。以下三种情况可能用到JNI：
* 实现平台相关的功能，但Java无法实现
* 复用老旧、非Java编写的库
* 高性能

实现步骤如下，demo可以参考[示例代码TestJNI](code/jni/jni.md)
1. 需要本地实现的Java方法加上`native`声明
2. javac编译
3. javah生成头文件
4. 在本地代码实现`native`方法，并编译成动态链接库
5. 在Java类中加载动态库并调用`native`方法

关于JNI的更多内容，例如JNIEnv、jni数据类型、类型签名、本地代码调用Java函数等，可以参考[官方文档](https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/jniTOC.html)。

# 第05章 Android进程线程和程序内存优化

## Android进程和线程

* 四大组件不是进程的全部，而是进程的零件。从`AndroidManifest.xml`看，四大组件定义在application标签下面。
* 主进程由`Zygote`创建，然后会创建主线程`ActivityThread`。
* 同一个包中的组件将运行在相同的进程空间中。
* 不同包中的组件可以通过一定的方式运行在同一个进程空间中。
  * 在`AndroidManifest.xml`中，可以为`<application>`、`<activity>`、`<service>`、`<reciver>`、`<provider>`指定`android:process`属性，指明想要依存的进程环境。
* 一个Activity应用启动后至少有三个线程：一个主线程和两个Binder线程。

## Handler、MessageQueue、Runnable与Looper

Looper不断获取MessgaeQueue中的一个Messgae，然后由Handler来处理。这些对象的对应关系如下：
* 每个线程只有一个Looper
* 每个Looper只有一个MessageQueue
* 每个MessageQueue中有N个Message
* 每个Message中最多指定一个Handler来处理事件。

**Handler**

源代码：frameworks/base/core/java/android/os/Handler.java。关键成员如下。

```java
public class Handler {
    final Looper mLooper;       // 当前线程的Looper
    final MessageQueue mQueue;  // mLooper.mQueue
    final Callback mCallback;

    // 处理Message
    public void dispatchMessage(Message msg);   // 对消息进行分发
    public void handleMessage(Message msg);     // 对消息进行处理，子类需要实现

    // 发送消息
    // post系列
    final boolean post(Runnable r);
    final boolean postAtTime(Runnable r, long uptimeMillis);
    // send系列
    public final boolean sendEmptyMessage(int what);
    public final boolean sendEmptyMessageDelayed(int what, long delayMillis);
}
```

Handler主要有两个功能：
* 处理Message
* 将某个Messgae压入MessageQueue中

`Handler.dispatchMessage()`根据实际情况，调用不同的函数处理消息，按优先级排列是：
1. Message.callback -> Runnable对象
2. Handler.mCallback
3. Handler.handleMessage

**MessageQueue**

源代码：frameworks/base/core/java/android/os/MessageQueue.java。主要成员如下。

```java
public final class MessageQueue {
    // 构造函数，创建队列。会调用nativeInit()创建一个NativeMessageQueue对象
    MessageQueue(boolean quitAllowed);
    private native static long nativeInit();
    // 元素入队
    boolean enqueueMessage(Message msg, long when);
    // 元素出队
    Message next();
    // 删除元素
    void removeMessages(Handler h, int what, Object object);
    void removeMessages(Handler h, Runnable r, Object object);
    // 销毁队列
    private native static void nativeDestroy(long ptr);
}
```

**Looper**

源代码：frameworks/base/core/java/android/os/Looper.java。Looper的关键成员如下：

```java
public final class Looper {
    // 线程局部变量，存储了当前线程的Looper对象。黑魔法的关键所在。
    static final ThreadLocal<Looper> sThreadLocal = new ThreadLocal<Looper>();
    // ActivityThread线程的Looper对象
    private static Looper sMainLooper;

    // 创建Looper对象并保存到 sThreadLocal
    public static void prepare();
    private static void prepare(boolean quitAllowed);
    // ActivityThread线程专属成员函数
    public static void prepareMainLooper();

    // 获取当前线程的Looper对象
    public static @Nullable Looper myLooper();
    // 获取ActivityThread线程的Looper对象
    public static Looper getMainLooper()

    // 死循环，获取消息并处理
    public static void loop();
}
```

使用`Looper`的典型代码如下。

```java
class LooperThread extends Thread {   // Thread是Runnable对象
    public Handler mHandler;
    public void run() {
        // 1. 创建Looper对象并保存到 sThreadLocal
        Looper.prepare();
        // 2. 重写 Handler.handleMessage() 函数
        mHandler = new Handler() {
            public void handleMessage(Message msg) {
                // 处理消息的地方
            }
        };
        // 3. 开始运行Looper
        Looper.run();
    }
}
```

`Handler`有一个`mLooper`成员，但是在以上三步中，并没有看赋值。关键点就在于`Looper.sThreadLocal`。这是一个`ThreadLocal`，意味着只有本线程的代码才能够访问。在`Handler`的构造函数中，会调用`Looper.myLooper()`获取当前线程的Looper。

Looper将每个线程特有的Looper对象隐藏了起来，并提供了若干`static`函数方便开发人员调用。

## UI主线程——ActivityThread

`ActivityThread`的主要启动代码如下。与普通线程相比，差异主要在`Looper.prepareMainLooper()`。这个函数会在内部调用`Looper.prepare()`，同时将生成的线程局部变量`sThreadLocal`保存到`sMainLooper`。这样其他线程可以通过`Looper.getMainLooper()`获取`ActivityThread`线程的Looper。

```java
// 创建Looper对象并保存到sThreadLocal和sMainLooper
Looper.prepareMainLooper();
...
// 生成ActivityThread()对象，内部同时会生成 final H mH = new H()
ActivityThread thread = new ActivityThread();
thread.attach(false, startSeq);

if (sMainThreadHandler == null) {
    // 放回 mH
    sMainThreadHandler = thread.getHandler();
}
Looper.loop();
```

`Looper.loop()`的实现大致如下：

```java
public final class Looper {
    public static void loop() {
        // 获取当前线程的Looper对象
        final Looper me = myLooper();
        // 获取Looper管理的MsgQueue，这个对象在Looper的构造函数中创建
        final MessageQueue queue = me.mQueue;

        for (;;) {
            // 获取一个消息，可能会阻塞
            Message msg = queue.next();
            if (msg == null) {
                // 没有Msg，直接返回
                return;
            }
            // 调用dispatchMessage()处理Msg。msg.target实际上是一个Handler对象
            msg.target.dispatchMessage(msg);
            // 消息处理完毕，进行回收
            msg.recycleUnchecked();
        }
    }
}
```

## Thread类

线程是操作系统分配CPU资源的调度单位。从Thread类的定义看，Thread实现了Runnable，也就是说Thread是可执行代码。

```java
// libcore/ojluni/src/main/java/java/lang/Thread.java
public class Thread implements Runnable {
    ...
}

// libcore/ojluni/src/main/java/java/lang/Runnable.java
@FunctionalInterface
public interface Runnable {
    public abstract void run();
}
```

使用Thread的两种方法。

**从Thread继承**

定义一个`MyThread`继承自`Thread`，重写`run()`方法，然后调用`start()`。

```java
MyThread thr = new MyThread(...); // MyThread重写run()方法
thr.start()
```

**直接实现Runnable**

用Runnable对象构造出一个Thread对象。

```java
Thread thr = new Thread(new Runnable() {
    @Override
    public void run() {
        // code
    }
});
thr.start();
```

**Thread休眠和唤醒**

控制进程休眠和唤醒的函数有：
* `wait()`
* `notify()`
* `notifyAll()`
* `interrupt()`
* `join()`
* `sleep()`

`wait()`和`notify()`、`notifyAll()`是由`Object()`类定义的，也就是说任何类都有这三个成员函数。调用`wait()`会导致调用线程睡眠，直到其他线程调用`notify()`或`notifyAll()`，wait和notify必须使用同一个Object来调用。官方文档对`wait()`的解释如下。

> Causes the calling thread to wait until another thread calls the notify() or notifyAll() method of the object.

`interrupt()`会中断线程的执行，根据不同的情况，Thread可能会收到exception。

`join()`用来等待指定线程执行完毕。默认一直等待，也可以指定超时时间。

```java
public final void join();
public final void join(long millis, int nanos);
public final void join(long millis);

// 示例
Thread t1 = new Thread(...);
Thread t2 = new Thread(...);
t1.start();
t1.join();   // 等待t1执行完毕
t2.start();
```

`sleep()`用来睡眠指定时间。

```java
public static void sleep(long millis);
public static void sleep(long millis, int nanos);
```


## Android程序的内存管理与优化

**内存使用的限制**

进程申请的heap空间超过`dalvik.vm.heapsize`设定的值，就会触发OOM。

```shell
# Android 9.0
console:/$ getprop dalvik.vm.heapsize
256m
```

查看某个进程的内存使用情况，可以使用`dumpsys`命令。各列含义如下：
* PSS：Proportional Set Size，进程独占的内存页 + 按比例分配与其他进程共享的内存页
* Private RAM：进程独占的内存页
  * Private Dirty：必须常驻内存的页面
  * Private Clean：可能被Page Out的页面

```shell
console:/$ dumpsys meminfo 1
Applications Memory Usage (in Kilobytes):
Uptime: 208504 Realtime: 208504
                   Pss  Private  Private  SwapPss     Heap     Heap     Heap
                 Total    Dirty    Clean    Dirty     Size    Alloc     Free
                ------   ------   ------   ------   ------   ------   ------
  Native Heap      732      732        0        0        0        0        0
  Dalvik Heap        0        0        0        0        0        0        0
        Stack       20       20        0        0
    Other dev      240      192        0        0
   Other mmap      469        4       68        0
      Unknown       56       56        0        0
        TOTAL     1517     1004       68        0        0        0        0

 App Summary
                       Pss(KB)
                        ------
           Java Heap:        0
         Native Heap:      732
                Code:        0
               Stack:       20
            Graphics:        0
       Private Other:      320
              System:      445

               TOTAL:     1517       TOTAL SWAP PSS:        0
```

**Android中的内存泄露与内存监测**

在Android Studio的`Profiler`窗口下，可以为调试的APP新建一个SESSIONS，看到CPU、MEMORY、NETWORK、ENERGY的实时使用率。

![](pic/as-profiler.png)

点击`Memory`，则可以看到一个记录按钮，可以记录heap的转储，或`java/Kotlin`的分配情况。

![](pic/as-profiler-memory.png)

记录的HEAP可以保存为`HPROF`文件，通过`hprof-conv`（在platform-tools文件夹下）工具转化为标准的`J2SE HPROF`文件后，可以用`Memory Analyzer Tool（mat）`打开。

参考：
* https://www.eclipse.org/mat/
* https://blog.csdn.net/shulianghan/article/details/106958491

# 第06章 进程间通信——Binder

`Binder`是Android系统的一种进程间通信方式。类似于网络通信，Binder客户端在与Binder服务器通信之前，需要知道Binder服务器的ID。`ServiceManager`是一个特殊的Binder服务器，其ID是0。通过ServiceManager，可以注册、查询等。

Binder涉及到Linux driver、native、framework，理解起来比较困难。

## 智能指针

智能指针是为了彻底解决使用指针的各种问题，包括没有初始化、资源泄露、反复释放、野指针等等。

```c
// 没有初始化
void *p;
/*********************/
// 资源泄露
int *p = malloc(10);
/*********************/
// 反复释放 / 野指针 1
int *p = malloc(10);
// 使用p做一些事情
free(p);
// p任然指向原来的内存块，但已经释放
// 如果再次访问，行为是未知的
free(p); // 这里会导致程序crash
/*********************/
// 野指针 2
int *p = malloc(10);
int *m = p;  // m和p指向同一个对象
free(p);
p = NULL;
// m 指向的对象已经释放了，但m不为NULL
```

观察以上示例代码，指针最主要的问题是：
* 对象在什么时候释放。
  * 当有指针指向某一个对象的时候，不能够释放；
  * 当没有指针指向对象时，必须要释放
* 释放动作需要程序员手工完成

智能指针的设计思路如下：
* 被引用的对象维护一个引用计数，当计数为0时释放
* 智能指针负责增加、减少引用计数

**强指针StrongPointer**

`sp`是一个模板类，可以指向以`LightRefBase`为基类的对象。

* sp：`system/core/libutils/include/utils/StrongPointer.h`
* 对象：以`system/core/libutils/include/utils/LightRefBase.h`为基类

强指针存在一个问题，如果两个对象互相指向对方，那么这两个对象将永远无法得到释放。

**弱指针wp**

弱指针规定：
* 强引用计数为0时，不论弱引用计数是否为0，对象都可以释放自己
* 弱指针必须升级为强指针，才能访问它所指向的目标对象

弱指针`wp`是一个模板类，可以指向以`RefBase`为基类的对象。这两个类都定义在`system/core/libutils/include/utils/RefBase.h`。

> wp的设计过于复杂，具体设计思路参考书籍。

## 进程间的数据载体Parcel

Parcel是一种数据载体，用于承载希望通过IBinder发送的相关信息。Parcel具有解包和打包的能力，接收方和发送方需要使用对应的接口。

Parcel的Java实现在`frameworks/base/core/java/android/os/Parcel.java`，其实只是对native接口的封装。native的实现在`frameworks/base/core/jni/android_os_Parcel.cpp`。

Parcel提供了非常多的接口，分类如下：
* Parcel对象申请与回收
  * obtain() 从Parcel池中获取一个Parcel对象
  * recycle() 回收到Parcel()池
* Parcel设置相关
  * dataSize() 当前已存储的数据大小
  * setDataCapacity() 设置Parcel的空间大小
  * setDataPosition() 设置读写位置
  * dataAvail() 当前Parcel中的可读数据大小
  * dataCapacity()
  * dataPosition()
* 原始数据类型
  * writeLong() / readLong()
  * writeByte() / readByte()
  * writeDouble() / readDouble()
  * writeString() / readString()
  * ...
* 原始数据数组
* Parcelables
* Bundles
* Active Objects：对象的特殊标记引用
  * Binder
    * writeStrongBinder()
    * writeStrongInterface()
    * readStrongBinder()
  * 文件描述符
    * writeFileDescriptor()
    * readFileDescriptor()
* Untyped Containers：读写标准的任意类型的Java容器
  * writeArray(Object[]) / readArray(ClassLoader)
  * writeList(List) / readList(List, ClassLoader)

Parcel的使用示例如下，以`ServiceManagerProxy`的`getService()`为例。

```java
// frameworks/base/core/java/android/os/ServiceManagerNative.java
class ServiceManagerProxy implements IServiceManager {
    public IBinder getService(String name) throws RemoteException {
        Parcel data = Parcel.obtain();  // 获取Parcel对象
        Parcel reply = Parcel.obtain();
        data.writeInterfaceToken(IServiceManager.descriptor);
        data.writeString(name);         // 写入要发送的数据
        // 发送数据
        mRemote.transact(GET_SERVICE_TRANSACTION, data, reply, 0);
        IBinder binder = reply.readStrongBinder(); // 读取接收端返回的数据
        reply.recycle();   // 回收Parcel对象
        data.recycle();
        return binder;
    }
}
```

## Binder驱动

Binder驱动实现了Binder通信机制。[Binder驱动源码阅读](https://pkemb.com/2022/04/binder-driver/)。

## Binder服务端与客户端

* ProcessState 和 IPCThreadState
* ServiceManager
* IBinder：对Binder的接口抽象
  * BpBinder：从IBinder继承而来，p代表proxy，binder driver的代理
  * BBinder：处理Binder消息。作为BnInterface的父类
* IServiceManager：对ServiceManager的接口抽象
  * BpServiceManager：客户端，发送消息到ServiceManager
  * service manager：纯C写的服务端
* IMediaPlayerService：媒体播放服务的接口
  * BpMediaPlayerService：接口的代理，通过代理可以使用服务端提供的服务
  * BnMediaPlayerService：BBinder的子类，实现onTransact()函数
  * MediaPlayerService：BnMediaPlayerService的子类，实现服务端的业务逻辑。

实现服务端与客户端：
* 纯native：可以参考IMediaPlayerService的实现。邓凡平《深入理解Android 卷1》第168页。
* Java：利用AIDL工具，[Android 接口定义语言 (AIDL)](https://developer.android.google.cn/guide/components/aidl?hl=zh-cn)

匿名服务

# 第07章 Android启动过程

## 第一个系统进程init

`init`是第一个用户进程，主要提供了以下两个服务：
* 通过解析[init.r](#initrc语法)文件来创建其他的服务
* [属性服务](#属性服务)

### init.rc语法

`init.rc`主要由`actions`和`services`组成。当`action`指定的事件发生时，会执行相应的命令。`service`用于启动一个后台服务进程，每个`service`包含若干个约束选项。

**基本语法规则**

* 注释以`#`开头
* 关键字和参数用空格分隔
* 参数中如果包含空格，需要用`\`转义
* 每个语句以行为单位，行尾的`\`表示续行

**actions**

格式如下，以`on`开头，后接一个`event`，之后每一行都是一个命令，直到遇见`on`或`service`表示`action`结束。

表示当指定的事件发生时，会依次指定指定的命令。注意，不是shell命令，而是`init`预先定义的一些命令，后面会列出。

```
on <event>
    command
    command
    ...

# 第二个action
on <event>
    command
    command
```

**event**

`init.rc`支持如下的`event`。

| event | 说明 |
| - | - |
| boot | init程序启动后触发的第一个事件 |
| name=value | 当属性name满足特定value时触发 |
| device-added-[path]<br>device-removed-[path] | 当设备节点添加/删除时触发此事件 |
| service-exited-[name] | 当指定的服务name存在时触发 |

**command**

`init.rc`支持下表所示的命令。

| command  | 说明 |
| - | - |
| `exec <path> [<argument>]` | fork并执行一个程序。这条命令将阻塞直到该程序运行完毕。 |
| export name value | 设置环境变量为指定值，全局有效，后续的进程都会继承。 |
| ifup interface | 启动指定网络接口 |
| import filename | 导入另外一个rc文件 |
| hostname name | 设置主机名为name |
| chdir dir | 更改工作目录 |
| chmmod mod path | 更改文件的权限 |
| chown owner group path | 更改文件所有者和所属组 |
| chroot dir | 更改根目录位置 |
| class_start class | 启动由类名指定的所有相关服务 |
| class_stop class | 停止由类名指定的所有相关服务 |
| domainname name | 设置域名 |
| insmod path | 安装模块 |
| mkdir path [mode] [owner] [group] | 新建目录 |
| mount type device dir [mountoption] | 指定路径上挂载一个设备 |
| setprop name value  | 设置指定的属性值 |
| setrlimit resource cur max | 设置一种资源的使用限制 |
| start serivce | 启动指定服务 |
| stop service | 停止指定服务 |
| symlink target path | 创建软链接path，指向target |
| sysclktz mins_west_of_gmt | 设置基准时间 |
| trigger event  | 触发一个事件 |
| write path string [string]* | 在指定文件写入一个或多个字符串 |

**service**

在特定选项的约束下启动指定程序。格式如下，关键字`service`开头，然后是服务的名字和程序的路径，最后是可选的参数。`name`必须全局唯一，可以作为`start`和`stop`命令的参数。

```
service name path [argument]*
    option
    option
```

**option**

| option | 说明 |
| - | - |
| socket name type pem [user [group]] | 创建一个`/dev/socket/name`的UNIX域套接字，并将fd传递给启动的进程。<br>有效的type值包括dgram、stream和seqpacket。<br>user和group的默认值是0。|
| user username | 切换进程的用户为username，默认是root |
| group groupname [groupname]* | 切换进程的用户组为groupname |
| oneshot | 当服务退出时，不要主动重启 |
| class name | 为该服务指定一个class名。同一个class的所有服务必须同时启动或者停止。默认情况下服务的class名是default。|
| onrestart | 当此服务重启时，执行某些命令 |

### init.rc示例

`/init`在`system/core/rootdir/init.rc`。

```
import /init.environ.rc
import /init.usb.rc
import /init.${ro.hardware}.rc
import /vendor/etc/init/hw/init.${ro.hardware}.rc
import /init.usb.configfs.rc
import /init.${ro.zygote}.rc

on init
    sysclktz 0

    # Mix device-specific information into the entropy pool
    copy /proc/cmdline /dev/urandom
    copy /default.prop /dev/urandom

    symlink /system/bin /bin
    symlink /system/etc /etc

service ueventd /sbin/ueventd
    class core
    critical
    seclabel u:r:ueventd:s0
    shutdown critical
```

### init.rc解析

分析`init`的源码，略。

### 属性服务

类似于Windows的注册表，可以用于存储key及其对应的value。`getprop`命令用于读取属性值，`setprop`命令用于设置属性值。

```shell
console:/ # getprop ro.hardware
sun50iw6p1
console:/ # setprop myname pk
console:/ # getprop myname
pk
```

## 系统关键服务的启动

### ServiceManager

### Zygote 孕育新的线程和进程

* zygote的init.rc，启动app_process
* 解析参数，启动Android虚拟机，加载ZygoteInit或RuntimeInit
* ZygoteInit 运行在Java环境
  * 注册一个socket，用于接收消息
  * 预加载资源，包括Classes、Resources、OpenGL、SharedLibraries等，这一步非常非常非常耗时
  * 调用forkSystemServer启动SystemServer
    * 子进程：system_server进程，初始化binder通信、启动各种service，从Binder读取请求并处理
    * 父进程：zygote进程，从socket中读取消息并创建应用程序。理论上，所有的应用程序都要通过zygote启动。

### vold和external storage存储设备

# 第08章 ActivityManagerService(AMS)

AMS是用于管理四大组件运行状态的系统进程，systemServer负责启动AMS。故AMS是运行在systemServer中的一个线程。

```java
public final class SystemServer {
    private void startBootstrapServices() {
        // ...
        // 这里最终会调用 ActivityManagerService.start() 函数
        mActivityManagerService = mSystemServiceManager.startService(
                ActivityManagerService.Lifecycle.class).getService();
        mActivityManagerService.setSystemServiceManager(mSystemServiceManager);
        mActivityManagerService.setInstaller(installer);
        // ...
        // 向SM添加activity、meminfo、dbinfo等若干个service
        mActivityManagerService.setSystemProcess();
    }
}
```

AMS的主要功能：
1. 四大组件状态管理，启动、停止等等。例如startActivity()。
2. 四大组件状态查询
3. Task相关
4. 其他内容，例如getMemoryInfo()

## Activity

AMS通过`ActivityStack`来管理所有的Activity状态，`ActivityStackSupervisor`负责管理所有的`ActivityStack`。每个Activity对应一个`ActivityRecord`，通过ArrayList来描述各种状态下的Activity集合。

```java
public class ActivityStack {
    final ArrayList<ActivityRecord> mLRUActivities = new ArrayList();
    final ArrayList<ActivityRecord> mWaitingVisibleActivities = new ArrayList();
    final ArrayList<ActivityRecord> mStoppingActivities = new ArrayList();
    final ArrayList<ActivityRecord> mGoingToSleepActivities = new ArrayList();
    final ArrayList<ActivityRecord> mNoAnimActivities = new ArrayList();
    final ArrayList<ActivityRecord> mFinishingActivities = new ArrayList();
    // 特殊状态的ActivityRecord
    ActivityRecord mPausingActivity = null;
    ActivityRecord mLastPausedActivity = null;
    ActivityRecord mResumedActivity = null;
    ActivityRecord mLastStartedActivity = null;
}
```

TODO：startActivity()的流程

## ActivityTask

## am指令

```shell
console:/ # am help
Activity manager (activity) commands:
  help
      Print this help text.
  start-activity [-D] [-N] [-W] [-P <FILE>] [--start-profiler <FILE>]
          [--sampling INTERVAL] [--streaming] [-R COUNT] [-S]
          [--track-allocation] [--user <USER_ID> | current] <INTENT>
      Start an Activity.  Options are:
      -D: enable debugging
      -N: enable native debugging
      -W: wait for launch to complete
      --start-profiler <FILE>: start profiler and send results to <FILE>
      --sampling INTERVAL: use sample profiling with INTERVAL microseconds
          between samples (use with --start-profiler)
      --streaming: stream the profiling output to the specified file
          (use with --start-profiler)
      -P <FILE>: like above, but profiling stops when app goes idle
      --attach-agent <agent>: attach the given agent before binding
      --attach-agent-bind <agent>: attach the given agent during binding
      -R: repeat the activity launch <COUNT> times.  Prior to each repeat,
          the top activity will be finished.
      -S: force stop the target app before starting the activity
      --track-allocation: enable tracking of object allocations
      --user <USER_ID> | current: Specify which user to run as; if not
          specified then run as the current user.
      --windowingMode <WINDOWING_MODE>: The windowing mode to launch the activity into.
      --activityType <ACTIVITY_TYPE>: The activity type to launch the activity as.
  start-service [--user <USER_ID> | current] <INTENT>
      Start a Service.  Options are:
      --user <USER_ID> | current: Specify which user to run as; if not
          specified then run as the current user.
  start-foreground-service [--user <USER_ID> | current] <INTENT>
      Start a foreground Service.  Options are:
      --user <USER_ID> | current: Specify which user to run as; if not
          specified then run as the current user.
  stop-service [--user <USER_ID> | current] <INTENT>
      Stop a Service.  Options are:
      --user <USER_ID> | current: Specify which user to run as; if not
          specified then run as the current user.
  broadcast [--user <USER_ID> | all | current] <INTENT>
      Send a broadcast Intent.  Options are:
      --user <USER_ID> | all | current: Specify which user to send to; if not
          specified then send to all users.
      --receiver-permission <PERMISSION>: Require receiver to hold permission.
  instrument [-r] [-e <NAME> <VALUE>] [-p <FILE>] [-w]
          [--user <USER_ID> | current] [--no-hidden-api-checks]
          [--no-window-animation] [--abi <ABI>] <COMPONENT>
      Start an Instrumentation.  Typically this target <COMPONENT> is in the
      form <TEST_PACKAGE>/<RUNNER_CLASS> or only <TEST_PACKAGE> if there
      is only one instrumentation.  Options are:
      -r: print raw results (otherwise decode REPORT_KEY_STREAMRESULT).  Use with
          [-e perf true] to generate raw output for performance measurements.
      -e <NAME> <VALUE>: set argument <NAME> to <VALUE>.  For test runners a
          common form is [-e <testrunner_flag> <value>[,<value>...]].
      -p <FILE>: write profiling data to <FILE>
      -m: Write output as protobuf to stdout (machine readable)
      -f <Optional PATH/TO/FILE>: Write output as protobuf to a file (machine
          readable). If path is not specified, default directory and file name will
          be used: /sdcard/instrument-logs/log-yyyyMMdd-hhmmss-SSS.instrumentation_data_proto
      -w: wait for instrumentation to finish before returning.  Required for
          test runners.
      --user <USER_ID> | current: Specify user instrumentation runs in;
          current user if not specified.
      --no-hidden-api-checks: disable restrictions on use of hidden API.
      --no-window-animation: turn off window animations while running.
      --abi <ABI>: Launch the instrumented process with the selected ABI.
          This assumes that the process supports the selected ABI.
  trace-ipc [start|stop] [--dump-file <FILE>]
      Trace IPC transactions.
      start: start tracing IPC transactions.
      stop: stop tracing IPC transactions and dump the results to file.
      --dump-file <FILE>: Specify the file the trace should be dumped to.
  profile [start|stop] [--user <USER_ID> current] [--sampling INTERVAL]
          [--streaming] <PROCESS> <FILE>
      Start and stop profiler on a process.  The given <PROCESS> argument
        may be either a process name or pid.  Options are:
      --user <USER_ID> | current: When supplying a process name,
          specify user of process to profile; uses current user if not specified.
      --sampling INTERVAL: use sample profiling with INTERVAL microseconds
          between samples
      --streaming: stream the profiling output to the specified file
  dumpheap [--user <USER_ID> current] [-n] [-g] <PROCESS> <FILE>
      Dump the heap of a process.  The given <PROCESS> argument may
        be either a process name or pid.  Options are:
      -n: dump native heap instead of managed heap
      -g: force GC before dumping the heap
      --user <USER_ID> | current: When supplying a process name,
          specify user of process to dump; uses current user if not specified.
  set-debug-app [-w] [--persistent] <PACKAGE>
      Set application <PACKAGE> to debug.  Options are:
      -w: wait for debugger when application starts
      --persistent: retain this value
  clear-debug-app
      Clear the previously set-debug-app.
  set-watch-heap <PROCESS> <MEM-LIMIT>
      Start monitoring pss size of <PROCESS>, if it is at or
      above <HEAP-LIMIT> then a heap dump is collected for the user to report.
  clear-watch-heap
      Clear the previously set-watch-heap.
  bug-report [--progress | --telephony]
      Request bug report generation; will launch a notification
        when done to select where it should be delivered. Options are:
     --progress: will launch a notification right away to show its progress.
     --telephony: will dump only telephony sections.
  force-stop [--user <USER_ID> | all | current] <PACKAGE>
      Completely stop the given application package.
  crash [--user <USER_ID>] <PACKAGE|PID>
      Induce a VM crash in the specified package or process
  kill [--user <USER_ID> | all | current] <PACKAGE>
      Kill all background processes associated with the given application.
  kill-all
      Kill all processes that are safe to kill (cached, etc).
  make-uid-idle [--user <USER_ID> | all | current] <PACKAGE>
      If the given application's uid is in the background and waiting to
      become idle (not allowing background services), do that now.
  monitor [--gdb <port>]
      Start monitoring for crashes or ANRs.
      --gdb: start gdbserv on the given port at crash/ANR
  watch-uids [--oom <uid>]
      Start watching for and reporting uid state changes.
      --oom: specify a uid for which to report detailed change messages.
  hang [--allow-restart]
      Hang the system.
      --allow-restart: allow watchdog to perform normal system restart
  restart
      Restart the user-space system.
  idle-maintenance
      Perform idle maintenance now.
  screen-compat [on|off] <PACKAGE>
      Control screen compatibility mode of <PACKAGE>.
  package-importance <PACKAGE>
      Print current importance of <PACKAGE>.
  to-uri [INTENT]
      Print the given Intent specification as a URI.
  to-intent-uri [INTENT]
      Print the given Intent specification as an intent: URI.
  to-app-uri [INTENT]
      Print the given Intent specification as an android-app: URI.
  switch-user <USER_ID>
      Switch to put USER_ID in the foreground, starting
      execution of that user if it is currently stopped.
  get-current-user
      Returns id of the current foreground user.
  start-user <USER_ID>
      Start USER_ID in background if it is currently stopped;
      use switch-user if you want to start the user in foreground
  unlock-user <USER_ID> [TOKEN_HEX]
      Attempt to unlock the given user using the given authorization token.
  stop-user [-w] [-f] <USER_ID>
      Stop execution of USER_ID, not allowing it to run any
      code until a later explicit start or switch to it.
      -w: wait for stop-user to complete.
      -f: force stop even if there are related users that cannot be stopped.
  is-user-stopped <USER_ID>
      Returns whether <USER_ID> has been stopped or not.
  get-started-user-state <USER_ID>
      Gets the current state of the given started user.
  track-associations
      Enable association tracking.
  untrack-associations
      Disable and clear association tracking.
  get-uid-state <UID>
      Gets the process state of an app given its <UID>.
  attach-agent <PROCESS> <FILE>
    Attach an agent to the specified <PROCESS>, which may be either a process name or a PID.
  get-config [--days N] [--device] [--proto]
      Retrieve the configuration and any recent configurations of the device.
      --days: also return last N days of configurations that have been seen.
      --device: also output global device configuration info.
      --proto: return result as a proto; does not include --days info.
  supports-multiwindow
      Returns true if the device supports multiwindow.
  supports-split-screen-multi-window
      Returns true if the device supports split screen multiwindow.
  suppress-resize-config-changes <true|false>
      Suppresses configuration changes due to user resizing an activity/task.
  set-inactive [--user <USER_ID>] <PACKAGE> true|false
      Sets the inactive state of an app.
  get-inactive [--user <USER_ID>] <PACKAGE>
      Returns the inactive state of an app.
  set-standby-bucket [--user <USER_ID>] <PACKAGE> active|working_set|frequent|rare
      Puts an app in the standby bucket.
  get-standby-bucket [--user <USER_ID>] <PACKAGE>
      Returns the standby bucket of an app.
  send-trim-memory [--user <USER_ID>] <PROCESS>
          [HIDDEN|RUNNING_MODERATE|BACKGROUND|RUNNING_LOW|MODERATE|RUNNING_CRITICAL|COMPLETE]
      Send a memory trim event to a <PROCESS>.  May also supply a raw trim int level.
  display [COMMAND] [...]: sub-commands for operating on displays.
       move-stack <STACK_ID> <DISPLAY_ID>
           Move <STACK_ID> from its current display to <DISPLAY_ID>.
  stack [COMMAND] [...]: sub-commands for operating on activity stacks.
       start <DISPLAY_ID> <INTENT>
           Start a new activity on <DISPLAY_ID> using <INTENT>
       move-task <TASK_ID> <STACK_ID> [true|false]
           Move <TASK_ID> from its current stack to the top (true) or
           bottom (false) of <STACK_ID>.
       resize <STACK_ID> <LEFT,TOP,RIGHT,BOTTOM>
           Change <STACK_ID> size and position to <LEFT,TOP,RIGHT,BOTTOM>.
       resize-animated <STACK_ID> <LEFT,TOP,RIGHT,BOTTOM>
           Same as resize, but allow animation.
       resize-docked-stack <LEFT,TOP,RIGHT,BOTTOM> [<TASK_LEFT,TASK_TOP,TASK_RIGHT,TASK_BOTTOM>]
           Change docked stack to <LEFT,TOP,RIGHT,BOTTOM>
           and supplying temporary different task bounds indicated by
           <TASK_LEFT,TOP,RIGHT,BOTTOM>
       move-top-activity-to-pinned-stack: <STACK_ID> <LEFT,TOP,RIGHT,BOTTOM>
           Moves the top activity from
           <STACK_ID> to the pinned stack using <LEFT,TOP,RIGHT,BOTTOM> for the
           bounds of the pinned stack.
       positiontask <TASK_ID> <STACK_ID> <POSITION>
           Place <TASK_ID> in <STACK_ID> at <POSITION>
       list
           List all of the activity stacks and their sizes.
       info <WINDOWING_MODE> <ACTIVITY_TYPE>
           Display the information about activity stack in <WINDOWING_MODE> and <ACTIVITY_TYPE>.
       remove <STACK_ID>
           Remove stack <STACK_ID>.
  task [COMMAND] [...]: sub-commands for operating on activity tasks.
       lock <TASK_ID>
           Bring <TASK_ID> to the front and don't allow other tasks to run.
       lock stop
           End the current task lock.
       resizeable <TASK_ID> [0|1|2|3]
           Change resizeable mode of <TASK_ID> to one of the following:
           0: unresizeable
           1: crop_windows
           2: resizeable
           3: resizeable_and_pipable
       resize <TASK_ID> <LEFT,TOP,RIGHT,BOTTOM>
           Makes sure <TASK_ID> is in a stack with the specified bounds.
           Forces the task to be resizeable and creates a stack if no existing stack
           has the specified bounds.
  update-appinfo <USER_ID> <PACKAGE_NAME> [<PACKAGE_NAME>...]
      Update the ApplicationInfo objects of the listed packages for <USER_ID>
      without restarting any processes.
  write
      Write all pending state to storage.

<INTENT> specifications include these flags and arguments:
    [-a <ACTION>] [-d <DATA_URI>] [-t <MIME_TYPE>]
    [-c <CATEGORY> [-c <CATEGORY>] ...]
    [-n <COMPONENT_NAME>]
    [-e|--es <EXTRA_KEY> <EXTRA_STRING_VALUE> ...]
    [--esn <EXTRA_KEY> ...]
    [--ez <EXTRA_KEY> <EXTRA_BOOLEAN_VALUE> ...]
    [--ei <EXTRA_KEY> <EXTRA_INT_VALUE> ...]
    [--el <EXTRA_KEY> <EXTRA_LONG_VALUE> ...]
    [--ef <EXTRA_KEY> <EXTRA_FLOAT_VALUE> ...]
    [--eu <EXTRA_KEY> <EXTRA_URI_VALUE> ...]
    [--ecn <EXTRA_KEY> <EXTRA_COMPONENT_NAME_VALUE>]
    [--eia <EXTRA_KEY> <EXTRA_INT_VALUE>[,<EXTRA_INT_VALUE...]]
        (mutiple extras passed as Integer[])
    [--eial <EXTRA_KEY> <EXTRA_INT_VALUE>[,<EXTRA_INT_VALUE...]]
        (mutiple extras passed as List<Integer>)
    [--ela <EXTRA_KEY> <EXTRA_LONG_VALUE>[,<EXTRA_LONG_VALUE...]]
        (mutiple extras passed as Long[])
    [--elal <EXTRA_KEY> <EXTRA_LONG_VALUE>[,<EXTRA_LONG_VALUE...]]
        (mutiple extras passed as List<Long>)
    [--efa <EXTRA_KEY> <EXTRA_FLOAT_VALUE>[,<EXTRA_FLOAT_VALUE...]]
        (mutiple extras passed as Float[])
    [--efal <EXTRA_KEY> <EXTRA_FLOAT_VALUE>[,<EXTRA_FLOAT_VALUE...]]
        (mutiple extras passed as List<Float>)
    [--esa <EXTRA_KEY> <EXTRA_STRING_VALUE>[,<EXTRA_STRING_VALUE...]]
        (mutiple extras passed as String[]; to embed a comma into a string,
         escape it using "\,")
    [--esal <EXTRA_KEY> <EXTRA_STRING_VALUE>[,<EXTRA_STRING_VALUE...]]
        (mutiple extras passed as List<String>; to embed a comma into a string,
         escape it using "\,")
    [-f <FLAG>]
    [--grant-read-uri-permission] [--grant-write-uri-permission]
    [--grant-persistable-uri-permission] [--grant-prefix-uri-permission]
    [--debug-log-resolution] [--exclude-stopped-packages]
    [--include-stopped-packages]
    [--activity-brought-to-front] [--activity-clear-top]
    [--activity-clear-when-task-reset] [--activity-exclude-from-recents]
    [--activity-launched-from-history] [--activity-multiple-task]
    [--activity-no-animation] [--activity-no-history]
    [--activity-no-user-action] [--activity-previous-is-top]
    [--activity-reorder-to-front] [--activity-reset-task-if-needed]
    [--activity-single-top] [--activity-clear-task]
    [--activity-task-on-home] [--activity-match-external]
    [--receiver-registered-only] [--receiver-replace-pending]
    [--receiver-foreground] [--receiver-no-abort]
    [--receiver-include-background]
    [--selector]
    [<URI> | <PACKAGE> | <COMPONENT>]
```
