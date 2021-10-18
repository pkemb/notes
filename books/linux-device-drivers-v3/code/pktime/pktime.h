#ifndef __PKTIME_H__
#define __PKTIME_H__

#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/ioctl.h>

#define DEVICE_NAME     "pktime"

// proc name
#define PROC_JIFFIES               "pktime_jiffies"
#define PROC_CYCLES                "pktime_cycles"
#define PROC_BUSY                  "pktime_busy"
#define PROC_SCHED                 "pktime_sched"
#define PROC_QUEUE                 "pktime_queue"
#define PROC_SCHEDTO               "pktime_schedto"
#define PROC_TIMER                 "pktime_timer"

#define SAFE_REMOVE_PROC_ENTRY(entry, path)       \
     do {                                         \
          if (entry != NULL) {                    \
               remove_proc_entry(path, NULL);     \
               entry = NULL;                      \
          }                                       \
     } while(0)

enum {
     BUSY = 1,
     SCHED,
     QUEUE,
     SCHEDTO,
} e_delay;

// jiffies和second 的互相转换
// jiffies转换为s
#define J2S(j)      (j/(HZ))
// jiffies转换为ms
#define J2MS(j)     ((j*1000)/(HZ))
// s转换为jiffies
#define S2J(s)      (s*(HZ))
#define MS2J(ms)    ((ms*(HZ))/1000)


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

#define CHECK_POINT(point, label)                 \
     do {                                         \
          if (point == NULL) {                    \
               PDEBUG("%s is NULL\n", #point);    \
               goto label;                        \
          }                                       \
     } while(0)



#endif // __PKTIME_H__
