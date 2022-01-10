#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/device.h>

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("char device, create device node with udev");

#define log(fmt, ...)   pr_debug("[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

struct pkchrdev {
    dev_t dev;
    struct cdev cdev;
    struct device *device;
};
#define to_pkchrdev(ptr)     container_of(ptr, struct pkchrdev, cdev)

static dev_t devno = 0;

int pkchr_open(struct inode *inode, struct file *filp)
{
    struct pkchrdev *pkchr = to_pkchrdev(inode->i_cdev);
    log("major = %d, minor = %d\n", MAJOR(inode->i_rdev), MINOR(inode->i_rdev));
    filp->private_data = pkchr;
    return 0;
}

ssize_t pkchr_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    struct pkchrdev *pkchr = filp->private_data;
    dev_t dev = pkchr->dev;
    log("dev = 0x%08x, major=%d, minor=%d\n", dev, MAJOR(dev), MINOR(dev));
    return size;
}

void print_buf(char *str, size_t len)
{
    size_t i = 0;
    for (i = 0; i<len; i++) {
        log("str[%d] = 0x%x, %c\n", i, str[i], isprint(str[i])?str[i]:'.');
    }
}

ssize_t pkchr_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    struct pkchrdev *pkchr = filp->private_data;
    char *ptr = NULL;
    dev_t dev = pkchr->dev;

    log("dev = 0x%08x, major=%d, minor=%d\n", dev, MAJOR(dev), MINOR(dev));
    ptr = kmalloc(size, GFP_KERNEL);
    log("ptr = %p\n", ptr);
    if (ptr) {
        int ret = copy_from_user(ptr, buff, size); // 返回值是剩余未拷贝的字符数
        log("ret = %d\n", ret);
        if (!ret) {
            print_buf(ptr, size);
        }
        kfree(ptr);
    }
    *off = *off + size; // 更新位置，加上本次写入的长度
    return size;
}

struct file_operations fops = {
    .read = pkchr_read,
    .write = pkchr_write,
    .open  = pkchr_open,
};

#define COUNT   4
struct pkchrdev pkchrdevs[COUNT] = {};
struct class   *pkclass = NULL;

static int pkchr_init(void)
{
    int ret = 0;
    int i = 0;
    dev_t dev = 0;

    // 最后一个参数设备名会出现在文件 /proc/devices
    ret = alloc_chrdev_region(&devno, 0, COUNT, MODULE_NAME);
    if (ret) {
        log("alloc char devno fail, ret = %d\n", ret);
        goto alloc_chrdev_region_fail;
    }
    log("major = %d, minor = %d\n", MAJOR(devno), MINOR(devno));

    pkclass = class_create(THIS_MODULE, "pkclass");
    if (IS_ERR(pkclass)) {
        log("class create fail\n");
        goto class_create_fail;
    }

    for (i=0; i<COUNT; i++) {
        dev = MKDEV(MAJOR(devno), i);
        // 添加字符设备
        pkchrdevs[i].dev = dev;
        cdev_init(&pkchrdevs[i].cdev, &fops);
        cdev_add(&pkchrdevs[i].cdev, dev, 1);

        // 创建device
        pkchrdevs[i].device = device_create(pkclass, NULL, dev, NULL, MODULE_NAME"%d", i);
        if (IS_ERR(pkchrdevs[i].device)) {
            log("device %d create fail\n", i);
            goto device_create_fail;
        }
    }

    return 0;
device_create_fail:
    for (i=0; i<COUNT; i++) {
        if (!IS_ERR(pkchrdevs[i].device)) {
            device_destroy(pkclass, devno);
        }
    }
    class_destroy(pkclass);
class_create_fail:
    unregister_chrdev_region(devno, COUNT);
alloc_chrdev_region_fail:
    return ret;
}
module_init(pkchr_init);

static void pkchr_exit(void)
{
    int i = 0;
    log("exit");
    for (i=0; i<COUNT; i++) {
        device_destroy(pkclass, pkchrdevs[i].dev);
        cdev_del(&pkchrdevs[i].cdev);
    }

    class_destroy(pkclass);

    if (devno) {
        unregister_chrdev_region(devno, COUNT);
        devno = 0;
    }

    return;
}
module_exit(pkchr_exit);
