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
