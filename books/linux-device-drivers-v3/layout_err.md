# 《Linux设备驱动程序（第三版）》错误记录

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