#ifndef __PKTIME_H__
#define __PKTIME_H__

#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/ioctl.h>

#define DEVICE_NAME     "pktime"

struct pktime_dev {
    struct cdev cdev;
    // 设备自定义数据
    // 信号量用于保护pktime_dev结构体
    struct semaphore sem;
    // proc 入口
    struct proc_dir_entry *proc_jiffies;
    struct proc_dir_entry *proc_cycles;
};


// proc name
#define PROC_JIFFIES               "pktime_jiffies"
#define PROC_CYCLES                "pktime_cycles"

#define SAFE_REMOVE_PROC_ENTRY(entry, path)       \
     do {                                         \
          if (entry != NULL) {                    \
               remove_proc_entry(path, NULL);     \
               entry = NULL;                      \
          }                                       \
     } while(0)

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

#endif // __PKTIME_H__
