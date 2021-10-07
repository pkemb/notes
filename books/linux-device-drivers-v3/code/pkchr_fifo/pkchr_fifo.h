#ifndef __PKCHR_FIFO_H__
#define __PKCHR_FIFO_H__

#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/ioctl.h>

#define DEVICE_NAME     "pkchr_fifo"
#define MEM_SIZE        16

struct pkchr_fifo_dev {
    struct cdev cdev;
    // 设备自定义数据
    // 信号量用于保护pkchr结构体
    struct semaphore sem;
    char mem[MEM_SIZE];
};

// proc name
#define PROC_NAME               "pkchr_fifo"

#endif // __PKCHR_H__
