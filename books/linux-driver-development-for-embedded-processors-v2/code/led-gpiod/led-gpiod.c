/*
 * led - 使用GPIOD控制引脚的电平
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
#include <linux/of.h>
#include <linux/errno.h>

// for platform
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

#include <linux/miscdevice.h>

//for gpio
#include <linux/gpio/consumer.h>

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("led");

#define log(fmt, ...)   pr_debug("[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

struct led_dev {
    struct miscdevice misc;
    const char *name;
    struct gpio_desc *gpio;
};

/*************** miscdevice相关的fops函数 ***************************/

ssize_t led_misc_read(struct file *filp, char __user *buff, size_t size, loff_t *off)
{
    struct led_dev *led = (struct led_dev *)filp->private_data;
    int pin_value = 0;
    size_t value_len = 0;
    char *value = NULL;
    int ret = 0;
    log("size = %d, name = %s\n", size, led->name);

    value = kzalloc(size, GFP_KERNEL);
    if (!value) {
        log("kmalloc fail\n");
        return -1;
    }

    pin_value = gpiod_get_value(led->gpio);
    value_len = snprintf(value, size - 1, "%s: %d\n", led->name, pin_value);
    value[size - 1] = 0;

    if (*off == 0) {
        if (copy_to_user(buff, value, value_len)) {
            log("copy to user fail\n");
            kfree(value);
            return -EFAULT;
        }
        *off += value_len;
        ret = value_len;
    }

    kfree(value);
    return ret;
}

ssize_t led_misc_write(struct file *filp, const char __user *buff, size_t size, loff_t *off)
{
    struct led_dev *led = (struct led_dev *)filp->private_data;

    char cmd[10] = {0};
    int len = min((size_t)10, size);
    char *str = NULL;
    int value = 0;

    log("size = %d, name = %s\n", size, led->name);

    if (copy_from_user(cmd, buff, len)) {
        log("copy from user error\n");
        return -EFAULT;
    }

    // remove \n
    str = strchr(cmd, '\n');
    if (str)
        *str = 0;

    log("cmd = %s\n", cmd);

    if (!strcmp(cmd, "on")) {
        gpiod_set_value(led->gpio, 1);
        log("led on\n");
    } else if (!strcmp(cmd, "off")) {
        gpiod_set_value(led->gpio, 0);
        log("led off");
    } else if (!strcmp(cmd, "toggle")) {
        value = gpiod_get_value(led->gpio);
        gpiod_set_value(led->gpio, !value);
    } else {
        log("wrong command!\n");
    }

    *off += size;
    return size;
}

struct file_operations misc_fops = {
    .read  = led_misc_read,
    .write = led_misc_write,
};


// 驱动支持的设备列表
static const struct of_device_id led_of_ids[] = {
    {
        .compatible = "pk,RGBleds",
    },
    { }
};

/*
设备树的配置如下：

&soc {
    ledred {
        compatible = "pk,RGBleds";
        label = "ledred";
        led-gpios = <&gpio 27 GPIO_ACTIVE_HIGH>;
    };
    ledgreen {
        compatible = "pk,RGBleds";
        label = "ledgreen";
        led-gpios = <&gpio 22 GPIO_ACTIVE_HIGH>;
    };
    ledblue {
        compatible = "pk,RGBleds";
        label = "ledblue";
        led-gpios = <&gpio 26 GPIO_ACTIVE_HIGH>;
    };

};
*/

// 创建misc deivce，获取gpio
int led_probe(struct platform_device *pdev)
{
    struct led_dev *led = NULL;
    int ret = 0;

    log("name = %s, id = %d\n", pdev->name, pdev->id);

    // Memory allocated with this function is automatically freed on driver detach.
    led = devm_kzalloc(&pdev->dev, sizeof(*led), GFP_KERNEL);
    log("led = %p\n", led);
    if (!led) {
        log("devm_kzalloc fail\n");
        return -1;
    }

    led->gpio = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_HIGH);
    log("led->gpio = %p\n", led->gpio);
    if (IS_ERR(led->gpio)) {
        log("get gpio fail, err = %ld\n", PTR_ERR(led->gpio));
        return -1;
    }

    ret = of_property_read_string(pdev->dev.of_node, "label", &led->name);
    if (ret) {
        log("read label from dts fail, please check\n");
        return -1;
    }

    led->misc.name  = led->name;
    led->misc.minor = MISC_DYNAMIC_MINOR;
    led->misc.fops  = &misc_fops;
    led->misc.mode  = 0666;

    ret = misc_register(&led->misc);
    if (ret) {
        log("add misc device fail\n");
        return -1;
    }

    platform_set_drvdata(pdev, led);
    return 0;
}

// 销毁 misc device，释放gpio
int led_remove(struct platform_device *pdev)
{
    struct led_dev *led = platform_get_drvdata(pdev);
    log("led name = %s\n", led->name);
    devm_gpiod_put(&pdev->dev, led->gpio);
    misc_deregister(&led->misc);
    return 0;
}

static struct platform_driver led_platform_driver = {
    .probe = led_probe,
    .remove = led_remove,
    .driver = {
        .name = MODULE_NAME,
        .of_match_table = led_of_ids,
        .owner = THIS_MODULE,
    }
};
module_platform_driver(led_platform_driver);