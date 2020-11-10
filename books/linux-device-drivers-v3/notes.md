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

### poll和select

### 异步通知

### 定位设备

### 设备文件的访问控制
