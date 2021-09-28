#ifndef __PKCHR_H__
#define __PKCHR_H__

#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/ioctl.h>

#define DEVICE_NAME     "pkchr"
#define MEM_SIZE        16

struct pkchr_dev {
    struct cdev cdev;
    // 设备自定义数据
    char mem[MEM_SIZE];
};

// ioctl 相关的定义
// _IO(type, nr)                构造无参数的命令
// _IOW(type, nr, datatype)     从驱动程序中读
// _IOR(type, nr, datatype)
// _IORW(type, nr, datatype)

#define PKCHR_IOC_MAGIC         'k'
#define PKCHR_IOCLEAR           _IO(PKCHR_IOC_MAGIC, 0)
#define PKCHR_IOC_MAXNR         0

// proc name
#define PROC_NAME               "pkchr"
#define PROC_SEQ_NAME           "pkchr_seq"

#endif // __PKCHR_H__
