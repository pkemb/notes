#include "pkbus.h"
#include "pkdevice.h"

int pkdevice_register(struct pkdevice *pkdev)
{
    pkdev->dev.bus = &pkbus;
    pkdev->dev.parent = &pkbus_device;
    pkdev->dev.release = pkdevice_release;
    strncpy(pkdev->dev.bus_id, pkdev->name, BUS_ID_SIZE);
    return device_register(&pkdev->dev);
}
EXPORT_SYMBOL(pkdevice_register);

void pkdevice_unregister(struct pkdevice *pkdev)
{
    device_unregister(&pkdev->dev);
}
EXPORT_SYMBOL(pkdevice_unregister);

void pkdevice_release(struct device *dev)
{
    struct pkdevice *pkdev = to_pkdevice(dev);
    PDEBUG("name = %s\n", pkdev->name);
}
