#!/bin/sh

module="pktime"
device="pktime"

insmod ./${module}.ko $* || exit 1
