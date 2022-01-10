#!/bin/sh

module="pkchr_fifo"
device="pkchr_fifo"

rmmod $module
rm -f /dev/${device}
