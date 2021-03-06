# Linux设备驱动程序（第三版）

相关资源：
* [ldd3英文版PDF](https://lwn.net/Kernel/LDD3/)

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

当向系统添加字符设备时，需要提供一个`struct file_opetations`实例，里面包含了操作字符设备的系统调用的实现。这些系统调用，一般有这两个参数：`struct file *filp`和`struct inode *inode`。

filp表示了一个打开的文件。filp的`private_data`可用来跨系统调用保存信息。

inode用来表示一个文件。与filp不同，多个打开的filp，可能对应同一个inode。inode包含了大量有关文件的信息。对于字符设备来说，以下两个字段非常有用：
```c
dev_t i_rdev;           // 对表示设备文件的inode结构，该字段包含了真正的设备编号
struct cdev *i_cdev;    // inode 指向一个字符设备文件时，包含了指向 struct cdev 结构的指针
```

注：对于i_rdev来说，由于类型有发生变化，不建议直接操作，建议使用以下两个宏：
```c
unsigned int imajor(struct inode *inode);
unsigned int imonor(struct inode *inode);
```

### 字符设备的注册

```c
// 动态分配 cdev 结构
struct cdev * cdev_alloc(void);
// 初始化已分配到的结构
void cdev_init(struct cdev *cdev, struct file_operations *fops);
// 将设备添加到系统。该调用返回之后，设备即可使用。
int cdev_add(struct cdev *cdev, dev_t num, unsigned int count);
// 移除一个字符设备
void cdev_del(struct cdev *cdev);
```

早期注册字符设备的方法：
```c
int register_chrdev(unsigned int major, const char *name, struct file_operations *fops);
int unregister_chrdev(unsigned int major, const char *name);
```

### open 和 release

open方法提供给驱动程序以初始化的能力，主要完成以下工作：
* 检查设备特定的错误
* 如果时首次打开，则对其初始化
* 如有必要，更新 f_op 指针
* 分配并填写 filp->private_data

open方法的原型：
```c
int (*open)(struct inode *inode, struct file *filp);
```

inode参数的i_cdev字段，包含了先前设置的cdev结构。

release方法与open方法正好相反。不是所有的close调用都会调用release方法，只有真正释放设备数据结构的close调用才会调用这个方法。主要完成以下工作：
* 释放由open分配的、保存在filp->private_data中的所有内容
* 在最后一次关闭操作时关闭设备

release方法的原型：
```c
int (*release)(struct inode *inode, struct file *filp);
```

### scull的内存使用

### read 和 write

read用于拷贝数据到用户空间，write用于从用户空间拷贝数据。函数原型如下：
```c
ssize_t read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
ssize_t write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
```

filp是文件指针，count是请求传输的数据长度，buff是指向用户空间的缓冲区，offp指明用户在文件中进行存取操作的位置。

buff是用户空间的指针，基于以下原因，不能直接操作：
* 在内核模式中，用户空间指针可能无效。
* 用户空间是分页的，buff指向的页可能不在内存中。直接引用可能导致页错误。
* 应用程序可能是个恶意程序，直接引用可能导致系统出现后门。

为了安全的访问用户空间，必须使用专用的函数：
```c
// 拷贝数据到用户空间
unsigned long copy_to_user(void __user *to, const void *from, unsigned long count);
// 从用户空间拷贝数据
unsigned long copy_from_user(void *to, const void __user *from, unsigned long count);
```

如果指针无效，会拒绝拷贝。返回值是还需要拷贝的内存数量。被寻址的内存空间可能不在内存中，会导致当前进程被转入睡眠状态。这就要求访问用户空间的任何函数都必须是可重入的，必须能和其他驱动程序函数并发执行。

无论传输了多少数据，都应该更新offp所表示的文件位置。

read方法和write方法的返回值：略。

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

### 自旋锁

自旋锁只有两个状态，锁定和解锁。如果锁定成功，则代码进入临界区。如果锁定失败，则代码不断的检测并尝试锁定，直到锁可用。测试并锁定的操作必须以原子方式完成。对于不同的处理器架构，自旋锁的实现是不同的。核心概念是一样的：自旋锁在等待时，处理器不能做任何有用的工作。

自旋锁通常用于不能休眠的代码。

#### 自旋锁API

```c
// 头文件
#include <linux/spinlock.h>
// 静态初始化
spinlock_t my_lock = SPIN_LOCK_UNLOCKED;
// 动态初始化
void spin_lock_init(spinklock_t *lock);
// 锁定。等待时，处理器无法做其他事情
void spin_lock(spinlock_t *lock);
// 解锁。
void spin_unlock(spinlock_t *lock);
```

#### 自旋和原子上下文

在使用自旋锁时，要注意以下规则，避免系统响应时间过长，或进入死锁。
1. 任何拥有自旋锁的代码都必须是原子的，不能休眠。
2. 在拥有自旋锁时需要禁止中断（仅本地CPU）
3. 自旋锁必须在可能的最短时间内拥有。

#### 自旋锁函数

```c
// 不关闭任何中断
void spin_lock(spinlock_t *lock);
// 禁止硬件中断和软件中断，中断状态保存在flags中
void spin_lock_irqsave(spinlock_t *lock, unsigned long flags);
// 禁止硬件中断和软件中断，不保存中断状态
void spin_lock_irq(spinlock_t *lock);
// 只禁止软件中断
void spin_lock_bh(spinlock_t *lock);
// 非阻塞获取，成功时返回非0
int spin_trylock(spinlock_t *lock);
int spin_trylock_bh(spinlock_t *lock);
```

当自旋锁能被运行在中断上下文的代码获得时，必须使用某个关闭中断的版本（在哪种中断上下文被获取，就要关闭哪种中断），避免系统死锁。如果只在软中断被获取，可以使用spin_lock_bh()，还能服务硬件中断。

释放自旋锁的函数严格对应于获取自旋锁的函数。

```c
void spin_unlock(spinlock_t *lock);
void spin_unlock_irqrestore(spinklock_t *lock, unsigned long flags);
void spin_unlock_irq(spinklock_t *lock);
void spin_unlock_bh(spinlock_t *lock);
```

#### 读取者/写入者自旋锁

类似于读取者写入者信号量。

相关API：略。

### 锁陷阱

关于使用锁的一些经验。

* 明确锁定模式，在`一开始`就要制定好清晰和明确的锁定规则。
* 明确锁定顺序，如果要同时获取多把锁，为避免发生死锁，需要按照固定的顺序获取锁。
  * 如果一个是局部锁，一个属于内核更中心位置的锁，则先获取局部锁。
  * 如果一个是信号量，一个是自旋锁，则先获取信号量。
  * 最好的方法是避免出现需要多个锁的情况。
* 综合权衡性能和复杂度。
  * 细粒度的锁有较高的性能，但代码复杂性较高，不利于维护。
  * 粗粒度的锁的性能较低，但是代码复杂性较低，便于维护。
  * 在初期应该使用粗粒度的锁，抑制自己过早考虑优化的欲望。
  * [lockmeter](http://oss.sgi.com/projects/lockmeter) 可度量内核花费在锁上的时间。

### 除了锁之外的办法

#### 免锁算法

重构算法，从根本上避免使用锁。

例如生产者-消费者问题，可以使用环形缓冲区。当只有一个生产者写入，只有一个消费者读取时，可以保证数据一致性。要小心处理缓冲区满和缓冲区空的情况。

Linux在2.6.10，有一个通用的缓冲区实现`linux/kfifo.h`。

#### 原子变量

当共享变量是一个简单的整数类型时，可以使用内核提供的`atomic_t`数据类型。特点如下：
* 保持的是一个int类型的值。
* 最大表示24位的整数
* 对该类型的操作，保证是原子的，而且速度很快。

原子变量API：
```c
void atomic_set(atomic_t *v, int i);
atomic_t v = ATOMIC_INIT(i);
int atomic_read(atomic_t *v);
void atomic_add(int i, atomic_t *v);
void atomic_sub(int i, atomic_t *v);
void atomic_inc(atomic_t *v);
void atomic_dec(atomic_t *v);
int atomic_inc_and_test(atomic_t *v);
int atomic_dec_and_test(atomic_t *v);
int atomic_sub_and_test(int i, atomic_t *v);
....
```

注意：需要多个atomic_t变量的操作，任然需要某种类型的锁。

#### 位操作

原子地修改和测试单个bit的函数。这些函数使用的数据类型依赖于具体架构。

```c
void set_bit(nr, void *addr); // 设置addr指向的数据项的第nr位为1
void clear_bit(nr, void *addr);
test_bit(nr, void *addr); // 返回第nr位，非原子方式实现

// 返回nr位之前的值，并设置相应的位
int test_and_set_bit(nr, void *addr);
int test_and_clear_bit(nr, void *addr);
int test_and_change_bit(nr, void *addr);
```

#### seqlock

用于保护很小、很简单、会频繁读取、写入很少且快速的资源。本质上，seqlock允许读者对资源的自由访问，但需要读者检查是否和写入者发生冲突。当冲突发生时，需要重试对资源的访问。

初始化代码：
```c
// 静态初始化
seqlock_t lock1 = SEQLOCK_UNLOCKED;
// 动态初始化
seqlock_t lock2;
seqlock_init(&lock);
```

读者在进入临界区之前，需要获取一个整数顺序值，在退出临界区时会和当前的顺序值比较。如果不相等，则必须重新读取。不能保护含有指针的数据结构。示例代码：
```c
#include <linux/seqlock.h>
unsigned int seq;
do {
    seq = read_seqbegin(&lock);
    // 完成读取工作
} while (read_seqretry(&lock, seq));
// 在中断上下文中，需要使用IRQ安全版本
unsigned int read_seqbegin_irqsave(seqlock_t *lock, unsigned long flags);
int read_seqretry_irqrestore(seqlock_t *lock, unsigned int seq, unsigned long flags);
```

写入者需要获取seqlock的自旋锁，所以自旋锁常见的限制也适用于seqlock。以下是写入者需要使用的API：
```c
// 获取
void write_seqlock(seqlock_t *lock);
void write_seqlock_irqsave(seqlock_t *lock, unsigned long flags);
void write_seqlock_irq(seqlock_t *lock);
void write_seqlock_bh(seqlock_t *lock);

// 释放
void write_sequnlock(seqlock_t *lock);
void write_sequnlock_irqrestore(seqlock_t *lock, unsigned long flags);
void write_sequnlock_irq(seqlock_t *lock);
void write_sequnlock_bh(seqlock_t *lock);
```

#### 读取-复制-更新

RCU, read-copy-update。RCU发明者的[白皮书](http://www.rdrop.com/users/paulmck/rclock/intro/rclock_intro.html)。

RCU保护包含以下限定的资源：
* 经常发生读取而很少写入
* 被保护的资源通过指针访问

RCU的原理：在需要修改数据时，写入线程首先复制，然后修改副本，然后用新版本替代相关指针。当内核确认老版本没有引用时，即可释放。

RCU相关API和使用示例：略。

## 第六章 高级字符驱动程序操作

本章主要讲述了编写全功能字符设备驱动程序的几个概念，均会通过对scull驱动程序的修改来说明。
* [ioctl系统调用，设备控制的公共接口](#ioctl)
* [阻塞型IO](#阻塞型io)
* [poll和select](#poll和select)
* [异步通知](#异步通知)
* [定位设备](#定位设备)

### ioctl

设备驱动程序通常会通过ioctl执行各种类型的硬件控制。

用户空间的ioctl系统调用具有如下原型。最后的三个点不代表可变数目的参数表，表示一个可选参数，通常定义为`char *argp`。使用指针可以向ioctl传递任意数据，也可以访问任意数据。也可以不使用第三个参数，取决于命令。
```c
int ioctl(int fd, unsigned long cmd, ...);
```

驱动程序的ioctl原型如下。inode和filp与open方法的参数一样。cmd由用户层不经修改的传给驱动程序。arg是可选参数，无论用户层是指针还是整型，这里都是long。如果用户层没有传递第三个参数，则arg处于未定义状态。
```c
int (*ioctl)(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
```

#### 选择ioctl命令

ioctl的命令分为4个字段，分别是：
| 字段 | 说明 | 长度 |
| - | - | - |
| type | 选择一个号码，并在整个驱动程序使用 | 8bit，_IOC_TYPEBITS |
| number | 序号，顺序编号 | 8bit，_IOC_NRBITS |
| direction | 从应用程序看，数据的传输方向。可能的取值有 _IOC_NONE、_IOC_READ、_IOC_WRITE |
| size | 所涉及的用户数据大小，但内核不强制使用此字段。 | 与体系结构有关，13bit或14bit，_IOC_SIZEBITS |

ioctl命令要求在系统范围内唯一，为了方便构造命令，内核提供了以下宏：
```c
// type 和 number 字段通过参数传入
// size 字段通过对 datatype 参数取sizeof获得
_IO(type, nr);                  // 构造无参数的命令编号
_IOR(type, nr, datatype);       // 从驱动程序读取数据
_IOW(type, nr, datatype);       // 写入数据到驱动程序
_IOWR(type, nr, datatype);      // 双向传输

// 解开字段的宏
_IOC_TYPE(cmd);     // type 字段
_IOC_NR(cmd);       // number 字段
_IOC_DIR(cmd);      // direction 字段
_IOC_SIZE(cmd);     // size 字段
```

除了少量预定义命令之后，内核并未使用ioctl的cmd参数的值。

#### ioctl返回值

当命令号不能匹配任何合法操作时，默认的返回值一般是`-ENVAL`（非法参数）。POSIX规定返回`-ENOTTY`，不合适的ioctl设备。返回`-ENVAL`是普遍做法。

#### 预定义命令

内核可以识别少量的预定义命令。当这些指令用于我们的设备时，它们会在我们自己的文件操作被调用之前被解码。如果自己的命令与预定义命令相同，应用程序的行为将无法预测。构建指令时要避开预定义指令。

预定义指令分为三组，设备驱动开发人员只对第一组感兴趣，他的幻数是'T'。
* 可用于任何文件（普通、设备、FIFO和socket）
* 只用于普通文件
* 特定于文件系统类型

下列ioctl命令对任何文件（包括设备特定文件）都是预定义的：
* FIOCLEX：设置执行时关闭标志
* FIONCLEX：清除执行时关闭标志
* FIOASYNC：设置或复位文件异步通知
* FIOQSIZE：返回文件或目录的大小。用于设备文件时，返回ENOTTY。
* FIONBIO：设置或清除O_NONBLOCK标志。

#### 使用ioctl参数

#### 权能与受限访问

#### ioctl命令的实现

#### 非ioctl的设备控制

### 阻塞型IO

当没有数据响应read方法，或没有空间响应write方法时，驱动程序应阻塞当前进程，将其置入休眠状态直到请求可继续。

#### 休眠的简单介绍

休眠是进程的一种状态。对进程来说，会从运行队列中移走，并被打上特殊标记。当标记被移除是，才会在任意CPU上调度。

进入休眠状态很容易，但要注意以下事项：
* 永远不要在原子上下文中进入休眠
  * 拥有自旋锁、seqlock、RCU时不能休眠
  * 禁止中断时不能休眠
  * 拥有信号量时可以休眠，但代码最好要非常短
* 唤醒时无法知道休眠了多长时间，休眠时都发生了什么。唤醒后，不能对状态做出任何假定。
* 除非知道其他地方会唤醒我们，否则不要进入休眠。

Linux通过等待队列来管理休眠的进程，等待队列是一个进程链表，包含了等待`某个特定事件`的所有进程。

等待队列头的数据类型是`wait_queue_head_t`。
```c
// 静态定义并初始化
DECLARE_WAIT_QUEUE_HEAD(name);
// 动态定义
wait_queue_head_t my_queue;
init_waitqueue_head(&my_queue);
```

#### 简单休眠

wait_event系列宏，可使进程进入休眠状态，直到condition变为真。在进入和退出休眠的时候，都会对condition求值（会被多次求值），所以表达式最好不要有副作用。
```c
// 非中断休眠，不会被信号打断
wait_event(queue, condition);
// 中断休眠
wait_event_interruptible(queue, condition);
wait_event_timeout(queue, condition, timeout);
wait_event_timeout_interruptible(queue, condition, timeout);
```

wake_up系列函数可用于唤醒指定queue上的所有进程。进程唤醒后会对condition再次求值，如果依旧为假，进程会再次休眠。
```c
void wake_up(wait_queue_head_t *queue);
void wake_up_interruptible(wait_queue_head_t *queue);
```

书中151页给出的示例程序存在竞态，应该如何解决？
1. 使用后文介绍的独占等待。

#### 阻塞和非阻塞型操作

* 阻塞操作：当操作不能继续下去时，让进程进入休眠状态，等待可以继续操作。
* 非阻塞操作：当操作不能继续下去时，直接返回错误。

如果应用程序指定了O_NONBLOCK 或 O_NDELAY 标志，read没有数据，或write没有空间时，应该立即返回-EAGAIN。
> 使用stdio处理非阻塞IO时，要时刻检查errno。否则会将错误返回当作EOF。

O_NONBLOCK标志也可用于open方法。如果打开设备需要很长的时间，可以考虑支持O_NONBLOCK标志。

*只有write read open受O_NONBLOCK标志影响。*

#### 一个阻塞IO示例

建议下载源码包，同时阅读scull_p_read()和scull_p_write()。

主要注意以下要点：
1. 进入休眠时，即调用wait_event_interruptible()宏时，不要处于原子上下文。
2. read()将进程放入inq队列，但是最后唤醒outq队列。

#### 高级休眠

在特殊的情况，可能需要使用底层的函数接口来实现休眠操作。

##### 进程如何休眠

进程进入休眠的步骤：
1. 分配并初始化一个`wait_queue_t`结构，并加入到对应的等待队列。
2. 设置进程的状态为`TASK_INTERRUPTIBLE`或`TASK_UNINTERRUPTIBLE`。这两种状态均可表示休眠。可用函数`set_current_state()`设置进程状态。
3. 调用`schedule()`放弃CPU。注意，在放弃CPU前，务必再次检查休眠条件。将进程放入等待队列之前，唤醒条件可能已经发生了。如果不检查，可能会丢失此唤醒条件。

几个需要注意的点：
1. `schedule()`调用并返回之后，进程无法得知过了多久，这段时间内发生了什么。
2. `schedule()`返回之后，需要手动的将进程从等待队列移除。
3. 如果跳过了对`schedule()`的调用，需要将进程状态设置为`TASK_RUNNING`，并从等待队列中移除。

##### 手工休眠

可以手工处理进程进入休眠的步骤，但是很容易出错。内核提供了一些函数，可以简化这些操作。

* 建立并初始化一个等待队列入口
```c
// 静态
DEFINE_WAIT(my_wait);
// 动态
wait_queue_t my_wait;
init_wait(&my_wait);
```
* 加入等待队列并设置进程的状态
```c
void prepare_to_wait(
    wait_queue_head_t *queue,  // 等待队列
    wait_queue_t       *wait,  // 等待队列入口
    int state)                 // 进程的新状态
                               // TASK_INTERRUPTIBLE   可中断休眠
                               // TASK_UNINTERRUPTIBLE 不可中断休眠
```
* 调用`schedule()`。调用之前，务必再次检查休眠条件。
* `schedule()`返回之后，开始清理。
```c
void finish_wait(
    wait_queue_head_t *queue,
    wait_queue_t      *wait)
)
```
* 再次测试休眠条件，判断是否需要再次进入休眠。

##### 独占等待

调用`wake_up()`时，会唤醒等待队列中的所有进程。如果资源只允许被一个进程获取，那么其余的进程会再次进入休眠，极大的浪费了系统的资源。独占等待可以解决此问题。

加入独占等待选项的休眠，与普通休眠，有以下不同：
1. 等待队列入口设置了WQ_FLAG_EXCLUSIVE标志时，会被加入到等待队列的尾部。
2. wake_up唤醒某个队列上的进程时，遇到了具有WQ_FLAG_EXCLUSIVE标志的进程后，停止唤醒其他进程。

注意：在遇到WQ_FLAG_EXCLUSIVE标志之前，依旧会唤醒所有的非独占进程。

设置独占等待标志的最简单方法是调用`prepare_to_wait_exclusive()`。wake_event()及其变种无法设置独占等待。
```c
void prepare_to_wait_exclusive(
    wait_queue_head_t *head,
    wait_queue_t *wait,
    int state);
```

##### 唤醒的相关细节

当一个进程被唤醒时，实际的结果由等待队列入口中的一个函数控制。默认的唤醒函数将进程设置为可运行状态。

wake_up的所有变种：略。

##### 旧的历史：sleep_on

`sleep_on()`没有提供对竞态的任何保护。调用sleep_on()和进程真正进入休眠之间，有一段窗口期。窗口期内的唤醒将会丢失。所以不建议使用，以后会删除这两个接口。

```c
void sleep_on(wait_queue_head_t *queue);  // 当前进程无条件的休眠在给定的队列上
void interruptible_sleep_on(wait_queue_head_t *queue);
```

### poll和select

select / poll / epoll 用于那些要使用多个输入或输出流而又不会阻塞其中任何一个流的应用程序中。这三个系统调用均需要设备驱动程序`poll()`方法的支持。

`poll()`方法的原型如下：
```c
unsigned int (*poll)(struct file *filp, poll_table *wait);
```

`poll()`方法的处理步骤：
1. 在一个或多个可指示poll状态变化的等待队列上调用 poll_wait。
    ```c
    // 向 poll_table 结构添加一个等待队列
    void poll_wait(struct file *filp, wait_queue_head_t *head, poll_table *wait);
    ```
2. 返回一个用来描述操作是否可立即无阻塞执行的位掩码。
    * POLLIN
    * POLLRDNORM
    * POLLRDBAND
    * POLLPRI
    * POLLHUP
    * POLLERR
    * POLLOUT
    * POLLWRNORM
    * POLLWRBAND

##### poll 方法在驱动程序的实现

##### 与read和write的交互

为了使应用程序正常工作，正确实现 read / write / poll 方法非常重要。

* read的语义：
* write的语义：
  * 永远不要让write调用在返回前等待数据的传输结束。

`fsync()`用于确保数据已经传送到设备上，此方法只有在输出缓冲区为空时才会返回。
```c
int (*fsync)(struct file *, struct dentry *, int datasync);
```

##### 底层的数据结构

当用户程序调用了poll/select/epoll函数时，内核会调用由该系统调用引用的全部文件的poll方法，并向它们传递同一个poll_table。

poll_table的结构：略。

### 异步通知

通过异步通知，应用程序可以在数据可用时收到SIGIO信号，而不需要不停地使用轮询来关注数据。示例代码如下：

```c
signal(SIGIO, &input_handler);  // 设置信号处理程序
fcntl(STDIN_FILENO, F_SETOWN, getpid());  // 设置属主进程，告诉内核通知那个进程
                                          // 属主存储在 filp->f_owner
oflags = fcntl(STDIN_FILENO, F_GETFL);
fcntl(STDIN_FILENO, F_SETFL, oflags | FASYNC);  // 设置FASYNC标志，开启异步通知
```

#### 从驱动程序的角度考虑

驱动程序实现异步通知的步骤：
1. F_SETOWN被调用时对 filp->f_owner赋值
2. F_SETFL设置FASYNC标志时，调用驱动程序的fasync方法。
3. 当数据到达时，给所有注册异步通知的进程发送SIGIO信号。

内核为第2、3步提供了一个通用的实现，包含一个数据结构，两个函数。
```c
struct fasync_struct;
int fasync_helper(int fd, struct file *filp, int mode, struct fasync_struct **fa);
void kill_fasync(struct fasync_struct **fa, int sig, int band);
```

`fasync()`方法中调用`fasync_helper()`方法，`write()`方法中调用`kill_fasync()`方法。具体实现参考书籍。

当文件关闭时，需要从活动的异步读取进程列表中删除该文件。没有设置FASYNC标识时也可以调用。
```c
/* 从异步通知列表中删除该 filp */
fasync(-1, filp, 0);
```

### 定位设备

`llseek()`方法实现了lseek和llseek系统调用。如果设备没有实现`llseek()`方法，内核默认通过修改filp->f_pos来实现定位。

`llseek()`原型如下，实现请参考书。
```c
loff_t (*llseek)(struct file *filp, loff_t off, int whence);
```

如果设备不支持定位（例如串口），可以在`open()`方法中调用`nonseekable_open()`，明确告诉内核不支持`llseek()`。同时将file_operations结构中的llseek设置为特殊的辅助函数no_llseek。

### 设备文件的访问控制

本小节主要介绍一些附加检查的技术。

#### 独享设备

一次只允许一个进程打开设备。最好不要这样做，因为限制了用户程序的灵活。用户可能会使用多个进程打开同一个文件。

实现方法：`open()`方法维护一个available原子变量，初始值为1，表示设备可用。当打开设备时，减少值并测试，如果等于0，打开成功。否则打开失败。

具体实现参考书。

#### 限制每次只由一个用户访问

单个用户可以多个进程打开文件，但是每次只允许一个用户打开设备。

实现方法：`open()`方法维护两个变量，一个打开计数，一个设备属主的UID。当打开计数为0时，直接打开设备，并记录UID。否则，对比当前进程的UID和之前记录的UID，如果不同，则拒绝打开。由于涉及到两个变量，用自旋锁保护对变量的访问。

#### 替代EBUSY的阻塞型open

当设备不可用时，一般是返回`-EBUSY`。也可以实现阻塞型open。
> 使用场景没有看懂。

实现方法：当设备不可用时，将进程放入到一个等待队列中。

阻塞型open()对交互式用户来说不是很友好。

如果需要对同一个设备以不同的、不兼容的策略访问，最好为每一种访问策略实现一个设备节点。

#### 在打开时复制设备

在进程打开设备时创建设备的不同私有副本。只有设备没有绑定到某个硬件对象时才能实现。如果复制的设备是由软件驱动程序创建的，称之为`虚拟设备`。

创建虚拟设备的关键是选择合适的key，不同的key，即代表不同的虚拟设备。key的来源不同，表示了不同的策略：
* 选择终端的次设备号：为每个终端复制不同的虚拟设备。
* 选择UID：为每个用户复制不同的虚拟设备。
* 选择PID：为每个访问该设备的进程复制一个虚拟设备。

代码请参考书籍。

## 第七章 时间、延迟及延缓操作

### 度量时间差

内核通过定时器中断来跟踪时间。其频率是常数`HZ`，默认值一般是50~1200。

定时器中断会增加时钟滴答数`jiffies_64`，通常会使用`jiffies`变量，要么和`jiffies_64`相同，要么是`jiffies_64`的低32位。不建议直接访问`jiffies_64`，因为不能保证在所有架构上都是原子的。

#### 使用jiffies计数器

`jiffies`和`jiffies_64`都应该看作只读变量。

计算未来时间戳：
```c
#include <linux/jiffies.h>
unsigned long j, stamp_1, stamp_half, stamp_n;
j       = jiffies;  // 读取当前值
stamp_1 = j + HZ;   // 未来的 1 秒
stamp_half = j + HZ/2;  // 半秒
stamp_n    = j + n * HZ / 1000;  // n 毫秒
```

比较jiffies时间戳：
```c
#include <linux/jiffies.h>
int time_after(unsigned long a, unsigned long b);   // a 代码的时间比 b 靠后，返回true
int time_before(unsigned long a, unsigned long b);  //
int time_after_eq(unsigned long a, unsigned long b);
int time_before_eq(unsigned long a, unsigned long b);
```

将jiffies转换为用户空间的时间表述方法：
```c
#include <linux/time.h>
unsigned long timespec_to_jiffies(struct timespec *value);
void jiffies_to_timespec(unsigned long jiffies, struct timespec *value);
unsigned long timeval_to_jiffies(struct timeval *value);
void jiffies_to_timeval(unsigned long jiffies, struct timeval *value);
```

读取64位计数器：
```c
u64 get_jiffies_64(void);
```

#### 处理器特定的寄存器

绝大多数现代处理器都包含一个随时钟周期不断递增的计数寄存器，通过此寄存器可以完成高分辨率计时。在x86的平台，是TSC寄存器（timestamp count，时间戳计数器）。

Linux提供了一个平台无关的函数，用于读取时钟周期计数寄存器。如果平台没有此寄存器，则一直返回0。
```c
#include <linux/timex.h>
cycles_t get_cycles(void);
```

### 获取当前时间

将墙上时间转换为jiffies值：
```c
#include <linux/time.h>
unsigned long mktime(
    unsigned int year,
    unsigned int mon,
    unsigned int day,
    unsigned int hour,
    unsigned int min,
    unsigned int sec);
```

获取墙上时间：
```c
#include <linux/time.h>
void do_gettimeofday(struct timeval *tv);
struct timespec current_kernel_time(void);
```

### 延迟执行

将特定代码延迟一段时间后执行。根据时间的长短，可以分为：
* [长延迟](#长延迟)：长于一个时钟滴答
* [短延迟](#短延迟)：一般少于一个时钟滴答

#### 长延迟

##### 忙等待

实现忙等待最简单的方法是监视jiffies计数器。例如以下代码：
```c
while (time_before(jiffies, j1))
    cpu_relax();
```

不建议使用这种方法，这会严重降低系统的性能。如果在进入循环之前关闭了中断，那么jiffies的值将永远不会得到更新。

##### 让出处理器

在不需要CPU时主动释放CPU：
```c
while (time_before(jiffies, t1))
    schedule();
```

让出CPU后，当前进程还在运行队列中。如果系统只有一个可运行的进程，那么此进程会不断的让出CPU-调度-让出CPU....。

同时不能确定下次调度此进程的时间，相比目标时间点，有可能已经过去很久了。

##### 超时

利用超时等待队列来延迟。进程会在指定的队列上休眠，超时到期时返回。时间使用jiffies表示，是相对时间而不是绝对时间。
```c
long wait_event_timeout(wait_queue_head_t q, condition, long timeout);
long wait_event_interruptible(wait_queue_head_t q, condition, long timeout);
```

利用内核提供的`schedule_timeout()`函数，可以避免使用和声明多余的等待队列头。
```c
#include <linux/sched.h>
signed long schedule_timeout(signed long timeout);
```

`schedule_timeout()`要求调用者在调用之前，设置进程的状态，典型代码如下：
```c
set_current_state(TASK_INTERRUPTIBLE);
schedule_timeout(delay);
```

#### 短延迟

显然，短延迟不能依赖于时钟滴答。使用以下内核函数，可以很好的实现短延迟。
```c
#include <linux/delay.h>
void ndelay(unsigned long nsecs); // 延迟纳秒
void udelay(unsigned long usecs); // 延迟微妙
void mdelay(unsigned long msecs); // 延迟毫秒
```

注意：
1. 参数不要传入太大的值，延迟时间与API要匹配。
2. 这三个函数均是忙等待函数。

实现毫秒级延迟还有其他的方法，同时不涉及忙等待：
```c
void msleep(unsigned int millisecs); // 不可中断的睡眠指定时间
unsigned long msleep_interruptible(unsigned int millisecs);
void ssleep(unsigned int seconds); // 秒级延迟
```

注意：所有的延迟方法，实际的延迟时间比指定的时间都要长。

### 内核定时器

内核定时器是一种数据结构，告诉内核在指定的时间，使用指定的参数，执行指定的函数。

定时器将会在中断上下文中运行，而不是注册定时器的进程上下文。在中断上下文中有以下限制：
* 不能访问用户空间。
* current指针没有意义。
* 不能执行休眠（wait_event）或调度（schedule）。

`in_interrupt()`在中断上下文中返回非零值，`in_atomic()`在调度不被允许的时候返回非零值。调度不被允许的情况包括硬件和软件中断上下文以及拥有自旋锁的任何时间点。

定时器的一些特点：
* 任务可以将自己注册以在稍后的时间重新运行。
* 在SMP系统中，定时器会在注册它的CPU上执行。
* 定时器是竞态的来源，即使是单处理器系统。

#### 定时器API

```c
#include <linux/timer.h>
struct timer_list {
    /* 外部不可访问的成员 */
    unsigned long expires;  // 期望定时器执行的 jiffies 值。
                            // 到达该值时，将执行function，并传递data作为参数
    void (*function)(unsigned long);
    unsigned long data;
};

void init_timer(struct timer_list *timer);  // 动态初始化 struct timer_list
struct timer_list TIMER_INITIALIZER(_function, _expires, _data);  // 静态初始化

void add_timer(struct timer_list *timer);
int  del_timer(struct timer_list *timer);

int  mode_timer(struct timer_lisr *timer, unsigned long expires); // 更新定时器的到期时间
int  del_timer_sync(struct timer_list *timer); // 确保返回时没有任何CPU在运行定时器函数，可在SMP系统上避免竞态。
int  timer_pending(const struct timer_list *timer); // 返回定时器是否在被调度运行。
```

#### 内核定时器的实现

根据到期时间的长短，将定时器散列到不同的链表。

### tasklet

tasklet与内核定时器类似，只有一点不同：不能要求tasklet在某个给定的时间执行。调度一个tasklet，表明希望内核选择某个其后的时间来执行给定的函数。

tasklet以数据结构的形式存在，使用前必须初始化。
```c
#include <linux/interrupt.h>
struct tasklet_struct {
    /* 外部不可访问的成员 */
    void (*func)(unsigned long);
    unsigned long data;
};

void tasklet_init(struct tasklet_struct *t, void (*func)(unsigned long), unsigned long data);
DECLARE_TASKLET(name, func, data);
DECLARE_TASKLET_DISABLE(name, func, data);
// 以下API的详细语义，可以看书P203
void tasklet_disable(struct tasklet_struct *t);  // 禁用tasklet，死等直到tasklet退出运行
void tasklet_disable_nosync(struct tasklet_struct *t); // 异步禁用tasklet，不会等待tasklet退出运行
void tasklet_enable(struct tasklet_struct *t);  // 启用tasklet
void tasklet_schedule(struct tasklet_struct *t); // 调度执行 tasklet
void tasklet_hi_schedule(struct tasklet_struct *t); // 高优先级调度执行 tasklet
void tasklet_kill(struct tasklet_struct *t); // 确保tasklet不会被再次调度执行
```

tasklet的特点：
* tasklet可以在稍后被禁止或重新启用。
* tasklet可以注册自己本身。
* tasklet可以在通常的优先级或高优先级执行。
* tasklet始终会在调度自己的CPU上运行。
* 如果系统负荷不重，tasklet会立即执行。最迟不晚于下一个时钟滴答。

### 工作队列

工作队列（workqueue）类似于tasklet，都允许内核代码请求某个函数在将来的时间别调用。主要区别如下：
* tasklet运行在软件中断上下文，workqueue运行在一个特殊的内核进程上下文，
  * tasklet以原子模式运行，workqueue不必原子化。
* 工作队列函数可以休眠
* tasklet始终运行在提交的同一CPU，而这是workqueue的默认方式。

创建工作队列：
```c
// 在每个处理器上为该工作队列创建专用的线程
struct workqueue_struct *create_workqueue(const char *name);
// 只在一个处理器上创建专用的线程
struct workqueue_struct *create_singlethread_workqueue(const char *name);
```

向工作队列提交一个任务，需要填充`work_struct`结构，可通过下列宏完成：
```c
// 编译时构造
DECLARE_WORK(name void (*function)(void *), void *data);
// 运行时构造
// 如果 work_struct 没有被提交到工作队列，使用 INIT_WORK
INIT_WORK(struct work_struct *work, void (*function)(void *), void *data);
// 如果 work_struct 已经提交到工作队列，使用 PREPARE_WORK
PREPARE_WORK(struct work_struct *work, void (*function)(void *), void *data);
```

将工作提交到工作队列：
```c
// 立即添加到工作队列
int queue_work(struct workqueue_struct *queue, struct work_struct *work);
// 至少经过指定的jiffies（由delay指定）之后才会被执行
int queue_delayed_work(struct workqueue_struct *queue,
                       struct work_struct *work,
                       unsigned long delay);
```

`data`将会作为参数传递给`function()`。如有必要，工作函数可以休眠，但要考虑会不会影响同一工作队列的其他工作。工作函数不能访问用户空间。

```c
// 取消某个挂起的工作队列入口项
// 返回非零表示在工作函数运行之前取消
// 返回零表示工作函数正在某个处理器上运行
int cancel_delayed_work(struct work_struct *work);
// 确保提交的工作函数不会在系统任何地方运行
void flush_workqueue(struct workqueue_struct *queue);

// 注意以上两个函数操作的对象是不同的

// 销毁工作队列
void destroy_workqueue(struct workqueue_struct *queue);
```

#### 共享队列

有时只需偶尔向队列提交工作，而创建工作队列会消耗很多资源，所以可以使用内核提供的共享的默认工作队列。

向共享队列提交工作：
```c
int schedule_work(struct work_struct *work);
// 延迟提交
int schedule_delayed_work(struct work_struct *work);
// 如果想取消提交到共享队列中的工作，可以用 cancel_delayed_work()。
```

```c
// 确保系统中任何地方都不会运行共享队列中的入口项
// 无法直到其他进程是否在使用共享队列，所以不知道此函数返回需要多长时间
void flush_shceduled_work(void);
```

## 第八章 分配内存

本章将会介绍设备驱动程序中使用内存的一些其他方法，还会介绍如何最好地利用系统内存资源。

### kmalloc函数的内幕

`kmalloc()`类似于`malloc()`函数，有如下特点：
* 可能被阻塞，否则运行很快。
* 不对获取的内存空间清零。
* 分配的区域在物理内存中也是连续的。

`kmalloc()`的原型如下：
```c
#include <linux/slab.h>
void *kmalloc(size_t size, int flags);
```

#### size参数

由于Linux的内存管理方法，`kmalloc()`只能分配一些预定义的、固定大小的字节数组。如果申请任意数量的内存，那么得到的很可能会多一些。

kmalloc()能够处理的最小内存块是32或64，取决于体系结构使用的页面大小。kmalloc()能够分配的最大内存块取决于体系结构和内核配置选项。

如果想代码具有可移植性，size最好不要超过128KB。

#### flags参数

`flags`参数用于控制`kmalloc()`的行为。

| flags | 说明 |
| -- | -- |
| GFP_KERNEL | 内存分配是代表运行在内核空间的进程执行的。可能会导致进程休眠，使用GFP_KERNEL 分配内存的函数必须是可重入的 |
| GFP_ATOMIC | 用于中断上下文或其他运行于进程上下文之外的代码中分配内存，不会休眠。 |
| GFP_USER | 为用户空间页分配内存，可能会休眠。 |
| GFP_HIGHUSER | 类似于 GFP_USER，如果有高端内存，则从高端内存分配。 |
| GFP_NOIO GFP_NOFS | 类似于GFP_KERNEL，不允许执行任何文件系统调用，或禁止任何IO的初始化。主要在文件系统和虚拟内存代码中使用。可能会休眠。 |
| __GFP_DMA | 请求分配发生在可进行DMA的内存区段中。 |
| __GFP_HIGHMEM | 要分配的内存可位于高端内存。 |
| __GFP_COLD | 请求尚未使用的冷页面。 |
| __GFP_NOWARN | 内存无法分配是不产生警告。 |
| __GFP_HIGH | 高优先级请求，允许为紧急情况而消耗由内核保留的最后一些页面。 |
| __GFP_REPEAT | 分配失败时再尝试一次，但仍有可能失败。 |
| __GFP_NOFAIL | 始终不返回失败，努力满足分配请求。 |
| __GFP_NORETRY | 如果所请求的内存不可获得，就立即返回。 |

注：双下划线开头的标志，可以“或”起来使用。

##### 内存区段

Linux把内核分为三个区段：可用于DMA的内存，常规内存，以及高端内存。

可用于DMA的内存指存在于特别地址范围内的内存，外设可以利用这些内存执行DMA访问。

高端内存时32位平台为了访问（相对）大量的内存而存在的一种进制。如果不首先完成一些特殊的映射，我们就无法从内核中直接访问这些内存。

如果指定了__GFP_DMA标志，则只有DMA区段会被搜索。如果不指定任何标志，则常规区段和DMA区段都会被搜索。如果指定了__GFP_HIGHMEM标志，三个区段都会被搜索。

### 后备高速缓存

后备高速缓存适用于相同大小，并且会反复分配的内存块。Linux的高速缓存有时又称为slab分配器，其类型是`kmem_cache_t`，通过下面的函数创建。
```c
kmem_cache_t *kmem_cache_create(
    const char *name,
    size_t size,
    size_t offset,
    unsigned long flags,
    void (*constructor)(void *, kmem_cache_t *, unsigned long flags),
    void (*destructor)(void *, kmem_cache_t *, unsigned long flags)
);
```
该函数创建一个高速缓存对象，其中可以容纳任意数目的内存区域，每个区域的大小都是`size`。`offect`参数是页面中第一个对象的偏移量，一般取零。

`flags`控制如何完成分配，是一个位掩码，可取的值如下：
| flags | 说明 |
| -- | -- |
| SLAB_NO_REAP | 保护高速缓存寻找内存的时候不会被减少。不建议使用。 |
| SLAB_HWCACHE_ALIGN | 要求所有数据对象跟高速缓存行对齐。对齐的填白可能浪费大量内存。 |
| SLAB_CACHE_DMA | 每个数据对象都从可用于DMA的内存区段获取。 |

`constructor()`用于初始化新分配的对象，`destructor()`用于内存空间释放给系统之前清除对象。这两个参数是可选的。在分配多个对象时，`constructor()`会被调用多次。但是`constructor()`和`destructor()`不是被立即调用，而是未来的某个时间点调用。

```c
// 从高速缓存分配内存对象，flags参数和传递给kmem_cache_create()的相同
void *kmem_cache_alloc(kmem_cache_t *cache, int flags);
// 释放一个内存对象
void kmem_cache_free(kmem_cache_t *cache, const void *obj);
// 释放高速缓存。如果失败，表明模板中发生了内存泄漏。
int kmem_cache_destroy(kmem_cache_t *cache);
```

可以从文件`/proc/slabinfo`查看高速缓存的使用情况。

#### 内存池

内存池就是某种形式的后备高速缓存，试图始终保持空间的内存，以便在紧急状态下使用，适用于不允许内存分配失败的情况。内存池会分配一些空闲且不会真正得到使用的内存块，所以内存池会浪费大量的内存。不推荐使用内存池。

内存池对象的类型是`mempool_t`，相关API如下：
```c
// 创建内存池对象
mempool_t *mempool_create(
    int min_nr,  // 始终保持已分配对象的最少数目
    mempool_alloc_t *alloc_fn,
    mempool_free_t  *free_fn,
    void *pool_data
);
// alloc_fn 的原型
typedef void *(mempool_alloc_t)(int gfp_mask, void *pool_data);
// free_fn 的原型
typedef void (mempool_free_t)(void *element, void *pool_data);
// 分配对象
void *mempool_alloc(mempool_t *pool, int gfp_mask);
// 释放对象
void mempool_free(void *element, mempool_t *pool);
// 调整 mempool的大小
int mempool_resize(mempool_t *pool, int new_min_nr, int gfp_mask);
// 销毁内存池
void mempool_destroy(mempool_t *pool);
```

### get_free_page和相关函数

如果模块需要分配大块的内存，使用面向页的分配技术会更好些。

分配函数：
```c
get_zeroed_page(unsigned int flags);  // 返回指向新页面的指针并将页面清零
__get_free_page(unsigned int flags);  // 不清零页面
// 分配若干物理连续的页面，返回指向该区域第一个字节的指针
__get_free_pages(unsigned int flags, unsigned int order);
// flags参数和kmalloc()函数一样
// order，阶数，实际分配的页面数是 2^order
// order 太大可能会导致失败。order最大值取10或11
// 查看 /proc/boddyinfo 可以得知系统每个内存区段上每个阶数下可获得的数据块数目。

// 释放页面
// 释放数目和分配数目务必要相等，否则内存映射关系会被破坏，系统会出错。
void free_page(unsigned long addr);
void free_pages(unsigned long addr, unsigned long order);
```

__get_free_page()函数的优点：
* 更有效地使用了内存。kmalloc因分配粒度的原因浪费一定数量的内存。
* 分配的页面完全属于自己。

#### alloc_pages接口

Linux页分配器。
```c
// nid是NUMA节点的ID号，flags是通常的GFP_标志，order是分配内存的大小
struct page *alloc_pages_node(int nid, unsigned int flags, unsigned int order);
// 在当前NUMA节点上分配内存
struct page *alloc_pages(unsigned int flags, unsigned int order);
struct page *alloc_page(unsigned int flags);

// 释放页面
void __free_page(struct page *page);
void __free_pages(struct page *page, unsigned int order);
// 帮助内存分配器优化内存的使用
void free_hot_page(struct page *page);
void free_cold_page(struct page *page);
```

### vmalloc及其辅助函数

`vmalloc()`用于分配连续的虚拟地址空间，物理地址空间上可能是不连续的。`vmalloc()`是Linux内存分配机制的基础。在大多数情况下不建议使用`vmalloc()`函数，因为获取的内存使用起来效率不高。

相关函数原型如下：
```c
#include <linux/vmalloc.h>
void *vmalloc(unsigned long size);
void vfree(void *addr);
void *ioremap(unsigned long phys_addr, unsigned long size);
void iounmap(void *addr);
```

需要注意的是，`kmalloc()`和`__get_free_pages()`返回的地址是部分虚拟，地址范围和物理地址是一一对应的，可能会有基于一个常量的偏移。而`vmalloc()`和`ioremap()`使用的地址范围是完全虚拟的，每次分配都要通过对页表的适当设置来建立（虚拟）内存区域。

`vmalloc()`分配的地址在VMALLOC_START和VMALLOC_END的范围。其地址不能再微处理器之外使用，只能在处理器的内存管理单元上才有意义。使用`vmalloc()`函数的正确使用场合是在分配一大块连续的、只在软件中存在的、用于缓冲的内存区域。`vmalloc()`不能在原子上下文中使用，内部调用了kmalloc(GFP_KERNEL)。

`ioremap()`也建立新的页表，但不实际分配内存。`ioremap()`的返回值是一个特殊的虚拟地址，可以用来访问指定的物理内存区域。为了保证可移植性，不应把返回的地址当作指向内存的指针而直接访问，应该使用`readb()`或其他IO函数。`ioremap()`更多用于映射（物理的）PCI缓冲区地址到（虚拟的）内核空间。

### per-CPU变量

当建立一个per-CPU变量时，系统中的每个处理器都会拥有该变量的特有副本。由于per-CPU变量可使用的地址空间是受限制的，所以应该保持per-CPU变量较小。优点：
* 对per-CPU变量的访问几乎不需要锁定，每个处理器在其自己的副本上工作。
* per-CPU变量可以保存在对应处理器的高速缓存中。频繁更新时有很好的性能。

编译时创建per-CPU变量及其相关的操作函数：
```c
#include <linux/percpu.h>
// 编译期间创建一个per-CPU变量。如果变量是一个数组，需要在type包含数组的维数。
DEFINE_PER_CPU(type, name);
```

当访问per-CPU变量的时候，应该避免进程被切换到另一个处理器上运行。所以应该显式地调用get_cpu_var宏访问某给定变量地当前处理器副本，结束后调用put_cpu_var。
```c
get_cpu_var(name)++; // 示例操作。get_cpu_var返回地是左值。
put_cpu_var(name);
```

如果要访问其他处理器的变量副本，可以使用`per_cpu()`宏。这时需要采用某种锁定机制来确保安全。
```c
per_cpu(variable, int cpu_id);
```

动态分配per-CPU变量：
```c
void *alloc_percpu(type);
void *__alloc_percpu(size_t size, size_t align); // 特定的对齐
void free_percpu(void *per_cpu); // 释放 per-CPU 变量
```

对动态分配的per-CPU变量通过per_cpu_ptr完成。
```c
// 返回给定cpu_id的per_cpu_var的指针
per_cpu_ptr(void *per_cpu_var, int cpu_id);
```

如果打算该变量的其他CPU版本，则可以引用该指针并进行相关操作。如果真正操作当前处理器版本，需要确保进程不会切换到其他进程运行。示例代码：
```c
int cpu = get_cpu();  // 获取对当前处理器的引用（阻塞抢占）并返回处理器ID
ptr = per_cpu_ptr(per_cpu_var, cpu);
// 使用 ptr
put_cpu();  // 返回对当前处理器的引用
```

per-CPU变量导出/导入：
```c
// 导出per-CPU变量给模块
EXPORT_PER_CPU_SYMBOL(per_cpu_var);
EXPORT_PER_CPU_SYMBOL_GPL(per_cpu_var);
// 模块导入per-CPU变量
DECLARE_PER_CPU(type, name);
```

### 获取大的缓冲区

## 第九章 与硬件通信

本章介绍驱动程序在Linux平台之上如何保持可移植性的前提下访问IO端口和IO内存。

### IO端口和IO内存

每种外设都通过读写寄存器进行控制，这些寄存器可以位于内存地址空间或IO地址空间。取决于处理器。

如果寄存器位于内存地址空间，则称为IO内存；如果位于IO地址空间，则称为IO端口。IO内存通常是首先的方案，因为不需要特殊的处理器指令。

#### IO寄存器和常规内存

IO寄存器和常规内存非常类似，但是IO操作有边际效应（side effect），而内存操作没有。所以为了提升内存的访问速度，可以用多种方法优化，例如高速缓存、重新排序读写指令。但对IO操作来说，这些优化可能照成致命的错误。
```
边际效应（side effect）：写入寄存器的值可能被外设修改；从寄存器读取的值不一定是最后一次写入的值。
```

驱动程序在读写IO寄存器的时候，必须确保不使用高速缓存，并且不发生读或写指令的重新排序。

不使用高速缓存：把底层硬件配置成在访问IO区域（不管是内存还是端口）时禁止硬件缓存。

指令重新排序：对必须以特定顺序执行的操作之间设置内存屏障。

编译器优化引起的指令重新排序：
```c
#include <linux/kernel.h>
// 避免在屏障前后的编译器优化，但硬件能完成自己的重新排序。
void barrier(void);
```

硬件引起的指令重新排序：
```c
#include <asm/system.h>
// 硬件内存屏障
void rmb(void); // 屏障之间的读操作一定会在后来的读操作之前完成。
void read_barrier_depends(void);
void wmb(void); // 写操作
void mb(void);  // rw
// SMP版本，在单处理器系统上，会被扩展为上面那些简单的屏障调用
void smp_rmb(void);
void smp_read_barrier_depends(void);
void smp_wmb(void);
void smp_mb(void);
```

注意事项：
* 内存屏障会影响系统性能，所以应该只用于真正需要的地方。
* 使用最符号需求的内存屏障。
* 大多数处理同步的内核原语（自旋锁或atomic_t），也能作为内存屏障使用。

### 使用IO端口

本节讲解了使用IO端口的不同函数。

#### IO端口分配

```c
#include <linux/ioport.h>
// 分配从first开始的n个端口，name是设备的名称
// 所有的端口分配可从 /proc/ioports 文件得到
struct resource *request_region(unsigned long first, unsigned long n, const char *name);
// 释放IO端口
void release_region(unsigned long start, unsigned long n);
// 检查给定端口集是否可用
// 不建议使用，因为检查和其后的分配并不是原子的操作
int check_region(unsigned long first, unsigned long n);
```

#### 操作IO端口

读取或写入这些端口。注意，大多数硬件都会把8位、16位和32位端口区分开来，因此C语言程序必须调用不同的函数来访问大小不同的端口。

```c
#include <asm/io.h>
unsigned inb(unsigned port); // 从端口读取一个字节
void outb(unsigned char byte, unsigned port); // 输出一个字节到端口

unsigned inw(unsigned port);
void outw(unsigned short word, unsigned port);

unsigned inl(unsigned port);
void outl(unsigned long word, unsigned port);
```

注意：没有定义64位的IO操作。即使在64位的体系架构上，端口地址空间也只使用最大32位的数据通路。

#### 在用户空间访问IO端口

GNU的C库`<sys/io.h>`中定义了这些函数，如果要使用，需要满足以下条件：
* 编译程序时必须带有 -O 选项来强制展开内联函数。
* 用ioperm或iopl系统调用来获取对端口进行IO操作的权限。
  * ioperm获取对单个端口的操作权限。
  * iopl获取对整个IO空间的操作权限。
* 必须以root身份运行该程序才能调用ioperm或iopl。或者进程的祖先已经获取对端口的访问。

如果平台没有ioperm和iopl系统调用，可以使用/dev/port设备文件访问端口。注意，该设备文件的含义与平台密切相关，并且除PC平台以外，几乎没有什么用处。

#### 串操作

有些处理器上实现了一次传输一个数据序列的特殊指令，序列中的数据单位可以是字节、字或双字。而且速度比一个C语言编写的循环语句快得多。

```c
// 从内存地址 addr 开始连续读/写count数目的字节，只对单一port操作
void insb(unsigned port, void *addr, unsigned long count);
void outsb(unsigned port, void *addr, unsigned long count);

void insw(unsigned port, void *addr, unsigned long count);
void outsw(unsigned port, void *addr, unsigned long count);

void insl(unsigned port, void *addr, unsigned long count);
void outsl(unsigned port, void *addr, unsigned long count);
```

串IO操作函数是直接将字节流从端口中读取或写入。所以，当端口与主机系统具有不同的字节序时，将导致不可预期的结果。需要在必要时交换字节。

#### 暂停式IO

#### 平台相关性

大部分与IO端口有关的源代码都是平台相关。平台相关性主要来自于：
* 数据类型
* 处理器基本结构上的差异。

具体差异细节：略。

### 使用IO内存

IO内存，映射到内存的寄存器或设备内存。如果使用IO内存来实现类似IO端口的设备寄存器，读写也有边际效应（side effect）。访问IO内存的方法和计算机体系结构、总线以及正在使用的设备有关。

根据计算机平台和总线的不同，访问IO内存：
* 通过页表：内核必须首先安排物理地址对设备驱动程序可见，即IO之前必须先调用ioremap()。
* 不通过页表：类似于IO端口，可以使用适当形式的函数读写。

强烈不建议直接使用指向IO内存的指针，而是使用包装函数访问IO内存。这些函数是安全的，而且经过优化。

#### IO内存分配和映射

在使用之前，必须首先分配IO内存区域。

```c
// 分配内存区域，所有IO内存分配情况可从/proc/iomem获得
// 从 start 开始分配 len 字节长的区域。
struct resource *request_mem_region(
       unsigned long start,
       unsigned long len,
       char *name);
// 释放IO内存区域
void release_mem_region(unsigned long start, unsigned long len);
// 检查给定的IO区域是否可用，不推荐使用。
int check_mem_region(unsigned long start, unsigned long len);
```

确保内核可以访问IO内存，即建立映射，为IO内存区域分配虚拟地址。

```c
#include <asm/io.h>
// 专门为IO内存区域分配虚拟地址
void *ioremap(unsigned long phys_addr, unsigned long size);
// 非缓存版本，绝大多数平台与ioremap()的实现一样。
void *ioremap_nocache(unsigned long phys_addr, unsigned long size);
void iounmap(void *addr);
```

#### 访问IO内存

```c
#include <asm/io.h>
// addr是从ioremap()获得的地址
// 从IO内存中读取
unsigned int ioread8(void *addr);
unsigned int ioread16(void *addr);
unsigned int ioread32(void *addr);

// 写入IO内存
void iowrite8(u8 value, void *addr);
void iowrite16(u16 value, void *addr);
void iowrite32(u32 value, void *addr);

// 在给定地址上读写一系列的值
void ioread8(void *addr, void *buff, unsigned long count);
void iowrite8(void *addr, void *buff, unsigned long count);
// 其余略

//在一块IO内存上执行操作
void memset_io(void *addr, u8 value, unsigned int count);
void memcpy_fromio(void *dest, void *source, unsigned int count);
void memcpy_toio(void *dest, void *source, unsigned int count);
```

老的IO内存函数，不执行类型检查，所以不推荐使用。
```c
unsigned readb(address);
unsigned readw(address);
unsigned readl(address);
void writeb(address);
void writew(address);
void writel(address);
```

#### 像IO内存一样使用端口

```c
// 重映射count个IO端口，使其看起来像IO内存
// 可在该函数的返回地址上使用ioread8及其同类函数
void *ioport_map(unsigned long port, unsigned int count);
// 取消重隐射
void ioport_unmap(void *addr);
```

## 第十章 中断处理

### 安装中断处理例程

### 实现中断处理例程

### 顶半部和低半部

### 中断共享

### 中断驱动的IO

## 第十一章 内核的数据类型

绝大多数的移植问题都和不正确的数据类型有关。坚持使用严格的数据类型，并且使用`-Wall -Wstrict-prototypes`选项编译可以防止大多数的代码缺陷。

内核使用的数据类型主要分为三类：
* C语言标志类型，例如int。
* 有确定大小的类型，例如u32。
* 用于特定内核对象的类型，例如pid_t。

### 使用标准C语言类型

### 为数据项分配确定的空间大小

### 接口特定的类型

### 其他有关移植性的问题

### 链表

## 第十二章 PCI驱动程序

Peripheral Component Interconnect，外围设别互联。

### PCI接口

PCI定义了计算机的各个不同的部分之间应该如何交互。本节主要介绍PCI驱动程序如何需要其硬件和获得对它的访问。

PCI架构被设计为ISA标准的替代品，主要有三个目标：
* 与外设更好的数据传输性能：使用比ISA更高的时钟频率。
* 平台无关：PCI广泛应用于IA-32、Alpha、PowerPC、SPARC64、IA-64等。
* 简化系统中增加和删除外设的工作：PCI设备可在引导阶段自动配置。

#### PCI寻址

* PCI域：16位，每个PCI域最多可以有256个PCI总线
* PCI总线：8位，每个PCI总线最多可以有32个设备
* PCI设备：5位，每个PCI设备可以有8种功能
* PCI功能编号：3位

PCI硬件地址（总线、设备、功能）存储在数据结构`struct pci_dev`中。硬件地址通常用如下的格式显示（十六进制）：
```
域:总线:设备.功能
0000:00:00.0
```

查看系统PCI设备的方法：
* lspci 指令，包含在pciutils包
* /proc/bus/pci目录：按照PCI总线划分子目录
* /sys/bus/pci/devices目录：每个设备是一个子目录，并按照`域:总线:设备.功能`命名
* /proc/bus/pci/devices 文件

每个PCI外设会响应三种外设地址空间：
* `IO端口`：由总线上的所有设备共享，通过`inb()`等函数访问，32位地址总线（4GB个端口）。
* `内存位置`：由总线上的所有设备共享，通过`readb()`等函数访问，32位或64位地址。
* `配置寄存器`：`配置事务`是地理寻址，每次只对一个槽寻址，通过调用特定的内核函数访问`配置寄存器`来执行`配置事务`。

固件在初始化引导系统时，会将不同的设备映射到不同的内存地址，映射的地址可以从配置空间中读取。映射的地址也可以通过配置事务来更改，不过可能会照成地址冲突，强烈不建议这样做。

PCI配置空间中每个PCI功能由256个字节组成，配置寄存器的布局是标准话的。从配置空间中可以读取到设备ID。更多信息请看[配置寄存器和初始化](#配置寄存器和初始化)。

#### 引导阶段

PCI设备上电时，处于未激活的状态。即改设备只会对配置事务做出响应，设备上不会有内存和IO端口映射到计算机的地址空间，其他设备功能也被禁止。

PCI主板上的固件，会对每个PCI设备执行`配置事务`，将设备提供的每个地址区域映射到处理器的地址空间。驱动程序可以修改这个默认配置，不过不建议。

PCI设备相关的文件：
* /proc/bus/pci/devices：包含有十六进制的设备信息的文本文件
* /proc/bus/pci/*/*：每个二进制文件对应一个设备，配置寄存器的快照。
* /sys/bus/pci/devices：PCI设备目录包，每个设备对应一个目录，包含许多不同的文件
  * config：配置寄存器的快照
  * vendor/device/subsystem_device/subsystem_vendor/class：该PCI设备的特定值
  * irq：分配给该PCI设备的当前IRQ
  * resource：该设备所分配的当前内存资源

#### 配置寄存器和初始化

每个PCI设备至少有256字节的配置寄存器，其中前64字节是标准的，其余是设备无关的。下图显示了设备无关的配置空间布局。配置寄存器分为必需和可选。必需的寄存器一定要包含有效值，可选的寄存器依赖于设备的实际功能。注意：PCI寄存器始终是小头的。

![PCI标准配置寄存器](image/PCI标准配置寄存器.png)

常用寄存器及其含义：
* vendor ID：16位，标识硬件制造商。制造商需要在PCI Special Interest Group申请。
* device ID：16位，由制造商选择，无需注册。vendorID和deviceID构成32位的设备签名。
* class：16位，每个外部设备属于某个类，高8位标识了基类。
* subsystem vendorID/subsystem deviceID：进一步标识设备。

PCI驱动程序用`struct pci_device_id`结构体定义该驱动程序支持的不同类型的PCI设备列表，包含以下字段：
* __u32 vendor
* __u32 device
  * PCI设备的厂商和设备ID。如果能处理任何厂商和设备，应该使用值PCI_ANY_ID。
* __u32 subvendor
* __u32 subdevice
  * 类似于 vendor / device
* __u32 class
* __u32 class_mask
  * 指定PCI设备支持一种PCI类（class）设备。如果驱动程序可以处理任何类型的子系统，这些字段应该使用值PCI_ANY_ID。
* kernel_ulong_t driver_data
  * 不是用来和设备匹配的，是用来保存PCI驱动程序用于区分不同设备的信息。

应该使用下列宏来初始化`struct pci_device_id`：
```c
// 仅和特定厂商及设备ID相匹配，subvendor和subdevice设置为PCI_ANY_ID。
PCI_DEVICE(vendor, device);
// 和特定PCI类相匹配
PCI_DEVICE_CLASS(class, class_mask);
```

使用宏定义驱动程序支持的设备类型的例子：略

#### MODULE_DEVICE_TABLE

```c
// 将pci_device_id结构体导出到用户空间
MODULE_DEVICE_TABLE(pci, pci_device_id_table);
```
该语句会创建名为`__mod_pci_device_table`的局部变量，指向数组`pci_device_id_table`。`depmod`程序会把所有模块中的`__mod_pci_device_table`添加到`/lib/modules/KENRNEL_VERSION/modules.pcimap`中。当PCI设备插入时，会在modules.pcimap文件中寻找恰当的驱动程序。

#### 注册PCI驱动程序

使用结构体`struct pci_driver`向PCI核心描述PCI驱动程序，主要字段如下：
* const char *name;
  * 驱动程序的名字，在系统中必须唯一。驱动程序运行后，会出现在/sys/bus/pci/drivers/下面。
* const struct pci_device_id *id_table;
  * 指向`struct pci_device_id`表的指针。
* int (*probe)(struct pci_dev *dev, const struct pci_device_id *id);
  * 指向探测函数的指针。当PCI核心有一个它认为驱动程序需要控制的`struct pci_dev`时，会调用此函数。如果PCI驱动程序确认传递给它的`struct pci_dev`，则应该恰达的初始化设备并返回0；否则返回一个负的错误值。
* void (*remove)(struct pci_dev *dev);
  * 当`struct pci_dev`被从系统中移除，或者PCI驱动程序真在从内核中卸载，PCI核心调用该函数。
* int (*suspend)(struct pci_dev *dev, u32 state);
  * 当`struct pci_dev`被挂起时PCI核心调用此函数，挂起状态通过state变量传递。该函数可选。
* int (*resume)(struct pci_dev *dev);
  * 当`struct pci_dev`被恢复时PCI核心调用此函数。该函数可选。

正确初始化一个PCI驱动程序，至少需要4个字段：
```c
static struct pci_driver pci_driver = {
    .name = "pci_skel",
    .id_table = ids,
    .probe = probe,
    .remove = remove,
}
```

```c
// 把 pci_driver 注册到pci核心，通常在模块初始化函数调用
// 注册成功返回0，否则返回负的错误编号。
int pci_register_driver(struct pci_driver *pci_driver);
// 从pci核心卸载pci_driver，通常在模块退出函数调用
int pci_unregister_driver(struct pci_driver *pci_driver);
```

#### 老式PCI探测

通过下列的函数，可以用来查找特定PCI设备。

```c
struct pci_dev *pci_get_device(
    unsigned int vendor,
    unsigned int device,
    struct pci_dev *from);
```
返回与输入参数匹配的PCI设备，并增加其引用计数。驱动程序必须调用`pci_dev_put()`函数减少引用计数。`from`参数用来得到具有同一签名的多个PCI设备，指向已经被找到的最近的一个设备。指定为NULL表示查找第一个。

```c
struct pci_dev *pci_get_subsys(
    unsigned int vendor,
    unsigned int device,
    unsigned int ss_vendor,
    unsigned int ss_device,
    struct pci_dev *from);
```
与`pci_get_device()`类似，允许在查找设备时指定子系统厂商和子设备ID。

```c
struct pci_dev *pci_get_slot(
    struct pci_bus *bus,
    unsigned int devfn);
```
在指定的PCI总线上根据设备的功能编号查找设备。

以上函数均不能在中断上下文中被调用。

#### 激活PCI设备

在PCI驱动程序的`probe()`函数中，访问任何PCI设备的资源（IO区域或中断）之前，必须调用下列函数激活设备：
```c
int pci_enable_device(struct pci_dev *dev);
```

#### 访问配置空间

处理器没有任何直接访问配置空间的途径，因此计算机厂商必须提供一个方法。由于各个厂商提供的方法不同，所以Linux提供了访问配置空间的标准接口。

```c
#include <linux/pci.h>
// 读取配置空间，
// where参数是从配置空间起始位置计算的字节偏移量，值通过val返回，
// 函数本身返回错误码。这三个函数返回的是处理器字节序。
int pci_read_config_byte(struct pci_dev *dev, int where, u8 *val);
int pci_read_config_word(struct pci_dev *dev, int where, u16 *val);
int pci_read_config_dword(struct pci_dev *dev, int where, u32 *val);

// 写配置空间。写入前，会将处理器字节序转换成小头。
int pci_write_config_byte(struct pci_dev *dev, int where, u8 val);
int pci_write_config_word(struct pci_dev *dev, int where, u16 val);
int pci_write_config_dword(struct pci_dev *dev, int where, u32 val);
```

上述inline函数实际上调用的是下列函数：
```c
int pci_bus_read_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 *val);
int pci_bus_read_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 *val);
int pci_bus_read_config_dword(struct pci_bus *bus, unsigned int devfn, int where, u32 *val);

int pci_bus_write_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 val);
int pci_bus_write_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 val);
int pci_bus_write_config_dword(struct pci_bus *bus, unsigned int devfn, int where, u32 val);
```

#### 访问IO和内存空间

每个PCI设备可以实现多达6个IO区域，每个区域可以是内存也可以是IO。大多数设备在内存区域实现IO寄存器。

设备通过配置寄存器报告每个区域的大小和当前位置，不推荐直接读取配置寄存器，而是使用以下函数接口：
```c
// 返回6个PCI IO区域之一的首地址（内存地址或IO端口编号），bar = 0, 1, 2, 3, 4, 5
unsigned long pci_resource_start(struct pci_dev *dev, int bar);
// 返回第 bar 个IO区域的尾地址，最后一个可用的地址。
unsigned long pci_resource_end(struct pci_dev *dev, int bar);
// 返回和该资源的标志
unsigned long pci_resource_flags(struct pci_dev *dev, int bar);
```

与IO区域相关的资源标志如下：
* IORESOURCE_IO
* IORESOURCE_MEM
  * 如果相关的IO区域存在，将设置这些标志之一
* IORESOURCE_PREFETCH
* IORESOURCE_READONLY
  * 表明内存区域是否可预取或是写保护的。

#### PCI中断

在计算机的引导阶段，固件就为PCI设备分配了中断号。借助以下两个寄存器，可以很容易的处理PCI中断。
* 60，PCI_INTERRUPT_LINE，中断号
* 61，PCI_INTERRUPT_PIN，中断引脚，0表示设备不支持中断。

书上还有一些关于PCI中断的扩展内容，略。

#### 硬件抽象

用于实现硬件抽象的机制，就是包含方法的普通结构（结构体就 + 函数指针）。

在PCI管理中，唯一依赖于硬件的操作是读取和写入配置寄存器，其他的工作都是通过直接读取和写入IO及内存地址来完成的，这些工作是由CPU直接控制的。所以，用于配置寄存器访问的相关结构仅包含两个字段：
```c
struct pci_ops {
    int (*read)(struct pci_bus *bus, unsigned int devfn, int where, int size);
    int (*write)(struct pci_bus *bus, unsigned int devfn, int where, int size);
};
```

系统中的各种PCI总线在系统引导阶段得到检测，即`struct pci_bus`项被创建并与其功能关联起来，其中包括ops字段。

### ISA回顾

### PC104和PC/104+

### 其他的PC总线

### SBus

### NuBus

### 外部总线

## 第十三章 USB驱动程序

USB，通用串行总线，是为了替代许多不同的低速总线。从拓扑结构看，USB是一颗由几个点对点的连接构成的树。主控制器询问设备是否有数据要传输，如果没有主控制器，设备无法传输数据。

USB协议定义了一套任何特定类型的设备都可以遵循的标准，这些不同类型称为类。如果符合此类，就不需要特殊的驱动程序。如果不符合，就需要编写针对于特定设备的特定厂商的设备驱动程序。

Linux内核支持两种USB驱动程序：
* 宿主（host）系统上的驱动程序：USB设备驱动程序
* 设备（device）上的驱动程序：USB器件驱动程序

宿主系统上的USB设备驱动程序控制插入其中的USB器件，USB器件驱动程序控制设备作为一个USB设备和主机通信。本书只介绍USB设备驱动程序如何编写，不介绍USB器件驱动程序。

USB设备驱动主要使用`USB核心`提供的接口来访问和控制USB设备，不必考虑不同USB主控制器的差异。

![USB设备驱动程序概观](image/USB设备驱动程序概观.png)

### USB设备基础

### USB和Sysfs

### USB urb

### 编写USB驱动程序

### 不使用urb的USB传输
