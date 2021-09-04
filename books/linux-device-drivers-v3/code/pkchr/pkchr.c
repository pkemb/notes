#include <linux/module.h>
#include <linux/init.h>
#include "pkchr.h"

static int pkchr_major = 0;
static int pkchr_minor = 0;
static int pkchr_dev_num = 1;

struct pkchr_dev pkchr = {0};

// inode 表示一个唯一的文件，主要关注成员 i_rdev 和 i_cdev
// filp 表示一个打开的文件，f_mode / f_ops / f_flags / f_op / private_data / f_dentry
int pkchr_open(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t pkchr_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    return 0;
}

ssize_t pkchr_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    return 0;
}

loff_t pkchr_llseek(struct file *filp, loff_t off, int whence)
{
    return 0;
}

int pkchr_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}

// file结构释放时，将调用此函数
int pkchr_release(struct inode *inode, struct file *filp)
{
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

static int __init pkchr_init(void)
{
    dev_t devno = 0;
    int ret = 0;
    printk(KERN_INFO"pk char device init\n");

    // 申请设备号，0 正确，小于0 错误
    ret = alloc_chrdev_region(
        &devno,         // 设备号
        0,              // 第一个子设备
        pkchr_dev_num,  // 设备总数
        DEVICE_NAME);   // 设备名
    if (ret < 0) {
        printk(KERN_ERR"dev number alloc fail, ret = %d\n", ret);
        return -1;
    }
    printk(KERN_INFO"devno = %d\n", devno);
    pkchr_major = MAJOR(devno);
    pkchr_minor = MINOR(devno);
    printk(KERN_INFO"major = %d\n", pkchr_major);
    printk(KERN_INFO"minor = %d\n", pkchr_minor);

    // 初始化 pkchr 变量
    // 初始化 struct cdev, 需要 fops 结构体
    cdev_init(&pkchr.cdev, &fops);
    pkchr.cdev.owner = THIS_MODULE;
    pkchr.cdev.ops   = &fops;

    // 注册cdev
    ret = cdev_add(&pkchr.cdev, devno, 1);
    if (ret < 0) {
        printk(KERN_INFO"add cdev %x fail, err = %d\n", devno, ret);
        return -1;
    }

    return 0;
}
module_init(pkchr_init);

static void __exit pkchr_exit(void)
{
    dev_t dev = MKDEV(pkchr_major, pkchr_minor);
    printk(KERN_INFO"pk char device exit\n");
    // 删除字符设备
    cdev_del(&pkchr.cdev);
    // 删除设备号
    unregister_chrdev_region(dev, pkchr_dev_num);
}
module_exit(pkchr_exit);

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPLv2");
