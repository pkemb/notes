# Linux设备驱动程序（第三版）

以下记录了此书的一些拼写、排版和逻辑错误。

## P159

倒数第三行：
> 休眠会将进程状态置为`TASK_RUNNING`，

根据函数`prepare_to_wait()`的语义，第三个参数是进程的新状态，所以这句话应该修改为：
> 休眠会将进程状态置为`TASK_INTERRUPTIBLE`，

## P160

第15行：
> 唤醒第一个具有`WQ_FLAG_EXCLUSIEVE`标志的进程

多了一个`E`，应该修改为：
> 唤醒第一个具有`WQ_FLAG_EXCLUSIVE`标志的进程

## P190

第2行：
> 64为jiffies计数器的高32位字的最低位被值一
应该修改为：
> 64为jiffies计数器的高32位字的最低位被置一

第3行：
> 这是由于`INTIAL_JIFFIES`的默认值所致

漏了一个`I`，应该为：
> 这是由于`INITIAL_JIFFIES`的默认值所致

# 嵌入式Linux设备驱动程序开发指南（原书第二版）

# Linux设备驱动开发

# Linux设备驱动开发详解 基于最新的Linux4.0内核