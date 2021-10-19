#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/timex.h>
#include <linux/sched.h>
#include <linux/hardirq.h>
#include <linux/interrupt.h>

#include "pktime.h"

struct proc_dir_entry *proc_jiffies = NULL;
struct proc_dir_entry *proc_cycles  = NULL;
struct proc_dir_entry *proc_busy    = NULL;
struct proc_dir_entry *proc_sched   = NULL;
struct proc_dir_entry *proc_queue   = NULL;
struct proc_dir_entry *proc_schedto = NULL;
struct proc_dir_entry *proc_timer   = NULL;
struct proc_dir_entry *proc_tasklet = NULL;

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

#define TIMER_DELAY     30

typedef struct {
    long prev_jiffies;
    int  loops;
    char *buf;
    wait_queue_head_t wait;
    struct timer_list timer;
} timer_data_t;

void pktime_timer_fn(unsigned long data)
{
    timer_data_t *timer_data = (timer_data_t *)data;
    long j = jiffies;
    timer_data->buf += sprintf(timer_data->buf,
                               "%-10li %-10li %-6d %-10d %-4d %-10s\n",
                               timer_data->prev_jiffies,
                               j,
                               in_interrupt() ? 1 : 0,
                               current->pid,
                               smp_processor_id(),
                               current->comm);
    PDEBUG("loops = %d\n", timer_data->loops);
    if (--timer_data->loops) {
        timer_data->timer.expires += TIMER_DELAY;
        timer_data->prev_jiffies = j;
        add_timer(&timer_data->timer);
    } else {
        wake_up_interruptible(&timer_data->wait);
    }
}

static timer_data_t timer_data;

int pktime_timer(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    long j = 0;
    char *buf2 = buf;

    buf2 += sprintf(buf2, "%-10s %-10s %-6s %-10s %-4s %-10s\n",
                            "pre",
                            "current",
                            "inirq",
                            "pid",
                            "cpu",
                            "command");
    memset(&timer_data, 0, sizeof(timer_data));

    // 初始化结构体
    init_waitqueue_head(&timer_data.wait);
    init_timer(&timer_data.timer);
    j = jiffies;
    timer_data.prev_jiffies = j;
    timer_data.loops = 5;
    timer_data.buf   = buf2;     // 打印数据

    timer_data.timer.expires  = j + TIMER_DELAY;
    timer_data.timer.function = pktime_timer_fn;
    timer_data.timer.data     = (unsigned long)&timer_data;

    PDEBUG("j = %ld\n", j);
    // 启动定时器
    add_timer(&timer_data.timer);

    // 等待缓冲区
    wait_event_interruptible(timer_data.wait, !timer_data.loops);
    PDEBUG("loops = %d\n", timer_data.loops);
    *eof = 1;
    buf2 = timer_data.buf;
    return buf2 - buf;
}

typedef struct {
    long prev_jiffies;
    int  loops;
    char *buf;
    wait_queue_head_t wait;
    struct tasklet_struct tasklet;
} tasklet_data_t;

void pktime_tasklet_fn(unsigned long arg)
{
    tasklet_data_t *data = (tasklet_data_t *)arg;
    long j = jiffies;
    data->buf += sprintf(data->buf,
                        "%-10li %-10li %-6d %-10d %-4d %-10s\n",
                        data->prev_jiffies,
                        j,
                        in_interrupt() ? 1 : 0,
                        current->pid,
                        smp_processor_id(),
                        current->comm);
    if (data->loops) {
        data->loops--;
        data->prev_jiffies = j;
        tasklet_schedule(&data->tasklet);
    } else {
        wake_up_interruptible(&data->wait);
    }
}

int pktime_tasklet(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    static tasklet_data_t tasklet_data;
    char *buf2 = buf;
    int j = 0;

    buf2 += sprintf(buf2, "%-10s %-10s %-6s %-10s %-4s %-10s\n",
                        "pre",
                        "current",
                        "inirq",
                        "pid",
                        "cpu",
                        "command");

    memset(&tasklet_data, 0, sizeof(tasklet_data));
    init_waitqueue_head(&tasklet_data.wait);

    j = jiffies;

    tasklet_data.loops = 5;
    tasklet_data.buf = buf2;
    tasklet_data.prev_jiffies = j;
    tasklet_init(&tasklet_data.tasklet, pktime_tasklet_fn, (unsigned long)&tasklet_data);

    // 启动tasklet的调用，kernel会选择一个合适的时间调用。程序不能指定时间。
    tasklet_schedule(&tasklet_data.tasklet);

    wait_event_interruptible(tasklet_data.wait, !tasklet_data.loops);

    buf2 = tasklet_data.buf;
    *eof = 1;
    return buf2 - buf;
}

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
    SAFE_REMOVE_PROC_ENTRY(proc_timer,   PROC_TIMER);
    SAFE_REMOVE_PROC_ENTRY(proc_tasklet, PROC_TASKLET);
}
module_exit(pktime_exit);

static int __init pktime_init(void)
{
    PDEBUG("%s init\n", DEVICE_NAME);

    // 在 /proc 根目录创建入口
    proc_jiffies = create_proc_read_entry(PROC_JIFFIES, 0, NULL, pktime_jiffies, NULL);
    CHECK_POINT(proc_jiffies, create_proc_fail);

    proc_cycles = create_proc_read_entry(PROC_CYCLES, 0, NULL, pktime_cycles, NULL);
    CHECK_POINT(proc_cycles, create_proc_fail);

    proc_busy = create_proc_read_entry(PROC_BUSY, 0, NULL, pktime_delay, (void *)BUSY);
    CHECK_POINT(proc_busy, create_proc_fail);

    proc_sched = create_proc_read_entry(PROC_SCHED, 0, NULL, pktime_delay, (void *)SCHED);
    CHECK_POINT(proc_sched, create_proc_fail);

    proc_queue = create_proc_read_entry(PROC_QUEUE, 0, NULL, pktime_delay, (void *)QUEUE);
    CHECK_POINT(proc_queue, create_proc_fail);

    proc_schedto = create_proc_read_entry(PROC_SCHEDTO, 0, NULL, pktime_delay, (void *)SCHEDTO);
    CHECK_POINT(proc_schedto, create_proc_fail);

    proc_timer = create_proc_read_entry(PROC_TIMER, 0, NULL, pktime_timer, NULL);
    CHECK_POINT(proc_timer, create_proc_fail);

    proc_tasklet = create_proc_read_entry(PROC_TASKLET, 0, NULL, pktime_tasklet, NULL);
    CHECK_POINT(proc_tasklet, create_proc_fail);

    return 0;

create_proc_fail:
    pktime_exit();
    return -1;
}
module_init(pktime_init);

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPLv2");
