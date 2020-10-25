#include <linux/module.h>
#include <linux/init.h>
#include "scull.h"

static int scull_major = SCULL_MAJOR;
static int scull_minor = SCULL_MINOR;

static int __init scull_init(void)
{
	dev_t dev;
	int result;

	if (scull_major)
	{
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	}
	printk("scull init\n");
	return 0;
}
module_init(scull_init);

static void __exit scull_exit(void)
{
	printk("scull exit\n");
}
module_exit(scull_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("pk");
