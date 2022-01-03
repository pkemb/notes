/*
 * led - 使用pinctrl控制引脚
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
#include <linux/io.h>

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

/*
关于外设物理地址，请参考文档BCM2835-ARM-Peripherals.pdf第6页，以下内容摘自文档。
不要使用GPIO章节给出的地址0x7E200000。

Physical addresses range from 0x3F000000 to 0x3FFFFFFF for peripherals. The
bus addresses for peripherals are set up to map onto the peripheral bus address range
starting at 0x7E000000. Thus a peripheral advertised here at bus address 0x7Ennnnnn is
available at physical address 0x3Fnnnnnn.
*/

#define BCM2837_PERI_BASE		0x3F000000
#define GPIO_BASE			(BCM2837_PERI_BASE + 0x200000)	/* GPIO controller */
#define GPSET0              (GPIO_BASE + 0x1C)
#define GPCLR0              (GPIO_BASE + 0x28)
#define GPLEV0              (GPIO_BASE + 0x34)

static void __iomem *GPSET0_V;
static void __iomem *GPCLR0_V;
static void __iomem *GPLEV0_V;

struct led_dev {
    struct miscdevice misc;
    const char *name;
    int pin_no;
};

void led_on(int pin) {
    unsigned long reg = (unsigned long)GPSET0_V + (pin/32) * 4;
    int offset = pin % 32;
    log("reg = 0x%lx, pin = %d\n", reg, pin);
    iowrite32(1<<offset, (void *)reg);
}

void led_off(int pin) {
    unsigned long reg = (unsigned long)GPCLR0_V + (pin/32) * 4;
    int offset = pin % 32;
    log("reg = 0x%lx, pin = %d\n", reg, pin);
    iowrite32(1<<offset, (void *)reg);
}

int get_pin_value(int pin) {
    unsigned long reg = (unsigned long)GPLEV0_V + (pin/32) * 4;
    int offset = pin % 32;
    int val = ioread32((void *)reg);
    log("reg = 0x%lx, pin = %d, value = %x\n", reg, pin, val);
    return (val >> offset) & 0x1;
}

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

    pin_value = get_pin_value(led->pin_no);
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
        led_on(led->pin_no);
        log("led on\n");
    } else if (!strcmp(cmd, "off")) {
        led_off(led->pin_no);
        log("led off");
    } else if (!strcmp(cmd, "toggle")) {
        value = get_pin_value(led->pin_no);
        value ? led_off(led->pin_no): led_on(led->pin_no);
        log("led toggle\n");
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

&gpio {
    led_pins: led_pins {
        brcm,pins = <27 22 26>;
        brcm,function = <BCM2835_FSEL_GPIO_OUT>;
        brcm,pull = <BCM2835_PUD_UP BCM2835_PUD_UP BCM2835_PUD_UP>;
    };
};

&soc {
    ledred {
        compatible = "pk,RGBleds";
        label = "ledred";
        pinctrl-names = "default";
        pinctrl-0 = <&led_pins>;
        pins = <27>;
    };

    ledgreen {
        compatible = "pk,RGBleds";
        label = "ledgreen";
        pins = <22>;
    };
    ledblue {
        compatible = "pk,RGBleds";
        label = "ledblue";
        pins = <26>;
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

    ret = of_property_read_string(pdev->dev.of_node, "label", &led->name);
    if (ret) {
        log("read label from dts fail, please check\n");
        return -1;
    }
    log("name = %s\n", led->name);

    // get pin number
    ret = of_property_read_u32(pdev->dev.of_node, "pins", &led->pin_no);
    if (ret) {
        log("read pins from dts fail, please check\n");
        return -1;
    }
    log("pin no = %d\n", led->pin_no);

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

static int led_init(void)
{
    log("register led pinctrl\n");
    platform_driver_register(&led_platform_driver);

    // pin脚的功能选项通过设备树，由pinctrl完成，驱动不设置
    GPSET0_V = ioremap(GPSET0, sizeof(u32));
	GPCLR0_V = ioremap(GPCLR0, sizeof(u32));
    GPLEV0_V = ioremap(GPLEV0, sizeof(u32));
    return 0;
}
module_init(led_init);

static void led_exit(void)
{
    log("unregister led pinctrl\n");

    iounmap(GPSET0_V);
	iounmap(GPCLR0_V);
    iounmap(GPLEV0_V);

    platform_driver_unregister(&led_platform_driver);
}
module_exit(led_exit);