#!/bin/sh

module="pkchr"
device="pkchr"
rmmod $module
rm -f /dev/$device
