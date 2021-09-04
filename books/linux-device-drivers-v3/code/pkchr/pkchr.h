#ifndef __PKCHR_H__
#define __PKCHR_H__

#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

#define DEVICE_NAME     "pkchr"
#define MEM_SIZE        16

struct pkchr_dev {
    struct cdev cdev;
    // 设备自定义数据
    char mem[MEM_SIZE];
};

#endif // __PKCHR_H__
