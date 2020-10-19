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

### 设置测试系统

关于开发环境的搭建，请参考[搭建设备驱动开发环境](development-environment.md)。

### hello world 模块

以下是hello world模块的[完整代码](code/ch02/hello.c)：
```c
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
    printk(KERN_ALERT "hello world!\n");
}

static int hello_exit(void)
{
    printk(KERN_ALERT "Googbye hello world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
```

最简单的模块包含的内容：
* MODULE_LICENSE()
* 模块初始化函数
* 模块退出函数
* 模块在运行时不能只用C库函数，可以使用kernel提供的函数（例如printk()）
* 加载模块的指令：insmod
* 卸载模块的指令：rmmod

关于模块的构造，请参考[编译和装载](#编译和装载)。

### 核心模块与应用程序的对比

从编程模式看，应用程序一般从头到尾执行单个任务。模块的初始化函数执行完毕后就直接退出了，相当于告诉内核，我在这，我能提供某些服务。

应用程序退出时，无需关系资源的释放，因为kernel会帮助完成资源的释放。而模块必须在退出函数，必须仔细的撤销初始化函数做的工作。

模块会与内核链接起来，但不会和任何函数库链接。所以模块无法使用常见的库和头文件，只能使用kernel提供的函数和头文件。

应用程序的错误一般只会对自己照成影响，而模块的错误可能会导致整个系统宕机。

#### 用户空间和内核空间

模块运行在内核空间，而应用程序运行在用户空间。

这两种模式具有不同的特权等级、不同的地址空间。当发生系统调用或中断时，会从用户空间陷入到内核空间。

#### 内核中的并发

应用程序通常是顺序执行的，无需关心其他事情会改变其运行环境。而模块（内核代码）必须时刻牢记：`同一时刻，可能会有很多事情正在发生`。

原因：
1. Linux通常同时运行多个并发进程，这些进程可能同时在使用驱动程序。
2. 模块可能被中断处理程序打断。
3. 内核中存在一些其他的异步运行进程，例如定时器。
4. 在SMP系统上，可能多个CPU同时在使用模块。

对代码编写的要求：
1. Linux内核代码（包括驱动程序的代码）必须是可重入的，必须同时运行在多个上下文。
2. 处理并发问题的同时，还要避免竞态。

#### 当前进程

内核代码可以通过全局项`current`来获取当前进程，此全局指针定义在`<asm/current.h>`中。

#### 其他一些细节

内核栈非常小，而且自己的函数和整个内核空间调用链共享栈。对于比较大的数据结构，建议动态分配。

有两个下划线（__）前缀的函数，通常是比较底层的实现。可以使用，后果自负。

内核不支持浮点运算，也不需要浮点运算。

### 编译和装载

如何编译模块并将其装载到内核。

#### 编译模块

在构造模块之前，需要确保环境准备妥当：
1. 具备了正确版本的编译器、模块工具和其他必要的工具。具体可以看内核源代码的Documentation/Changes文件。
2. 文件系统需要有内核树，或者配置并构造内核。

关于构造系统更加详细的内容，可以参考内核源代码Documentation/kbuild目录下的文件。

下面是hello world模块的[makefile](code/ch02/Makefile)，直接执行`make`指令，即可构造hello world模块。此makefile会被读取两次，第一次走else分支，第二次走if分支。
```makefile
# 如果已定义 KERNELRELEASE，则说明是从内核构造系统调用的，
# 因此可利用其内建语句。
ifneq($(KERNELRELEASE),)
    obj-m := hello.o
# 否则，是直接从命令行调用的，
# 这时要调用内核构造系统。
else
    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)

default:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
```

#### 装载和卸载模块

insmode：将模块的代码和数据装入内核，用内核的符号表解析未定义的符号。

modprobe：也用户装载模块，如果有未定义符号无法解析，insmode 会直接报错。而modprobe会搜索定义了此符号的模块并装载。

rmmod：从内核中移除模块。如果模块正在使用，或被内核配置未禁止移除，则无法移除模块。

lsmod：列出当前装载到内核的所有模块。有关装载模块的更多信息，可以查看/proc/modules文件和/sys/module目录。

#### 版本依赖

模块可以和内核中的vermagic.o链接，此目标文件包含了大量有关内核的信息。在装载时，可用来检查模块和内核的兼容性。如果不匹配，则拒绝装载模块。

如果要为特定内核版本构造模块，则需要该特定版本对应的构造系统和源代码树。

一些与版本检查相关的弘：
* UTS_RELEASE   描述内核版本的字符串，例如"2.6.10"。
* LINUX_VERSION_CODE    内核版本的二进制表示，2.6.10对应的是0x02060a。
* KERNEL_VERSION(major,minor,release)   利用版本号的三个部分，创建整数版本号。

可以使用条件编译，编写基于特定内核版本的代码。

#### 平台依赖

内核和模块可以针对特定的CPU平台进行特殊的优化，充分利用目标平台的特性。这需要针对目标平台定制编译后才能达到。利用vermagic.o，在装载模块时，内核会检查处理器的相关配置选项以确保匹配运行中的内核。如果不符合，则会拒绝装载。

如果打算编写一个通用的驱动程序，最好考虑一下，如何支持可能的不同CPU平台。

### 内核符号表

装载模块时，insmod会使用公共内核符号表解析未定义的符号。同时，模块也可以导出自己的符号到内核符号表，供其他的模块使用。

新模块可以使用已插入模块的符号，这种技术称为叠层技术。通过叠层技术，可以将模块划分为多个层，通过简化每个层可以缩短开发时间。

使用下列宏，可以方便的将符号导出，可以有效的避免名字空间污染。符号必须在模块的全局部分导出，该符号也必须是全局的。
```c
EXPORT_SYMBOL(name);
EXPORT_SYMBOL_GPL(name);    // 只能被GPL许可证下的模块使用
```

### 预备知识

每个可装载模块都必须包含下面两行代码：
```c
#include <linux/module.h>   // 含有可装载模块需要的大量符号和函数的定义
#include <linux/init.h>     // 指定初始化和清除函数
```

模块应该指定代码所使用的许可证：
```c
MODULE_LICENSE("GPL");
```
内核能够识别的许可证有：
<table>
    <tr><th>许可证</th><th>描述</th></tr>
    <tr><td>GPL</td><td>任一版本的GNU通用公共许可证</td></tr>
    <tr><td>GPL v2</td><td>GPL版本2</td></tr>
    <tr><td>GPL and additional rights</td><td>GPL及附加权力</td></tr>
    <tr><td>Dual BSD/GPL</td><td>BSD/GPL双重许可证</td></tr>
    <tr><td>Dual MPL/GPL</td><td>MPL/GPL双重许可证</td></tr>
    <tr><td>Proprietary</td><td>专有</td></tr>
</table>

可在模块中包含的其他描述性定义：
```c
MODULE_AUTHOR(描述模块作者);
MODULE_DESCRIPTION(用来说明模块用途的简短描述);
MODULE_VERSION(代码修订号); // 有关版本字符串的创建惯例，请参考 <linux/module.h> 中的注释
MODULE_ALIAS(模块的别名);
MODULE_DEVICE_TABLE(用来告诉用户空间模块所支持的设备);
```

上述`MODULE_`声明，需要放置在全局区，习惯上放在文件的最后。

### 初始化和关闭

模块初始化函数负责注册模块所提供的任何`设施`。对于每种设施，对应有具体的内核函数用来完成注册。

初始化函数的典型定义如下：
```c
static int __init initialization_function(void)
{
    // 这是初始化代码
}
module_init(initialization_function); // 必须使用 moudle_init() 注册初始化函数
```

`__init`标记用来告诉内核，此函数仅在初始化期间使用。模块初始化完毕之后，此函数会被丢弃，释放内存空间。类似的标记还有`__initdata`、`__devinit`、`__devinitdata`。如果一个函数在初始化完毕之后还想使用，则不能使用`__init`标记。

#### 清除函数

清除函数负责在模块被移除前注销接口，并向系统返回所有资源。典型定义如下：
```c
static void __exit cleanup_function(void)
{
    // 这是清除代码
}
module_exit(cleanup_function);
```

`__exit`标记表示函数仅用于卸载。如果模块直接内嵌到内核，或禁止卸载，此函数会被丢弃。如果模块没有注册清除函数，则禁止卸载。

#### 初始化过程中的错误处理

向内核注册任何设施时，都有可能会失败，所以必须要检查返回值。

如果注册设施失败，模块应尽可能的向前初始化，通过降低功能来继续运转。如果遇到致命错误，初始化函数需要将已注册的设施释放，并返回一个错误码。

#### 模块装载竞争

初始化函数还在运行的时候，刚刚注册好的设施可能会被其他模块调用。也就是说，在用来支持某个设施的所有内部初始化完成之前，不要注册任何设施。

注册某个设施失败的时候，之前注册好的设施可能真正使用。如果要初始化失败，需要小心处理内核其他部分正在进行的操作，并等待这些操作完成。

### 模块参数

模块可以使用`module_param()`来声明参数。module_param()必须放在任何函数之外，通常在源文件的头部。下面是示例代码，声明了一个整型参数，和一个字符串参数。参数必须要有一个默认值，如果没有指定参数，则使用默认值。
```c
#include <moduleparam.h>
static char *whom = "world";
static int  howmany = 1;
module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGP);
```

可以使用以下命令更改参数：
```shell
insmod modname howmany=10 whom="Mom"
```

module_param()的第一个参数是变量名称，第二个参数是变量类型，第三个参数是sysfs入口项的访问许可掩码。

内核支持的模块参数类型
<table>
    <tr><th>参数类型</th><th>说明</th></tr>
    <tr><td>bool</td><td>布尔值，取true或false。</td></tr>
    <tr><td>invbool</td><td>反转bool值，true变false，false变true。</td></tr>
    <tr><td>charp</td><td>字符指针值。内核会为用户提供的字符串分配内存，并相应设置指针。</td></tr>
    <tr><td>int<br>long<br>short<br>
            uint<br>ulong<br>ushort</td>
            <td>具有不同长度的基本整数值，u开头的用于无符号值。</td></tr>
</table>

可以使用`module_param_array(name,type,num,perm)`声明数组参数。name是数组名，type是数组元素的类型，num是用户提供的值的个数。

### 在用户空间编写驱动程序

编写一个用户进程作为驱动程序，有很多好处，但是也有很多限制，具体参考书籍。通常，用户空间驱动程序被实现为一个服务器进程，替代内核作为硬件控制的唯一代理。

## 第三章 字符设备驱动程序

本章的目的是编写一个完整的字符设备驱动程序scull，Simple Character Utility for Loading Localities，区域装载的简单字符工具。

### scull 的设计

编写驱动程序的第一步就是定义驱动程序为用户程序提供的能力（机制）。

scull实现了以下类型的设备：
* scull0 ~ scull3
* scullpipe0 ~ scullpipe3
* scullsingle
* scullpriv
* sculluid
* scullwuid

### 主设备号和次设备号

对字符设备的访问是通过文件系统内的设备名称进行的，简单来说是文件系统树的节点，通常位于/dev目录。可通过`ls -l`的第一个字符`c`，来识别字符设备。

每个设备都有一个主设备号和次设备号。主设备号标识设备对应的驱动程序，次设备号由内核使用，用于正确确定设备文件所指的设备。

#### 设备号的内部表达

在内核中，`dev_t`类型用来保存设备号，包括主设备号和次设备号。
```c
#include <linux/types.h>
MAJOR(dev_t dev);   // 获取主设备号
MINOR(dev_t dev);   // 获取次设备号
MKDEV(int major, int minor);  // 构造 dev_t
```

#### 分配和释放设备编号

```c
#include <linux/fs.h>
int register_chrdev_region(dev_t first, unsigned int count, char *name);
int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name);
void unregister_chrdev_region(dev_t first, unsigned int count);
```

推荐动态分配主设备号。Documentation/devices.txt列出了静态分配的设备号。如果需要静态分配设备号，应该要避免已经分配的设备号。

动态分配的缺点：无法预先创建设备节点。不过可以通过读取文件`/proc/devices`来获取设备号，然后再创建设备节点。

### 一些重要的数据结构

* 文件操作：`struct file_operations`
* file 结构：`struct file`
* inoe 结构：`struct inode`

### 字符设备的注册

```c
struct cdev * cdev_alloc(void);
void cdev_init(struct cdev *cdev, struct file_operations *fops);
int cdev_add(struct cdev *cdev, dev_t num, unsigned int count);
void cdev_del(struct cdev *cdev);
```

早期注册字符设备的方法：
```c
int register_chrdev(unsigned int major, const char *name, struct file_operations *fops);
int unregister_chrdev(unsigned int major, const char *name);
```

### open 和 release

### scull的内存使用

### read 和 write

## 第四章 调试技术

### 内核中的调试支持

一些用于调试的内核选项。

### 通过打印调试

#### printk

### 使用 proc 文件系统

### 通过监视调试

strace 指令。

### 调试系统故障

主要是oops消息的解读。

### 调试器和相关工具

* gdb
* kdb内核调试器
* 用户模式的Linux虚拟机
* Linux跟踪工具包LTT
* 动态探测 Dprobes

## 第五章 并发和竞态

对共享数据的并发访问会导致竞态。

### 并发及其管理

并发的来源：
* SMP，内核代码是可抢占的
* 设备中断
* workqueue
* tasklet
* timer

为避免驱动程序产生竞态，需要遵循以下规则：
* 尽量避免资源的共享。
* 必须显式的管理对共享资源的访问，必须确保一次只有一个执行线程可以操作共享资源。
* 共享资源被其他组件引用时，必须确保自己可用。

### 信号量和互斥体

信号量用于建立临界区。当代码进入临界区时，减少信号量。如果信号量为0，则进程休眠，直到其他进程释放信号量。当代码退出临界区时，增加信号量。

初始值为1的信号量，又被称为互斥体。在任意给定时刻，只能被单个线程拥有。

#### Linux信号量接口

头文件和数据类型：
```c
#include <asm/semaphore.h>
struct semaphore
```

声明和初始化：
```c
// 按指定值初始化信号量
void sema_init(struct semaphore *sem, int val);
// 声明和初始化互斥体
DECLARE_MUTEX(name);
DECLARE_MUTEX_LOCKED(name);
// 动态分配互斥体
void init_MUTEX(struct semaphore *sem);
void init_MUTEX_LOCKED(struct semaphore *sem);
```

减少信号量的值（获取信号量）：
```c
// 减少信号量，并在必要时一直等待，不可中断。用于建立不可杀进程。
void down(struct semaphore *sem);
// down()的中断版本。被中断时，返回非0值，并且没有获取到信号量。
int down_interruptible(struct semaphore *sem);
// 尝试减少信号量，如果不可获得，则立即返回非0值。
int down_trylock(struct semaphore *sed);
```

增加信号量的值（释放信号量）：
```c
void up(struct semaphore *sem);
```

特别注意：如果在拥有一个信号量时发生错误，必须在将错误状态返回给调用者之前释放信号量。

在等待信号量时被中断，先撤销用户可见的任何修改，然后返回`-ERESTARTSYS`。如果无法撤销，则返回`-EINTR`。

#### 读取者写入者信号量

对共享数据的访问分为只读和写入。我们可以接收并发读取。

头文件和数据类型：
```c
#include<linux/rwsem.h>
struct rw_semaphore
```

初始化接口：
```c
void init_rwsem(struct rw_semaphore *sem);
```

只读访问的接口：
```c
void down_read(struct rw_semaphore *sem);
// 获取信号量返回非0，其他情况返回0
int down_read_trylock(struct rw_semaphore *sem);
void up_read(struct rw_semaphore *sem);
```

写入者接口：
```c
void down_write(struct rw_semaphore *sem);
int down_write_trylock(struct rw_semaphore *sem);
void up_write(struct rw_semaphore *sem)l
// 将写入访问转换为只读访问
void downgrade_write(struct rw_semaphore *sem);
```

rwsem的特点：
* 允许一个写入者，或无数个读取者拥有
* 写入者具有更高的优先级
* 适用于很少需要写访问，且写入者只会短期拥有。

#### completion

驱动编程的一个常见模型：在当前线程之外初始化一个活动，然后等待该活动结束。信号量可以完成此项工作，但是不是最适合的。

completion是一种轻量级的进制，允许一个线程告诉另外一个线程某个工作已经完成。

头文件和数据类型：
```c
#include<linux/completion.h>
struct completion
```

初始化和声明接口：
```c
void init_completion(struct completion *c);
DECLARE_COMPLETION(my_completion);
```

等待completion：
```c
// 非中断等待，可能会照成不可杀的进程
void wait_for_completion(struct completion *c);
```

触发completion事件：
```c
void complete(struct completion *c);      // 唤醒一个等待线程
void complete_all(struct completion *c);  // 唤醒所有等待线程
```

completion通常是一个单次设备，使用一次后被丢弃。小心处理，也可以被重复使用。

问题：对一个completion连续两次调用complete()会发生什么？