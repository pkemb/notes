#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include "pkirq.h"

MODULE_AUTHOR("pkemb");
MODULE_LICENSE("GPL");

static int irq = -1;
static unsigned long baseaddr = 0x378;

module_param(irq, int, S_IRUGO);
module_param(baseaddr, long, S_IRUGO);

int probe_irq(unsigned long baseaddr)
{
    int count = 0;
    int irq = -1;
    unsigned long mask = 0;
    if (baseaddr == 0) {
        return -1;
    }
    do {
        mask = probe_irq_on();
        outb(0x10, baseaddr + 2);
        outb(0x00, baseaddr);
        outb(0xFF, baseaddr);
        outb(0x00, baseaddr + 2);
        udelay(5);
        irq = probe_irq_off(mask);

        if (irq == 0) {
            PDEBUG("no irq\n");
            irq = -1;
        }
    } while (irq < 0 && count++ < 10);
    return irq;
}

static int __init pkirq_init(void)
{

    PDEBUG("irq = %d, baseaddr = %ld\n", irq, baseaddr);

    if (irq < 0) {
        switch(baseaddr) {
            case 0x378: irq = 7; break;
            case 0x278: irq = 2; break;
            case 0x3bc: irq = 5; break;
        }
    }

    return 0;
}
module_init(pkirq_init);

static void __exit pkirq_exit(void)
{
    PDEBUG("exit\n");
}
module_exit(pkirq_exit)