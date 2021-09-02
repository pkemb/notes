#include <linux/module.h>
#include <linux/init.h>
#include "pkchr.h"

static int __init pkchr_init(void)
{
    printk(KERN_INFO"hello\n");
    return 0;
}
module_init(pkchr_init);

static void __exit pkchr_exit(void)
{
    printk(KERN_INFO"exit\n");
}
module_exit(pkchr_exit);

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPLv2");
