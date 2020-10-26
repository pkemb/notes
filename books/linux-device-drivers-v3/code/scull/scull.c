#include <linux/module.h>
#include <linux/init.h>
#include "scull.h"

static int scull_major = SCULL_MAJOR;
static int scull_minor = SCULL_MINOR;

struct file_opetations scull_fops =
{
	.owner   = THIS_MODULE;
	//.llseek  = scull_llseek;
	.read    = scull_read;
	.write   = scull_write;
	//.ioctl   = scull_ioctl;
	.open    = scull_open;
	.release = scull_release;
};

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err;
	int devno = MKDEV(scull_major, scull_minor + index);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops   = &scull_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

static int __init scull_init(void)
{
	dev_t dev;
	int result;

	if (scull_major)
	{
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	}
	printk("scull init\n");
	return 0;
}
module_init(scull_init);

static void __exit scull_exit(void)
{
	printk("scull exit\n");
}
module_exit(scull_exit);

/*
 * 释放整个数据区
 * 在文件以写入方式打开时由 scull_open 调用
 */
int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;
	int i;

	for (dptr = dev->data; dptr; dptr = next)
	{
		if (dptr->data)
		{
			for (i=0; i < qset; i++)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}

	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset    = scull_qset;
	dev->data    = NULL;
	return 0;
}

/*
 * open() 函数通常完成以下工作：
 * 1. 检查设备特定的错误
 * 2. 如果设备是首次打开，则对其初始化
 * 3. 如有必要，更新 f_op 指针
 * 4. 分配并填写置于 filp->private_data 里的数据
 */
int scull_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev;

	dev = container_of(inode->i_cdev, struct scull_dev, cdev); // struct scull_dev 有一个成员是 cdev，并且其地址是 inode->i_cdev
	filp->private_data = dev;

	// 当设备以写方式打开时，长度被截为 0
	if ((filp->f_flags & O_ACCMODE) == O_WRONLY)
	{
		scull_trim(dev);
	}
	return 0;
}

/*
 * release() 与 open() 函数正好相反，通常完成以下工作：
 * 1. 释放由 open() 分配的、保存在 filp->private_data 中的内容
 * 2. 在最后一次关闭操作时关闭设备
 */
int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}

/*
 * 从设备拷贝数据到用户空间
 */
ssize_t scull_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;					// 第一个链表项
	int    quantum  = dev->quantum;
	int    qset     = dev->qset;
	int    itemsize = quantum * qset;
	int    item;
	int    s_pos;
	int    q_pos;
	int    rest;
	ssize_t retval = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if (*f_pos >= dev->size)
		goto out;
	if (*f_pos + count > dev->size)
		count = dev->size - *f_pos;

	// 在量子集中寻找链表项、qset索引以及偏移量
	item  = (long)*f_pos / itemsize;
	rest  = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	// 沿链表前行，直到正确的位置
	dptr = scull_follow(dev, item);

	if (dptr == NULL || !dptr->data || !dptr->data[s_pos])
		goto out;

	// 读取量子的数据直到结尾
	if (count > quantum - q_pos)
		count = quantum - q_pos;

	if (copy_to_user(buff, dptr->data[s_pos] + q_pos, count))
	{
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

out:
	up(&dev->sem);
	return retval;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("pk");
