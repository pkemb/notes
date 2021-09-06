#!/bin/sh

module="pkchr"
device="pkchr"

insmod ./${module}.ko $* || exit 1
major=`grep $module /proc/devices | awk '{print $1}'`

if [ ! "$dev_number" ]; then
    dev_number=4
fi

dev_number=$((${dev_number}-1))
for minor in $(seq 0 $dev_number)
do
    rm -f /dev/${device}${minor}
    mknod /dev/${device}${minor} c $major $minor
    chmod 666 /dev/${device}${minor}
done

