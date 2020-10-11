# Linux设备驱动程序（第三版）

## 第一章 设备驱动程序简介

设备驱动程序：使某个特定硬件响应一个定义良好的内部编程接口，这些接口完全隐藏了设备的工作细节。

设备驱动程序主要用于提供`机制`，而不是提供`策略`。编写访问硬件的驱动代码时，不要给用户强加任何特定策略。有时候也需要在驱动程序中实现一些策略，例如以字节为单位访问的IO驱动程序。
```
机制：需要提供什么功能
策略：如何使用这些功能
```

### 内核功能划分
* 进程管理
* 内存管理
* 文件系统
* 设备控制：本书讨论的主题
* 网络功能
* ...

### 可装载模块
Linux内核可以在运行时动态的装载（insmod）或卸载（rmmod）模块。模块由目标代码组成（未链接）。Linux内核支持多种模块类型，`设备驱动程序`是其中一种。

### 设备和模块的分类

按数据的访问方式划分：
* 字符设备：以字节为单位进行数据访问。
* 块设备：以块（一般为512B）为单位进行数据访问
* 网络接口：围绕数据包的传输和接收而设计

按支持给定类型设备：
* USB模块
* 串行模块
* SCSI模块
* ...

## 第二章 构造和运行模块

安装Linux 2.6.x 内核
1. 安装编译内核需要的工具

build-essential kernel-package libncurses5-dev libqt3-headers

 build-essential  (基本的编程库（gcc, make等）
 kernel-package   (Debian 系统里生成 kernel-image 的一些配置文件和工具)
 libncurses5-dev  (meke menuconfig要调用的）
 libqt3-headers   (make xconfig要调用的）
 其他工具在升级过程中可以按提示安装

2. 下载内核源代码。[链接](https://mirrors.edge.kernel.org/pub/linux/kernel/v2.6/linux-2.6.34.tar.gz)
3. 配置内核
    将当前内核的配置文件.config拷贝到解压的目录，执行 make menuconfig，并save。

    根据第四章的内容，要开启以下选项：
    * CONFIG_DEBUG_KERNEL
    * CONFIG_DEBUG_SLAB
    * CONFIG_DEBUG_PAGEALLOC
    * CONFIG_DEBUG_SPINLOCK
    * CONFIG_DEBUG_SPINLOCK_SLEEP
    * CONFIG_INIT_DEBUG
    * CONFIG_DEBUG_INFO
    * CONFIG_MAGIC_SYSRQ
    * CONFIG_DEBUG_STACKOVERFLOW
    * CONFIG_DEBUG_STACK_USAGE
    * CONFIG_KALLSYMS
    * CONFIG_IKCONFIG
    * CONFIG_IKCONFIG_PROC
    * CONFIG_ACPI_DEBUG
    * CONFIG_DEBUG_DRIVER
    * CONFIG_SCSI_CONSTANTS
    * CONFIG_INPUT_EVBUG
    * CONFIG_PROFILING
4. 编译并安装新内核
5. 更新GRUB引导列表

