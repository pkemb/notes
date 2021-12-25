#!/bin/bash

function checkdir()
{
    if [ ! "$1" ]; then
        return
    fi
    if [ ! -d "$1" ]; then
        echo "directory not exits, please check"
        exit -1
    fi
}

# kernel源代码根目录
KERNELDIR=${HOME}/linux-rpi-4.19.y
# 工具链根目录
TOOLDIR=${HOME}/tools-rpi

checkdir ${KERNELDIR}
checkdir ${TOOLDIR}

DEFCONFIG=bcm2709_pk_defconfig

export PATH=${TOOLDIR}/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:$PATH
export TOOLCHAIN=${TOOLDIR}/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm

rm -rf $KERNELDIR/arch/arm/configs/${DEFCONFIG}
cp $(dirname $0)/${DEFCONFIG} $KERNELDIR/arch/arm/configs

cd $KERNELDIR

make mrproper
KERNEL=kernel7
make ARCH=arm ${DEFCONFIG}
make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
