#include <linux/init.h>    // 指定初始化和清除函数
#include <linux/module.h>  // 含有可装载模块需要的大量符合和函数的定义
#include <linux/kernel.h>  // printk

static int __init hello_init(void)
{
    printk(KERN_ALERT "hello world!\n");
}
module_init(hello_init);

static void __exit hello_exit(void)
{
    printk(KERN_ALERT "Googbye hello world!\n");
}
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");         // 代码使用的许可证
MODULE_AUTHOR("pk");                    // 模块作者
MODULE_DESCRIPTION("hello world");      // 说明模块用途的简短描述
MODULE_VERSION("0.1");                  // 版本
MODULE_ALIAS("helloworld");             // 模块别名
// MODULE_DEVICE_TABLE()