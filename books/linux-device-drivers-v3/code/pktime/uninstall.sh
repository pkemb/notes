#!/bin/sh

module="pktime"
device="pktime"

rmmod $module
rm -f /dev/${device}
