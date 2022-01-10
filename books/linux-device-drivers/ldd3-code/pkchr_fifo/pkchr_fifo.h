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
    // 读写指针
    size_t read_point;   // 指向的位置可读，除非等于write_point
    size_t write_point;  // 指向的位置可写，不可读
    // 读写的休眠队列
    wait_queue_head_t read_queue;
    wait_queue_head_t write_queue;
    // 异步通知
    struct fasync_struct *async_queue;
};

// proc name
#define PROC_NAME               "pkchr_fifo"

#define DEBUG

#undef PDEBUG
#ifdef DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG DEVICE_NAME "[%s:%d] " fmt, __FUNCTION__, __LINE__, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, DEVICE_NAME "[%s:%d] " fmt, __FUNCTION__, __LINE__, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#endif // __PKCHR_H__
