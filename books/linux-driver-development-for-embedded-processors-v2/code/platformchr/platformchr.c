/*
 * platform设备，主要用于枚举SoC上的设备
 * platform设备驱动用于驱动platform设备
 * platform设备和platform设备驱动挂载在platform bus
 *
 * struct platform_device
 * struct platform_driver
 *      probe / remove
 *      driver
 * struct platform_bus
 *
 * 注意区分platform设备和字符设备的区别，
 * platform是设备的管理与组织方式，是Linux设备模型的一部分。
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/ctype.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/mutex.h>

// for platform
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

#include <linux/miscdevice.h>

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("platform char device");

#define log(fmt, ...)   pr_debug("[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

struct pkmiscdevice {
    struct miscdevice misc;
    struct list_head list;
};
#define to_pkmiscdevice(list)   container_of(list, struct pkmiscdevice, list)

LIST_HEAD(pkmisc_list);   // 通过链表管理所有的 pkmiscdevice
DEFINE_MUTEX(pkmisc_mtx); // 访问链表时需要互斥锁

/*************** miscdevice相关的fops函数 ***************************/

int miscchr_open(struct inode *inode, struct file *filp)
{
    struct pkmiscdevice *pkmisc = NULL;
    log("major = %d, minor = %d\n", MAJOR(inode->i_rdev), MINOR(inode->i_rdev));

    mutex_lock(&pkmisc_mtx);

    list_for_each_entry(pkmisc, &pkmisc_list, list) {
        log("pkmisc name = %s\n", pkmisc->misc.name);
        if (pkmisc->misc.minor == MINOR(inode->i_rdev)) {
            log("found pkmisc\n");
            break;
        }
    }

    mutex_unlock(&pkmisc_mtx);

    // 如果不等表示找到了
    if (&pkmisc_list != &pkmisc->list) {
        filp->private_data = pkmisc;
        return 0;
    }

    return -1;
}

ssize_t miscchr_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    struct pkmiscdevice *pkmisc = (struct pkmiscdevice *)filp->private_data;
    log("size = %d, name = %s\n", size, pkmisc->misc.name);
    return size;
}

ssize_t miscchr_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    struct pkmiscdevice *pkmisc = (struct pkmiscdevice *)filp->private_data;
    log("size = %d, name = %s\n", size, pkmisc->misc.name);
    return size;
}

struct file_operations fops = {
    .read  = miscchr_read,
    .write = miscchr_write,
    .open  = miscchr_open,
};

// 驱动支持的设备列表
static const struct of_device_id my_of_ids[] = {
    {
        .compatible = "pk,helloplatform",
    },
    { }
};

/*
dts文件如下：
&soc {
        helloplatform1 {
                compatible = "pk,helloplatform";
        };
        helloplatform2 {
                compatible = "pk,helloplatform";
        };
};
*/

// 当出现匹配的设备时，会调用此函数，主要功能是字符设备注册
// 根据dts的内容，probe()会被调用两次。name分别是soc:helloplatform1和soc:helloplatform2
int my_probe(struct platform_device *device)
{
    struct pkmiscdevice *pkmisc = NULL;
    int ret = 0;
    log("name = %s\n", device->name);
    log("id = %d\n", device->id);

    mutex_lock(&pkmisc_mtx);

    pkmisc = (struct pkmiscdevice *)kmalloc(sizeof(*pkmisc), GFP_KERNEL);
    if (!pkmisc) {
        log("kmalloc fail\n");
        mutex_unlock(&pkmisc_mtx);
        return -1;
    }
    memset(pkmisc, 0, sizeof(struct pkmiscdevice));

    pkmisc->misc.name = device->name;
    pkmisc->misc.minor = MISC_DYNAMIC_MINOR;
    pkmisc->misc.fops = &fops;

    ret = misc_register(&pkmisc->misc);
    if (ret) {
        log("add misc device fail\n");
        kfree(pkmisc);
        pkmisc = NULL;
        mutex_unlock(&pkmisc_mtx);
        return -1;
    }

    list_add(&pkmisc->list, &pkmisc_list);

    mutex_unlock(&pkmisc_mtx);
    return 0;
}

// 字符设备删除
int my_remove(struct platform_device *device)
{
    struct pkmiscdevice *pkmisc = NULL;
    log("name = %s\n", device->name);

    mutex_lock(&pkmisc_mtx);

    list_for_each_entry(pkmisc, &pkmisc_list, list) {
        log("pkmisc name = %s\n", pkmisc->misc.name);
        if (!strcmp(pkmisc->misc.name, device->name)) {
            break;
        }
    }

    // 如果不等表示找到了
    if (&pkmisc->list != &pkmisc_list) {
        misc_deregister(&pkmisc->misc);
        list_del(&pkmisc->list);
        kfree(pkmisc);
        pkmisc = NULL;
    }

    mutex_unlock(&pkmisc_mtx);

    return 0;
}

static struct platform_driver my_platform_driver = {
    .probe = my_probe,
    .remove = my_remove,
    .driver = {
        .name = MODULE_NAME,
        .of_match_table = my_of_ids,
        .owner = THIS_MODULE,
    }
};

#if 1

static int platformchr_init(void)
{
    log("register platform driver\n");
    platform_driver_register(&my_platform_driver);
    return 0;
}
module_init(platformchr_init);

static void platform_exit(void)
{
    log("unregister platform driver\n");
    platform_driver_unregister(&my_platform_driver);
    return;
}
module_exit(platform_exit);

#else

module_platform_driver(&my_platform_driver); // 代替init和exit函数。

#endif

