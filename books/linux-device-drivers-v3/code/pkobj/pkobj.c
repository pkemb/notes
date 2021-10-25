#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/err.h>

#define DEVICE_NAME "pkobj"

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

static struct kobject  *parent = NULL;
// static struct kobject  *child;
// static struct kset     *kset;

ssize_t att_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
    ssize_t count = 0;
    count += sprintf(buf + count, "hello, sysfs\n");
    count += sprintf(buf + count, "attr name = %s\n", attr->name);
    count += sprintf(buf + count, "attr mode = %04o\n", attr->mode);
    count += sprintf(buf + count, "kobj name = %s\n", kobj->name);
    return count;
}

ssize_t att_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t size)
{
    char *mem = (char *)kmalloc(sizeof(char)*(size+1), GFP_KERNEL);
    if (mem == NULL)
        return size;
    memcpy(mem, buf, size);
    mem[size] = 0;
    PDEBUG("%s\n", mem);
    kfree(mem);
    return size;
}

void kobj_release(struct kobject *kobj)
{
    PDEBUG("call release function\n");
}

struct attribute att = {
    .name = "test",
    .mode = 0666,
};

struct sysfs_ops att_ops = {
    .show  = att_show,
    .store = att_store,
};

static int __init pkobj_init(void)
{
    int ret = 0;
    parent = kobject_create_and_add("pkobj", NULL);
    if (parent == NULL)
    {
        PDEBUG("kobject add fail\n");
        ret = PTR_ERR(parent);
        goto fail_add;
    }
    parent->ktype->sysfs_ops = &att_ops;
    parent->ktype->release   = kobj_release;
    ret = sysfs_create_file(parent, &att);
    if (ret != 0) {
        PDEBUG("create file fail, ret = %d\n", ret);
        goto fail_create_fail;
    }
    return 0;
fail_create_fail:
    kobject_del(parent);
fail_add:
    return -1;
}
module_init(pkobj_init);

static void __exit pkobj_exit(void)
{
    sysfs_remove_file(parent, &att);
    kobject_del(parent);
}
module_exit(pkobj_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("pkemb");
MODULE_DESCRIPTION("kobject test");
