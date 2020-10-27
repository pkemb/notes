#ifndef __SCULL_H__
#define __SCULL_H__

#define SCULL_MAJOR 0
#define SCULL_MINOR 0

struct scull_qset
{
    void **data;
    struct scull_qset *next;
};

struct scull_dev
{
    struct scull_qset *data;        // 指向第一个量子集的指针
    int    quantum;                 // 当前量子的大小
    int    qset;                    // 当前数组大小
    unsigned long size;             // 保存在其中的数据总量
    unsigned int  access_key;       // 由 sculluid 和 scullpriv 使用
    struct semaphore sem;           // 互斥信号量
    struct cdev cdev;               // 字符设备结构
};

int scull_trim(struct scull_dev *dev);

int scull_open(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp);
ssize_t scull_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)

#endif
