#!/bin/sh

module="pkchr"
device="pkchr"

insmod ./${module}.ko $* || exit 1
major=`grep $module /proc/devices | awk '{print $1}'`

rm -f /dev/$device
mknod /dev/$device c $major 0
chmod 666 /dev/$device