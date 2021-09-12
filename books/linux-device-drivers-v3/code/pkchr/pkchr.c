#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "pkchr.h"

static int pkchr_major = 0;
static int pkchr_minor = 0;
static int pkchr_dev_num = 4;

struct pkchr_dev *pkchr = NULL;

// inode 表示一个唯一的文件，主要关注成员 i_rdev 和 i_cdev
// filp 表示一个打开的文件，f_mode / f_pos / f_flags / f_op / private_data / f_dentry
int pkchr_open(struct inode *inode, struct file *filp)
{
    struct pkchr_dev *pkchr = NULL;
    // 从字符设备结构体，找到pkchr_dev结构体的地址
    pkchr = container_of(inode->i_cdev, struct pkchr_dev, cdev);
    filp->private_data = pkchr;
    return 0;
}

// 从用户空间拷贝数据到内核空间
ssize_t pkchr_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    long pos = *off;
    size_t count = size;
    struct pkchr_dev *pkchr = filp->private_data;

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

    // 返回值大于0：剩余没有拷贝的数据
    // 返回值等于0：拷贝成功
    // 小于0：出错
    if (copy_from_user(pkchr->mem + pos, buff, count)) {
        return -EFAULT;
    }

    *off = pos + count;
    return count;
}

// 从内核空间拷贝数据到用户空间
ssize_t pkchr_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    long pos = *off;
    size_t count = size;
    struct pkchr_dev *pkchr = filp->private_data;

    printk(KERN_DEBUG"read, pos = %ld, count = %d\n", pos, count);

    if (pos >= MEM_SIZE)
        return 0;

    if (count > MEM_SIZE - pos)
        count = MEM_SIZE - pos;

    if (copy_to_user(buff, pkchr->mem + pos, count)) {
        return -EFAULT;
    }

    *off = pos + count;
    return count;
}

// 设置偏移量，file结构体的f_pos成员
// f_pos表示缓冲区的位置指针，取值范围是 0 ~ MEM_SIZE-1
// 如果设置的值超过范围，则返回错误EINVAL。
loff_t pkchr_llseek(struct file *filp, loff_t off, int whence)
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

// inode和filp表示一个打开的文件，cmd表示不同的操作，arg是一个可选的参数
int pkchr_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct pkchr_dev *pkchr = filp->private_data;
    int ret = 0;

    switch (cmd)
    {
    case PKCHR_IOCLEAR:
        memset(pkchr->mem, 0, MEM_SIZE);
        ret = 0;
        break;

    default:
        ret = -EINVAL;
        break;
    }
    return ret;
}

// file结构释放时，将调用此函数。close系统调用会执行release函数。
// 只有file结构引用计数为0的时候，close才会调用release。保证了一次open对应一次release
int pkchr_release(struct inode *inode, struct file *filp)
{
    // 释放open函数申请的资源
    return 0;
}

struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = pkchr_open,
    .write   = pkchr_write,
    .read    = pkchr_read,
    .llseek  = pkchr_llseek,
    .ioctl   = pkchr_ioctl,
    .release = pkchr_release,
};

int pkchr_read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;
    // 增加模块引用计数
    if (!try_module_get(THIS_MODULE))
        return 0;

    len += sprintf(buf + len, "%d\n", MEM_SIZE);
    printk(KERN_INFO"count = %d, offset = %ld, buf = %p, len = %d\n", count, (long)offset, buf, len);
    module_put(THIS_MODULE);

    return len;
}

// 返回值是stop/show函数的void *v参数
static void *pkchr_seq_start(struct seq_file *s, loff_t *pos)
{
    if (*pos >= pkchr_dev_num) {
        return NULL;
    }
    return pkchr + *pos;
}

// 返回值是stop/show函数的void *v参数
static void *pkchr_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    (*pos)++;
    if (*pos >= pkchr_dev_num) {
        return NULL;
    }
    return pkchr + *pos;
}

static void pkchr_seq_stop(struct seq_file *s, void *v)
{
    return;
}

static int pkchr_seq_show(struct seq_file *s, void *v)
{
    struct pkchr_dev *dev = (struct pkchr_dev *)v;
    seq_printf(s, "dev address = 0x%p\n", dev);
    return 0;
}

static struct seq_operations pkchr_seq_ops = {
    .start = pkchr_seq_start,
    .stop  = pkchr_seq_stop,
    .next  = pkchr_seq_next,
    .show  = pkchr_seq_show,
};

static int pkchr_seq_proc_open(struct inode *inode, struct file *filp)
{
    return seq_open(filp, &pkchr_seq_ops);
}

static struct file_operations pkchr_seq_proc_ops = {
    .owner   = THIS_MODULE,
    .open    = pkchr_seq_proc_open,
    .read    = seq_read,    // sed_read 是kernel定义的函数
    .llseek  = seq_lseek,   // seq_lseek 是kernel定义的函数
    .release = seq_release, // seq_release 是kernel定义的函数
};

static void pkchr_setup_dev(struct pkchr_dev *pkchr, int index)
{
    dev_t devno = MKDEV(pkchr_major, pkchr_minor + index);
    int err = 0;

    printk(KERN_INFO"init pkchr%d\n", index);

    // 初始化 struct cdev, 需要 fops 结构体
    cdev_init(&pkchr->cdev, &fops);
    pkchr->cdev.owner = THIS_MODULE;

    // 注册cdev
    err = cdev_add(&pkchr->cdev, devno, 1);
    if (err < 0) {
        printk(KERN_INFO"add cdev %x fail, err = %d\n", devno, err);
    }
}

static int __init pkchr_init(void)
{
    dev_t devno = 0;
    int ret = 0;
    int i = 0;
    struct proc_dir_entry *proc_entry = NULL;
    struct proc_dir_entry *seq_proc_entry = NULL;
    printk(KERN_INFO"pk char device init\n");

    // 申请设备号，0 正确，小于0 错误
    ret = alloc_chrdev_region(
        &devno,         // 设备号
        0,              // 第一个子设备
        pkchr_dev_num,  // 设备总数
        DEVICE_NAME);   // 设备名
    if (ret < 0) {
        printk(KERN_ERR"dev number alloc fail, ret = %d\n", ret);
        goto alloc_devno_fail;
    }
    printk(KERN_INFO"devno = %d\n", devno);
    pkchr_major = MAJOR(devno);
    pkchr_minor = MINOR(devno);
    printk(KERN_INFO"major = %d\n", pkchr_major);
    printk(KERN_INFO"minor = %d\n", pkchr_minor);

    // 申请设备结构体
    pkchr = kmalloc(sizeof(*pkchr) * pkchr_dev_num, GFP_KERNEL);
    if (pkchr == NULL) {
        printk(KERN_ERR"kmalloc fail\n");
        goto kmalloc_fail;
    }
    memset(pkchr, 0, sizeof(*pkchr) * pkchr_dev_num);

    // 初始化 pkchr 变量
    for (i = 0; i < pkchr_dev_num; i++) {
        pkchr_setup_dev(pkchr + i, i);
    }

    // 在 /proc 根目录创建pkchr_length入口
    proc_entry = create_proc_read_entry("pkchr_length", 0, NULL, pkchr_read_proc, NULL);
    if (proc_entry == NULL) {
        printk(KERN_ERR"create_proc_read_entry fail\n");
        goto create_proc_entry_fail;
    }

    seq_proc_entry = create_proc_entry("pkchr_seq", 0, NULL);
    if (seq_proc_entry)
        seq_proc_entry->proc_fops = &pkchr_seq_proc_ops;

    return ret;

create_proc_entry_fail:
    kfree(pkchr);
kmalloc_fail:
    unregister_chrdev_region(devno, pkchr_dev_num);
alloc_devno_fail:
    return -1;
}
module_init(pkchr_init);

static void __exit pkchr_exit(void)
{
    dev_t dev = MKDEV(pkchr_major, pkchr_minor);
    int i = 0;
    printk(KERN_INFO"pk char device exit\n");
    // 删除字符设备
    if (pkchr) {
        for (i = 0; i < pkchr_dev_num; i++) {
            cdev_del(&pkchr[i].cdev);
        }
        kfree(pkchr);
    }
    // 删除设备号
    unregister_chrdev_region(dev, pkchr_dev_num);

    // 删除proc入口
    remove_proc_entry("pkchr_length", NULL);
    remove_proc_entry("pkchr_seq", NULL);
}
module_exit(pkchr_exit);

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPLv2");
