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

/*************** miscdevice相关的fops函数 ***************************/

ssize_t miscchr_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    struct miscdevice *misc = (struct miscdevice *)filp->private_data;
    log("misc = %p\n", misc);
    log("size = %d, name = %s\n", size, misc->name);
    return size;
}

ssize_t miscchr_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    struct miscdevice *misc = (struct miscdevice *)filp->private_data;
    log("misc = %p\n", misc);
    log("size = %d, name = %s\n", size, misc->name);
    return size;
}

struct file_operations fops = {
    .read  = miscchr_read,
    .write = miscchr_write,
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
    struct miscdevice *misc = NULL;
    int ret = 0;
    log("name = %s, id = %d\n", device->name, device->id);

    // Memory allocated with this function is automatically freed on driver detach.
    misc = devm_kzalloc(&device->dev, sizeof(*misc), GFP_KERNEL);
    log("misc = %p\n", misc);
    if (!misc) {
        log("devm_kzalloc fail\n");
        return -1;
    }

    misc->name  = device->name;
    misc->minor = MISC_DYNAMIC_MINOR;
    misc->fops  = &fops;
    misc->mode  = 0666;

    ret = misc_register(misc);
    if (ret) {
        log("add misc device fail\n");
        return -1;
    }

    platform_set_drvdata(device, misc);

    return 0;
}

// 字符设备删除
int my_remove(struct platform_device *device)
{
    struct miscdevice *misc = NULL;
    log("name = %s\n", device->name);

    misc = platform_get_drvdata(device);
    misc_deregister(misc);

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

