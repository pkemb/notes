/*
 * misc字符设备，共享同一个主设备号
 * 1. 创建结构体 struct miscdevice
 * 2. 设置结构体成员 minor / name / fops
 *      minor设置MISC_DYNAMIC_MINOR表示自动分配，否则使用指定子设备号
 *      name会出现在/proc/misc，并会自动创建设备文件/dev/<name>
 * 3. 调用 misc_register()
 * 4. 退出时调用 misc_deregister()
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/ctype.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("misc char device");

#define log(fmt, ...)   pr_debug("[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

int     miscchr_open(struct inode *inode, struct file *filp);
ssize_t miscchr_read(struct file *filp, char __user *buff, size_t size, loff_t *off);
ssize_t miscchr_write(struct file *filp, const char __user *buff, size_t size, loff_t *off);

struct file_operations fops = {
    .read  = miscchr_read,
    .write = miscchr_write,
    .open  = miscchr_open,
};

struct miscdevice miscdev = {
    .name = "pkmisc",            //name会出现在/proc/misc，并会自动创建设备文件/dev/<name>
    .minor = MISC_DYNAMIC_MINOR, //MISC_DYNAMIC_MINOR表示自动分配，否则使用指定子设备号
    .fops = &fops,
};

static int miscchr_init(void)
{
    int ret = misc_register(&miscdev);
    log("misc register ret = %d\n", ret);
    log("miscdev addr = %p\n", &miscdev);
    return ret;
}
module_init(miscchr_init);

static void miscchr_exit(void)
{
    misc_deregister(&miscdev);
    return;
}
module_exit(miscchr_exit);

int miscchr_open(struct inode *inode, struct file *filp)
{
    log("major = %d, minor = %d\n", MAJOR(inode->i_rdev), MINOR(inode->i_rdev));
    filp->private_data = &miscdev;
    return 0;
}

ssize_t miscchr_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    struct miscdevice *misc = (struct miscdevice *)filp->private_data;
    log("size = %d, name = %s\n", size, misc->name);
    return size;
}

void print_buf(char *str, size_t len)
{
    size_t i = 0;
    for (i = 0; i<len; i++) {
        log("str[%d] = 0x%x, %c\n", i, str[i], isprint(str[i])?str[i]:'.');
    }
}

ssize_t miscchr_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    struct miscdevice *misc = (struct miscdevice *)filp->private_data;
    char *ptr = NULL;
    log("size = %d, name = %s\n", size, misc->name);
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
