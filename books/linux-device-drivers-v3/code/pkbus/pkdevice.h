#ifndef __PKDEVICE_H__
#define __PKDEVICE_H__

#include <linux/device.h>

struct pkdevice {
    char *name;
    struct device dev;
};

#define to_pkdevice(dev)    container_of(dev, struct pkdevice, dev)

int  pkdevice_register(struct pkdevice *pkdev);
void pkdevice_unregister(struct pkdevice *pkdev);
void pkdevice_release(struct device *dev);

#endif
