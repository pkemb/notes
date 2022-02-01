# UNIX环境高级编程

记录阅读此书时发现的排版错误，或其他一些值得商榷的地方。



## 书籍版本

2014年6月第2版
2017年9月北京第15次印刷

## P75

在`struct stat`的介绍中，有三个成员拼写错误。


>```c
> struct timespec st_atime;
> struct timespec st_mtime;
> struct timespec st_ctime;
>```

应该修改为：
```c
 struct timespec st_atim;
 struct timespec st_mtim;
 struct timespec st_ctim;
```

在2008年以前的标准中，是如此定义的（注意类型）：
```c
time_t st_atime;
time_t st_mtime;
time_t st_ctime;
```

## P99

第4.19小节第二行，
> 每个文件`属性`所保存的实际精度依赖于文件系统的实现。

`属性`更换为`时间`可能更好一些。

TODO：待查阅英文原版。

## P177

此页中部有这样一句话：
> umask和chdir函数也必须是shell内置的。

这句话从语义上讲不通，umask()和chdir()是库函数，怎么可能是`shell内置的`。从上下文来看，这句话要表达的是：
```
umask和cd命令也必须是shell内置的。
```
因为`文件模式创建屏蔽字`和`当前工作目录`，可由子进程继承（参考P186）。

## P221

描述错误：
nice 取值范围应该是`-NZERO ~ (NZERO-1)`之间，而非`0~(2*NZERO)-1`之间。

错别字：
在调用nice函数之前需要清`楚`errno。应该是清`除`。

## P254

在介绍SIGQUIT时，应该是
> `终端`驱动程序产生此信号
而不是
> `中断`驱动程序产生此信号

## P280

应该是 `struct siginfo_t`，而不是`struct siginfo`，这是在介绍结构体的定义，不应该省略后面的`_t`。除此之外的多处`siginfo结构`的表述，个人认为是可以接受的。

## P318

图11-6中，是`pthread_cleanup_push`，而不是`pthread_cancel_push`。

## P343

函数`pthread_attr_setdetachstate()`的第二个形参有误，是`int detachstate`，而不是`int *detachstate`。