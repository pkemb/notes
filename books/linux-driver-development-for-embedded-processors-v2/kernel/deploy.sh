#!/bin/bash

HOST=pi3b.inc
# kernel源代码根目录
KERNELDIR=${HOME}/linux-rpi-4.19.y

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

function deploy_kernel()
{
    scp arch/arm/boot/zImage                  root@${HOST}:/boot/kernel-rpi-pk.img
}

function deploy_dtb()
{
    scp arch/arm/boot/dts/bcm2710-rpi-3-b.dtb root@${HOST}:/boot/bcm2710-rpi-3-b-pk.dtb
    scp arch/arm/boot/dts/overlays/*.dtb*     root@${HOST}:/boot/overlays/
}

function deploy_modules()
{
    rm -rf modules_install
    mkdir modules_install
    make ARCH=arm INSTALL_MOD_PATH=./modules_install modules_install
    find ./modules_install -type l -exec rm -rf {} \;
    scp -r ./modules_install/*                 root@${HOST}:/
}

function usage()
{
    echo "Usage:  $0 [kernel|dtb|modules|all] [help]"
    echo ""
    echo "$0           deploy kernel"
    echo "$0 kernel    deploy kernel"
    echo "$0 dtb       deploy dtb"
    echo "$0 modules   deploy modules"
    echo "$0 all       deploy kernel & dtb & modules"
    echo "$0 help      print this message"
}

# check
ping ${HOST} -c 1 > /dev/null 2>&1
if [ $? != "0" ]; then
    echo "${HOST} not online, please check"
    exit 1
fi
checkdir ${KERNELDIR}

deploy=$1
if [ ! "$deploy" ]; then
    deploy="kernel"
fi

cd ${KERNELDIR}
case "$deploy" in
    all)
        deploy_kernel
        deploy_dtb
        deploy_modules
        ;;
    kernel)
        deploy_kernel
        ;;
    dtb)
        deploy_dtb
        ;;
    modules)
        deploy_modules
        ;;
    *|help)
        usage
        exit -1
esac
