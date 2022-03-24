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
