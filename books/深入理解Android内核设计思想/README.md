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

