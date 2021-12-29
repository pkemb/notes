#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ctype.h>

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("char device, create device node with udev");

#define log(fmt, ...)   pr_debug("[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

struct pkchrdev {
    char *name;
    struct cdev cdev;
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
    log("name = %s\n", pkchr->name);
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

    log("name = %s, size = %d, *pos = %p\n", pkchr->name, size, *off);
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

#define COUNT   1
struct pkchrdev pkchrdevs[COUNT] = {};

static int pkchr_init(void)
{
    int ret = 0;
    int i = 0;

    ret = alloc_chrdev_region(&devno, 0, COUNT, MODULE_NAME);
    if (ret) {
        log("alloc char devno fail, ret = %d\n", ret);
        goto alloc_chrdev_region_fail;
    }
    log("major = %d, minor = %d\n", MAJOR(devno), MINOR(devno));

    for (i=0; i<COUNT; i++) {
        pkchrdevs[i].name = MODULE_NAME;
        cdev_init(&pkchrdevs[i].cdev, &fops);
        cdev_add(&pkchrdevs[i].cdev, MKDEV(MAJOR(devno), i), 1);
    }

    return 0;
alloc_chrdev_region_fail:
    return ret;
}
module_init(pkchr_init);

static void pkchr_exit(void)
{
    int i = 0;
    log("exit");
    for (i=0; i<COUNT; i++) {
        cdev_del(&pkchrdevs[i].cdev);
    }
    if (devno) {
        unregister_chrdev_region(devno, COUNT);
        devno = 0;
    }

    return;
}
module_exit(pkchr_exit);
