#!/bin/sh

module="pkchr_fifo"
device="pkchr_fifo"

insmod ./${module}.ko $* || exit 1
major=`grep -w $module /proc/devices | awk '{print $1}'`

rm -f /dev/${device}
mknod /dev/${device} c $major 0
chmod 666 /dev/${device}
