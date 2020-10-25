#!/bin/sh

module="scull"
device="scull"
mode="644"

/sbin/insmod ./$module.ko $* || exit 1

# 删除原有节点
rm -r /dev/${device}[0-3]

major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3

# 给定适当的组属性及许可，并修改属组。
group="staff"
grep -q '^staff:' /etc/group || group="wheel"

chgrp $group /dev/${device}[0-3]
chmod $mod   /dev/${device}[0-3]