# 搭建设备驱动开发环境

《Linux设备驱动程序（第三版）》以Linux2.6为例，介绍了设备驱动程序的开发。所以需要安装Linux2.6内核，或用此版本内核的Linux发布版本。

安装Linux 2.6.x 内核
1. 安装编译内核需要的工具
* build-essential  (基本的编程库（gcc, make等）
* kernel-package   (Debian 系统里生成 kernel-image 的一些配置文件和工具)
* libncurses5-dev  (meke menuconfig要调用的）
* libqt3-headers   (make xconfig要调用的）
* 其他工具在升级过程中可以按提示安装

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

