#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include "pkchr_fifo.h"

static int pkchr_fifo_major = 0;
static int pkchr_fifo_minor = 0;

struct pkchr_fifo_dev *pkchr_fifo = NULL;

/*
 * fifo 是一个循环缓冲区，写指针wp指向下一个可写的位置，读指针wp指向当前可读的位置
 * 也就是说，wp指向的位置可写，不能读
 *         rp指向的位置可读，除非 rp == wp
 * 下面对fifo的几个关键状态做出定义：
 * fifo空：wp == rp
 * fifo满：(wp + 1)%SIZE == rp
 *          当wp再往后移动一格时，如果等于rp，表示空间写满
 *          即最多写入(SIZE-1)的数据
 * fifo可读长度：
 *          wp >= rp: wp - rp
 *          wp <  rp: SIZE - (rp - wp)
 * fifo可写长度：
 *          wp >= rp: SIZE - (wp - rp) - 1   保留一个空间不写
 *          wp <  rp: rp - wp -1              保留一个空间不写
 *          或：
 *          wp == rp: SIZE -1
 *          wp != rp: (rp + SIZE - wp)%SIZE -1
 */

static size_t get_free_space(struct pkchr_fifo_dev *dev) {
    size_t rp = dev->read_point;
    size_t wp = dev->write_point;

    if (wp >= rp)
        return MEM_SIZE - (wp - rp) - 1; // 留一个空间不写
    else
        return rp - wp - 1;
}

// inode 表示一个唯一的文件，主要关注成员 i_rdev 和 i_cdev
// filp 表示一个打开的文件，f_mode / f_pos / f_flags / f_op / private_data / f_dentry
int pkchr_fifo_open(struct inode *inode, struct file *filp)
{
    struct pkchr_fifo_dev *pkchr_fifo = NULL;
    // 从字符设备结构体，找到pkchr_dev结构体的地址
    pkchr_fifo = container_of(inode->i_cdev, struct pkchr_fifo_dev, cdev);
    filp->private_data = pkchr_fifo;
    return nonseekable_open(inode, filp);
}

// 从用户空间拷贝数据到内核空间
// 如果没有空间可以写入，则在write_queue挂起读取进程，直到有数据可读
// read() 函数负责唤醒write_queue上的进程
ssize_t pkchr_fifo_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    long pos = *off;
    size_t count = size;
    struct pkchr_fifo_dev *dev = filp->private_data;
    DEFINE_WAIT(wait);

    printk(KERN_DEBUG"write, pos = %ld, count = %d\n", pos, count);

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    while (get_free_space(dev) == 0) {
        // 无空间写入数据，进程休眠
        up(&dev->sem);

        if (filp->f_flags & O_NONBLOCK) {
            return -ERESTARTSYS;
        }

        prepare_to_wait(&dev->write_queue, &wait, TASK_INTERRUPTIBLE);
        if (get_free_space(dev) == 0)
            schedule();
        // 进程唤醒
	    finish_wait(&dev->write_queue, &wait);
        // 再次获取锁
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    }

    // 有空间写入，表示 (wp + 1) % MEM_SZIE != rp
    count = min(count, get_free_space(dev));
    // 注意一定要有等号，不然 (wp == rp) != 0 时，缓冲区会溢出。
    if (dev->write_point >= dev->read_point) {
        // 最多只写入到缓冲区尾部。如果写入的长度不足，用户层会再次重新写入。
        // 如果rp=0，则 count < MEM_SIZE - wp
        count = min(count, MEM_SIZE - dev->write_point);
    }

    printk(KERN_DEBUG"write, wp = %d, rp = %d, count = %d\n",
                     dev->write_point, dev->read_point, count);

    // 返回值大于0：剩余没有拷贝的数据
    // 返回值等于0：拷贝成功
    // 小于0：出错
    if (copy_from_user(dev->mem + dev->write_point, buff, count)) {
        up(&dev->sem);
        return -EFAULT;
    }
    dev->write_point += count;
    // 回卷wp
    if (dev->write_point == MEM_SIZE)
        dev->write_point = 0;
    up(&dev->sem);

    // 唤醒读者
    wake_up_interruptible(&dev->read_queue);

    // 异步通知
    if (dev->async_queue)
        kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
    return count;
}

// 从内核空间拷贝数据到用户空间
// 如果没有数据可以读取，则在read_queue挂起读取进程，直到有数据可读
// write() 函数负责唤醒read_queue上的进程
ssize_t pkchr_fifo_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    long pos = *off;
    size_t count = size;
    struct pkchr_fifo_dev *dev = filp->private_data;

    printk(KERN_DEBUG"read, pos = %ld, count = %d\n", pos, count);

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    while (dev->read_point == dev->write_point) {
        // 无数据可读
        up(&dev->sem);
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        // 进入休眠状态
        if (wait_event_interruptible(dev->read_queue,
            (dev->read_point != dev->write_point))) {
            return -ERESTARTSYS;
        }
        // 进程唤醒，首先获取锁
        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    }

    // 有数据可以读，计算可写入的长度
    if (dev->read_point < dev->write_point) {
        count = min(count, dev->write_point - dev->read_point);
    } else {
        // 最多只读取到缓冲区尾部。如果读取的长度不足，用户层会再次重新读取。
        count = min(count, MEM_SIZE - dev->read_point);
    }

    printk(KERN_DEBUG"read, wp = %d, rp = %d, count = %d\n",
                     dev->write_point, dev->read_point, count);

    if (copy_to_user(buff, dev->mem + dev->read_point, count)) {
        up(&dev->sem);
        return -EFAULT;
    }

    dev->read_point += count;
    if (dev->read_point == MEM_SIZE) {
        // 读指针回卷
        dev->read_point = 0;
    }
    up(&dev->sem);

    // 唤醒write_queue上的所有进程
    wake_up_interruptible(&dev->write_queue);
    return count;
}

// file结构释放时，将调用此函数。close系统调用会执行release函数。
// 只有file结构引用计数为0的时候，close才会调用release。保证了一次open对应一次release
int pkchr_fifo_release(struct inode *inode, struct file *filp)
{
    // 释放open函数申请的资源
    return 0;
}

static unsigned int pkchr_fifo_poll(struct file *filp, poll_table *wait)
{
    struct pkchr_fifo_dev *dev = filp->private_data;
    unsigned int mask = 0;

    down(&dev->sem);
    // 添加等待队列到poll_table
    poll_wait(filp, &dev->read_queue, wait);
    poll_wait(filp, &dev->write_queue, wait);
    // 设置位掩码
    if (dev->read_point != dev->write_point)
        mask |= POLLIN | POLLRDNORM;
    if (get_free_space(dev))
        mask |= POLLOUT | POLLWRNORM;
    up(&dev->sem);
    return mask;
}

static int pkchr_fifo_fasync(int fd, struct file *filp, int mode)
{
    struct pkchr_fifo_dev *dev = filp->private_data;
    return fasync_helper(fd, filp, mode, &dev->async_queue);
}

struct file_operations fifo_fops = {
    .owner   = THIS_MODULE,
    .open    = pkchr_fifo_open,
    .write   = pkchr_fifo_write,
    .read    = pkchr_fifo_read,
    .llseek  = no_llseek,
    .release = pkchr_fifo_release,
    .poll    = pkchr_fifo_poll,
    .fasync  = pkchr_fifo_fasync,
};

int pkchr_fifo_read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;
    // 增加模块引用计数
    if (!try_module_get(THIS_MODULE))
        return 0;

    printk(KERN_INFO"count = %d, offset = %ld, buf = %p, len = %d\n", count, (long)offset, buf, len);

    len += sprintf(buf + len, "major = %d\n", pkchr_fifo_major);
    len += sprintf(buf + len, "buff len = %d\n", MEM_SIZE);

    module_put(THIS_MODULE);

    return len;
}

static int __init pkchr_fifo_init(void)
{
    dev_t devno = 0;
    int ret = 0;
    struct proc_dir_entry *proc_entry = NULL;
    printk(KERN_INFO"%s init\n", DEVICE_NAME);

    // 申请设备号，0 正确，小于0 错误
    ret = alloc_chrdev_region(
        &devno,         // 设备号
        0,              // 第一个子设备
        1,              // 设备总数
        DEVICE_NAME);   // 设备名
    if (ret < 0) {
        printk(KERN_ERR"dev number alloc fail, ret = %d\n", ret);
        goto alloc_devno_fail;
    }
    pkchr_fifo_major = MAJOR(devno);
    pkchr_fifo_minor = MINOR(devno);

    printk(KERN_INFO"devno = %d\n", devno);
    printk(KERN_INFO"major = %d\n", pkchr_fifo_major);
    printk(KERN_INFO"minor = %d\n", pkchr_fifo_minor);

    // 申请设备结构体
    pkchr_fifo = kmalloc(sizeof(*pkchr_fifo), GFP_KERNEL);
    if (pkchr_fifo == NULL) {
        printk(KERN_ERR"kmalloc fail\n");
        goto kmalloc_fail;
    }
    memset(pkchr_fifo, 0, sizeof(*pkchr_fifo));
    pkchr_fifo->read_point = pkchr_fifo->write_point = 0;
    init_waitqueue_head(&pkchr_fifo->read_queue);
    init_waitqueue_head(&pkchr_fifo->write_queue);

    // 初始化 struct cdev, 需要 fops 结构体
    cdev_init(&pkchr_fifo->cdev, &fifo_fops);
    pkchr_fifo->cdev.owner = THIS_MODULE;
    init_MUTEX(&pkchr_fifo->sem);

    // 注册cdev
    ret = cdev_add(&pkchr_fifo->cdev, devno, 1);
    if (ret < 0) {
        printk(KERN_INFO"add cdev %x fail, ret = %d\n", devno, ret);
        goto cdev_add_fail;
    }

    // 在 /proc 根目录创建pkchr_length入口
    proc_entry = create_proc_read_entry(PROC_NAME, 0, NULL, pkchr_fifo_read_proc, NULL);
    if (proc_entry == NULL) {
        printk(KERN_ERR"create_proc_read_entry fail\n");
        goto create_proc_entry_fail;
    }

    return ret;

create_proc_entry_fail:
    cdev_del(&pkchr_fifo->cdev);
cdev_add_fail:
    kfree(pkchr_fifo);
kmalloc_fail:
    unregister_chrdev_region(devno, 1);
alloc_devno_fail:
    return -1;
}
module_init(pkchr_fifo_init);

static void __exit pkchr_fifo_exit(void)
{
    dev_t dev = MKDEV(pkchr_fifo_major, pkchr_fifo_minor);
    printk(KERN_INFO"%s exit\n", DEVICE_NAME);
    // 删除字符设备
    if (pkchr_fifo) {
        cdev_del(&pkchr_fifo->cdev);
        kfree(pkchr_fifo);
    }
    // 删除设备号
    unregister_chrdev_region(dev, 1);

    // 删除proc入口
    remove_proc_entry(PROC_NAME, NULL);
}
module_exit(pkchr_fifo_exit);

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPLv2");
