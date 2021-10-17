#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/timex.h>
#include <linux/sched.h>

#include "pktime.h"

struct proc_dir_entry *proc_jiffies = NULL;
struct proc_dir_entry *proc_cycles  = NULL;
struct proc_dir_entry *proc_busy    = NULL;
struct proc_dir_entry *proc_sched   = NULL;
struct proc_dir_entry *proc_queue   = NULL;
struct proc_dir_entry *proc_schedto = NULL;

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

    do_gettimeofday(&timeval);
    timespec = current_kernel_time();
    len += sprintf(buf + len, "do_gettimeofday: %ld.%ld\n",
                              timeval.tv_sec, timeval.tv_usec);
    len += sprintf(buf + len, "current_kernel_time: %ld.%ld\n",
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

int pktime_delay(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    long before = 0;
    long after  = 0;
    long delay  = S2J(1);
    int len = 0;
    wait_queue_head_t wait;

    if (!try_module_get(THIS_MODULE))
        return 0;

    init_waitqueue_head(&wait);

    before = jiffies;
    after  = before + delay;

    switch((int)data) {
    case BUSY:
        while (time_before(jiffies, after))
            cpu_relax();
        break;
    case SCHED:
        while (time_before(jiffies, after))
            schedule();
        break;
    case QUEUE:
        wait_event_interruptible_timeout(wait, 0, delay); // 第二个参数，唤醒条件，一直为false
        break;
    case SCHEDTO:
        // 如果不更改进程状态，schedule_timeout()等同于schedule()
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(delay);
    }

    after = jiffies;
    len += sprintf(buf + len, "%9li %9li\n", before, jiffies);

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

    proc_busy = create_proc_read_entry(PROC_BUSY, 0, NULL, pktime_delay, (void *)BUSY);
    if (proc_busy == NULL) {
        PDEBUG("create %s fail\n", PROC_BUSY);
        goto create_proc_fail;
    }

    proc_sched = create_proc_read_entry(PROC_SCHED, 0, NULL, pktime_delay, (void *)SCHED);
    if (proc_sched == NULL) {
        PDEBUG("create %s fail\n", PROC_SCHED);
        goto create_proc_fail;
    }

    proc_queue = create_proc_read_entry(PROC_QUEUE, 0, NULL, pktime_delay, (void *)QUEUE);
    if (proc_queue == NULL) {
        PDEBUG("create %s fail\n", PROC_QUEUE);
        goto create_proc_fail;
    }

    proc_schedto = create_proc_read_entry(PROC_SCHEDTO, 0, NULL, pktime_delay, (void *)SCHEDTO);
    if (proc_schedto == NULL) {
        PDEBUG("create %s fail\n", PROC_SCHEDTO);
        goto create_proc_fail;
    }

    return 0;

create_proc_fail:
    SAFE_REMOVE_PROC_ENTRY(proc_jiffies, PROC_JIFFIES);
    SAFE_REMOVE_PROC_ENTRY(proc_cycles,  PROC_CYCLES);
    SAFE_REMOVE_PROC_ENTRY(proc_busy,    PROC_BUSY);
    SAFE_REMOVE_PROC_ENTRY(proc_sched,   PROC_SCHED);
    SAFE_REMOVE_PROC_ENTRY(proc_queue,   PROC_QUEUE);
    SAFE_REMOVE_PROC_ENTRY(proc_schedto, PROC_SCHEDTO);
    return -1;
}
module_init(pktime_init);

static void __exit pktime_exit(void)
{
    PDEBUG("%s exit\n", DEVICE_NAME);

    // 删除proc入口
    SAFE_REMOVE_PROC_ENTRY(proc_jiffies, PROC_JIFFIES);
    SAFE_REMOVE_PROC_ENTRY(proc_cycles,  PROC_CYCLES);
    SAFE_REMOVE_PROC_ENTRY(proc_busy,    PROC_BUSY);
    SAFE_REMOVE_PROC_ENTRY(proc_sched,   PROC_SCHED);
    SAFE_REMOVE_PROC_ENTRY(proc_queue,   PROC_QUEUE);
    SAFE_REMOVE_PROC_ENTRY(proc_schedto, PROC_SCHEDTO);
}
module_exit(pktime_exit);

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPLv2");
