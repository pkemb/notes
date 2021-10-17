#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/timex.h>

#include "pktime.h"

struct proc_dir_entry *proc_jiffies = NULL;
struct proc_dir_entry *proc_cycles  = NULL;

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
    *start = buf;
    module_put(THIS_MODULE);

    return len;
}

int pktime_cycles(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    cycles_t c = 0;
    int len = 0;
    if (!try_module_get(THIS_MODULE))
        return 0;

    c = get_cycles();
    len += sprintf(buf + len, "cycles = %lld\n", c);
    *start = buf;

    module_put(THIS_MODULE);

    return len;
}

static int __init pktime_init(void)
{
    PDEBUG("%s init\n", DEVICE_NAME);

    // 在 /proc 根目录创建入口
    proc_jiffies = create_proc_read_entry(PROC_JIFFIES, 0, NULL, pktime_jiffies, NULL);
    if (proc_jiffies == NULL) {
        PDEBUG("create %s fail\n", PROC_JIFFIES);
        goto create_proc_fail;
    }

    proc_cycles = create_proc_read_entry(PROC_CYCLES, 0, NULL, pktime_cycles, NULL);
    if (proc_cycles == NULL) {
        PDEBUG("create %s fail\n", PROC_CYCLES);
        goto create_proc_fail;
    }

    return 0;

create_proc_fail:
    SAFE_REMOVE_PROC_ENTRY(proc_jiffies, PROC_JIFFIES);
    SAFE_REMOVE_PROC_ENTRY(proc_cycles,  PROC_CYCLES);
    return -1;
}
module_init(pktime_init);

static void __exit pktime_exit(void)
{
    PDEBUG("%s exit\n", DEVICE_NAME);

    // 删除proc入口
    SAFE_REMOVE_PROC_ENTRY(proc_jiffies, PROC_JIFFIES);
    SAFE_REMOVE_PROC_ENTRY(proc_cycles,  PROC_CYCLES);
}
module_exit(pktime_exit);

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPLv2");
