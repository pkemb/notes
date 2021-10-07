#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "pkchr_fifo.h"

static int pkchr_fifo_major = 0;
static int pkchr_fifo_minor = 0;

struct pkchr_fifo_dev *pkchr_fifo = NULL;

// inode 表示一个唯一的文件，主要关注成员 i_rdev 和 i_cdev
// filp 表示一个打开的文件，f_mode / f_pos / f_flags / f_op / private_data / f_dentry
int pkchr_fifo_open(struct inode *inode, struct file *filp)
{
    struct pkchr_fifo_dev *pkchr_fifo = NULL;
    // 从字符设备结构体，找到pkchr_dev结构体的地址
    pkchr_fifo = container_of(inode->i_cdev, struct pkchr_fifo_dev, cdev);
    filp->private_data = pkchr_fifo;
    return 0;
}

// 从用户空间拷贝数据到内核空间
ssize_t pkchr_fifo_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    long pos = *off;
    size_t count = size;
    struct pkchr_fifo_dev *pkchr_fifo = filp->private_data;

    printk(KERN_DEBUG"write, pos = %ld, count = %d\n", pos, count);

    // 写入的数据量大于最大值，返回没有空间
    if (count > MEM_SIZE)
        return -ENOSPC;

    // 写入位置超过范围，返回0
    if (pos >= MEM_SIZE)
        return 0;

    // 剩余空间不够，至写入部分值
    if (count > MEM_SIZE - pos)
        count = MEM_SIZE - pos;

    if (down_interruptible(&pkchr->sem))
        return -ERESTARTSYS;

    // 返回值大于0：剩余没有拷贝的数据
    // 返回值等于0：拷贝成功
    // 小于0：出错
    if (copy_from_user(pkchr->mem + pos, buff, count)) {
        return -EFAULT;
    }

    *off = pos + count;
    up(&pkchr->sem);
    return count;
}

// 从内核空间拷贝数据到用户空间
ssize_t pkchr_fifo_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    long pos = *off;
    size_t count = size;
    struct pkchr_fifo_dev *pkchr_fifo = filp->private_data;

    printk(KERN_DEBUG"read, pos = %ld, count = %d\n", pos, count);

    if (pos >= MEM_SIZE)
        return 0;

    if (count > MEM_SIZE - pos)
        count = MEM_SIZE - pos;

    if (down_interruptible(&pkchr->sem))
        return -ERESTARTSYS;

    if (copy_to_user(buff, pkchr->mem + pos, count)) {
        return -EFAULT;
    }

    *off = pos + count;
    up(&pkchr->sem);
    return count;
}

// 设置偏移量，file结构体的f_pos成员
// f_pos表示缓冲区的位置指针，取值范围是 0 ~ MEM_SIZE-1
// 如果设置的值超过范围，则返回错误EINVAL。
loff_t pkchr_fifo_llseek(struct file *filp, loff_t off, int whence)
{
    loff_t ret = 0;
    loff_t new_pos = 0;
    switch (whence)
    {
    case SEEK_SET:
        if (off >= MEM_SIZE || off < 0) {
            ret = -EINVAL;
        } else {
            filp->f_pos = off;
            ret = filp->f_pos;
        }
        break;

    case SEEK_CUR:
        new_pos = filp->f_pos + off;
        if (new_pos < 0 || new_pos >= MEM_SIZE) {
            ret = -EINVAL;
        } else {
            filp->f_pos = new_pos;
            ret = filp->f_pos;
        }
        break;

    case SEEK_END:
        filp->f_pos = MEM_SIZE - 1;
        ret = filp->f_pos;
        break;

    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

// file结构释放时，将调用此函数。close系统调用会执行release函数。
// 只有file结构引用计数为0的时候，close才会调用release。保证了一次open对应一次release
int pkchr_fifo_release(struct inode *inode, struct file *filp)
{
    // 释放open函数申请的资源
    return 0;
}

struct file_operations fifo_fops = {
    .owner   = THIS_MODULE,
    .open    = pkchr_fifo_open,
    .write   = pkchr_fifo_write,
    .read    = pkchr_fifo_read,
    .llseek  = pkchr_fifo_llseek,
    .release = pkchr_fifo_release,
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
    int i = 0;
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
