# 工具链根目录
TOOLDIR=${HOME}/tools-rpi

if [ ! -d "$TOOLDIR" ]; then
    echo "$TOOLDIR not exits, please check"
    exit -1
fi

export PATH=${TOOLDIR}/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:$PATH
export TOOLCHAIN=${TOOLDIR}/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm