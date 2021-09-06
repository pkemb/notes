#!/bin/sh

module="pkchr"
device="pkchr"

if [ ! "$dev_number" ]; then
    dev_number=4
fi

rmmod $module

dev_number=$((${dev_number}-1))
for minor in $(seq 0 $dev_number)
do
    rm -f /dev/${device}${minor}
done
