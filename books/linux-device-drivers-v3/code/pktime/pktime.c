#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>

#include "pktime.h"

static int pktime_major = 0;
static int pktime_minor = 0;
static int pktime_dev_num = 1;

struct pktime_dev *pktime = NULL;

// inode 表示一个唯一的文件，主要关注成员 i_rdev 和 i_cdev
// filp 表示一个打开的文件，f_mode / f_pos / f_flags / f_op / private_data / f_dentry
int pktime_open(struct inode *inode, struct file *filp)
{
    struct pktime_dev *pktime = NULL;
    // 从字符设备结构体，找到pktime_dev结构体的地址
    pktime = container_of(inode->i_cdev, struct pktime_dev, cdev);
    filp->private_data = pktime;
    return nonseekable_open(inode, filp);;
}

// 从用户空间拷贝数据到内核空间
ssize_t pktime_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    return size;
}

// 从内核空间拷贝数据到用户空间
ssize_t pktime_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    return 0;
}

// file结构释放时，将调用此函数。close系统调用会执行release函数。
// 只有file结构引用计数为0的时候，close才会调用release。保证了一次open对应一次release
int pktime_release(struct inode *inode, struct file *filp)
{
    // 释放open函数申请的资源
    return 0;
}

struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = pktime_open,
    .write   = pktime_write,
    .read    = pktime_read,
    .llseek  = no_llseek,
    .release = pktime_release,
};

int pktime_jiffies(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;
    long j = 0;
    u64 j64 = 0;
    struct timeval timeval = {0};
    struct timespec timespec = {0};
    // 增加模块引用计数
    if (!try_module_get(THIS_MODULE))
        return 0;

    j = jiffies;
    j64 = get_jiffies_64();

    jiffies_to_timespec(j, &timespec);
    jiffies_to_timeval(j, &timeval);

    len += sprintf(buf + len, "HZ = %d\n", HZ);
    len += sprintf(buf + len, "jiffies = %ld\n", j);
    len += sprintf(buf + len, "jiffies_64 = %lld\n", j64);
    len += sprintf(buf + len, "timeval: tv_sec = %ld, tv_usec = %ld\n",
                    timeval.tv_sec, timeval.tv_usec);
    len += sprintf(buf + len, "timespec: tv_sec = %ld, tv_nsec = %ld\n",
                    timespec.tv_sec, timespec.tv_nsec);

    module_put(THIS_MODULE);

    return len;
}

static void pktime_setup_dev(struct pktime_dev *pktime, int index)
{
    dev_t devno = MKDEV(pktime_major, pktime_minor + index);
    int err = 0;

    PDEBUG("init pktime%d\n", index);

    // 初始化 struct cdev, 需要 fops 结构体
    cdev_init(&pktime->cdev, &fops);
    pktime->cdev.owner = THIS_MODULE;
    init_MUTEX(&pktime->sem);

    // 注册cdev
    err = cdev_add(&pktime->cdev, devno, 1);
    if (err < 0) {
        PDEBUG("add cdev %x fail, err = %d\n", devno, err);
    }
}

static int __init pktime_init(void)
{
    dev_t devno = 0;
    int ret = 0;
    int i = 0;
    struct proc_dir_entry *proc_entry = NULL;
    PDEBUG("%s init\n", DEVICE_NAME);

    // 申请设备号，0 正确，小于0 错误
    ret = alloc_chrdev_region(
        &devno,         // 设备号
        0,              // 第一个子设备
        pktime_dev_num,  // 设备总数
        DEVICE_NAME);   // 设备名
    if (ret < 0) {
        PDEBUG("dev number alloc fail, ret = %d\n", ret);
        goto alloc_devno_fail;
    }

    pktime_major = MAJOR(devno);
    pktime_minor = MINOR(devno);
    PDEBUG("devno = %x, major = %x, minor = %x\n", devno, pktime_major, pktime_minor);

    // 申请设备结构体
    pktime = kmalloc(sizeof(*pktime) * pktime_dev_num, GFP_KERNEL);
    if (pktime == NULL) {
        PDEBUG("kmalloc fail\n");
        goto kmalloc_fail;
    }
    memset(pktime, 0, sizeof(*pktime) * pktime_dev_num);

    // 初始化 pktime 变量
    for (i = 0; i < pktime_dev_num; i++) {
        pktime_setup_dev(pktime + i, i);
    }

    // 在 /proc 根目录创建pktime_length入口
    proc_entry = create_proc_read_entry(PROC_JIFFIES, 0, NULL, pktime_jiffies, NULL);
    if (proc_entry == NULL) {
        PDEBUG("create_proc_read_entry fail\n");
        goto create_proc_entry_fail;
    }

    return ret;

create_proc_entry_fail:
    kfree(pktime);
kmalloc_fail:
    unregister_chrdev_region(devno, pktime_dev_num);
alloc_devno_fail:
    return -1;
}
module_init(pktime_init);

static void __exit pktime_exit(void)
{
    dev_t dev = MKDEV(pktime_major, pktime_minor);
    int i = 0;
    PDEBUG("pk char device exit\n");
    // 删除字符设备
    if (pktime) {
        for (i = 0; i < pktime_dev_num; i++) {
            cdev_del(&pktime[i].cdev);
        }
        kfree(pktime);
    }
    // 删除设备号
    unregister_chrdev_region(dev, pktime_dev_num);

    // 删除proc入口
    remove_proc_entry(PROC_JIFFIES, NULL);
}
module_exit(pktime_exit);

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPLv2");
