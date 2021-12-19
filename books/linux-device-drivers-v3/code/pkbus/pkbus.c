#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/device.h>

#define DEBUG
#include "pkbus.h"

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPL");

/************ bus 属性 ***************/

ssize_t pkbus_attr_name_show(struct bus_type *bus, char *buf)
{
    ssize_t size = 0;
    if (bus)
        size += sprintf(buf + size, "bus name = %s\n", bus->name);
    return size;
}

static struct bus_attribute pkbus_attrs[] = {
    __ATTR(name, 0444, pkbus_attr_name_show, NULL),
    { },
};

/************ bus 设备属性 ***************/

static struct device_attribute pkbus_device_attrs[] = {
    { },
};

/************ bus 设备驱动属性 ***************/

static struct driver_attribute pkbus_driver_attrs[] = {
    { },
};

/************ bus 方法 ***************/

int pkbus_match(struct device *dev, struct device_driver *drv)
{
    return 0;
}

int pkbus_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    return 0;
}

static struct bus_type pkbus = {
    .name      = "pk",
    .bus_attrs = pkbus_attrs,
    .dev_attrs = pkbus_device_attrs,
    .drv_attrs = pkbus_driver_attrs,
    .match     = pkbus_match,
    .uevent    = pkbus_uevent,
};

void pkbus_device_release(struct device *dev)
{
    PDEBUG("release pkbus_device\n");
}

static struct device pkbus_device = {
    .bus_id = "pk0",    // 在总线上唯一标识设备的字符串
    .release  = pkbus_device_release,
};

static int __init pkbus_init(void)
{
    int ret = 0;
    PDEBUG("pkbus init\n");

    ret = bus_register(&pkbus);
    if (ret) {
        PDEBUG("pkbus register fail, ret = %d\n", ret);
        goto bus_register_fail;
    }

    ret = device_register(&pkbus_device);
    if (ret) {
        PDEBUG("register device %s fail, ret = %d\n", pkbus_device.bus_id, ret);
        goto device_register_fail;
    }
    return 0;

device_register_fail:
    bus_unregister(&pkbus);
bus_register_fail:
    return ret;
}
module_init(pkbus_init);

static void __exit pkbus_exit(void)
{
    PDEBUG("pkbus exit\n");
    device_unregister(&pkbus_device);
    bus_unregister(&pkbus);
}
module_exit(pkbus_exit);
