#!/bin/bash

# kernel源代码根目录
KERNELDIR=${HOME}/linux-rpi-4.19.y
if [ ! -d "$KERNELDIR" ]; then
    echo "$KERNELDIR not exits, please check"
    exit -1
fi

DEFCONFIG=bcm2709_pk_defconfig

. $(dirname $0)/setenv.sh

rm -rf $KERNELDIR/arch/arm/configs/${DEFCONFIG}
cp $(dirname $0)/${DEFCONFIG} $KERNELDIR/arch/arm/configs

cd $KERNELDIR

make mrproper
KERNEL=kernel7
make ARCH=arm ${DEFCONFIG}
make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
