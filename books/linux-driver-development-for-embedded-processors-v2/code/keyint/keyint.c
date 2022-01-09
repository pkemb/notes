/*
按键中断demo

dts如下：
&soc {
    keyint {
        compatible = "pk,keyint";
        key-gpios = <&gpio 23 GPIO_ACTIVE_HIGH>;
        led-gpios = <&gpio 27 GPIO_ACTIVE_HIGH>; // pin27 gpio red
    };
};

按键电路图
          5V
          ┃
         ┏┻┓
         ┃ ┃ 10K       KEY
         ┗┳┛         ╱
          ┃         ╱  ⇲
GPIO23━━━━┻━━━━━━━━━     ━━━━━━━━GND
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

// for interrupt
#include <linux/interrupt.h>

// for timer
#include <linux/timer.h>

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("key interrupt");

#define log(fmt, ...)   pr_debug("[%s:%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

struct pkkeyint {
    int irq;
    struct gpio_desc *key;
    struct gpio_desc *led;
    struct timer_list timer;
};

void key_irq_timer(struct timer_list *timer)
{
    struct pkkeyint *keyint = container_of(timer, struct pkkeyint, timer);
    int value = 0;
    int led_value = 0;

    value = gpiod_get_value(keyint->key);
    log("value = %d\n", value);
    if (value == 0) {
        log("key pressed\n");
        led_value = gpiod_get_value(keyint->led);
        gpiod_set_value(keyint->led, !led_value);
    }
    return;
}

// IRQ_NONE：驱动程序负责的设备没有产生中断
// IRQ_HANDLED：直接在中断上下文处理
// IRQ_WAKE_THREAD：需要调度进程上下文处理
irqreturn_t key_irq_handler(int irq, void *dev_id)
{
    struct pkkeyint *keyint = dev_id;
    int value = 0;

    value = gpiod_get_value(keyint->key);
    log("irq = %d, key value = %d\n", irq, value);

    // 消抖，10ms 后运行定时器
    mod_timer(&keyint->timer, jiffies + msecs_to_jiffies(10));
    return IRQ_HANDLED;
}

// 驱动支持的设备列表
static const struct of_device_id keyint_of_ids[] = {
    {
        .compatible = "pk,keyint",
    },
    { }
};

// 注册IRQ
int keyint_probe(struct platform_device *pdev)
{
    struct pkkeyint *keyint = NULL;
    int ret = 0;

    keyint = devm_kzalloc(&pdev->dev, sizeof(*keyint), GFP_KERNEL);
    if (!keyint) {
        log("devm_kzalloc fail\n");
        return -1;
    }

    // 通过gpiod获取硬件IRQ
    keyint->key = devm_gpiod_get(&pdev->dev, "key", GPIOD_IN);
    if (IS_ERR(keyint->key)) {
        log("get key gpio fail, please check\n");
        return -1;
    }
    keyint->irq = gpiod_to_irq(keyint->key);
    if (keyint->irq < 0) {
        log("get irq fail, please check\n");
        return -1;
    }
    log("irq number is %d\n", keyint->irq);

    // 申请中断，成功返回0
    ret =  devm_request_irq(
                    &pdev->dev,
                    keyint->irq,
                    key_irq_handler,
	                IRQF_SHARED | IRQF_TRIGGER_FALLING,
                    MODULE_NAME,
                    (void *)keyint);
    if (ret) {
        log("request irq fail, please check, ret = %d\n", ret);
        return ret;
    }

    // 获取LED gpio描述符
    keyint->led = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(keyint->led)) {
        log("get led gpio fail, please check\n");
        return -1;
    }

    // 初始化定时器
    timer_setup(&keyint->timer, key_irq_timer, 0);
    add_timer(&keyint->timer);

    platform_set_drvdata(pdev, keyint);

    return 0;
}

int keyint_remove(struct platform_device *pdev)
{
    struct pkkeyint *keyint = NULL;
    keyint = platform_get_drvdata(pdev);
    del_timer(&keyint->timer);
    return 0;
}

static struct platform_driver keyint_platform_driver = {
    .probe = keyint_probe,
    .remove = keyint_remove,
    .driver = {
        .name = MODULE_NAME,
        .of_match_table = keyint_of_ids,
        .owner = THIS_MODULE,
    }
};
module_platform_driver(keyint_platform_driver);
