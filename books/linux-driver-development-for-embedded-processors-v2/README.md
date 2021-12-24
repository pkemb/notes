# 嵌入式Linux设备驱动程序开发指南

在树莓派3B平台，基于内核4.x.y版本，学习驱动开发。

## 第01章 构建系统

### 树莓派3B

树莓派3B的开发环境搭建主要由以下步骤构成：
1. 系统安装，SSH/串口登录
2. 编译内核
3. 安装内核
4. 内核模块开发环境

#### 系统安装

树莓派raspbian系统可以从[官方网站](http://downloads.raspberrypi.org/raspbian_lite/images/)下载，推荐下载2018.3.13之后的版本。树莓派安装raspbian系统可以参考博客[树莓派安装raspbian系统](https://blog.csdn.net/qq_34672033/article/details/88389951)。默认登录用户名为pi，该账户默认密码是raspberry。参考博客[树莓派3 B+ 的串口（USART）使用问题](https://www.cnblogs.com/uestc-mm/p/7204429.html)打开串口。

#### 编译内核

由于树莓派3B的系统资源太少，所以选择在PC机交叉编译内核。树莓派启动之后，使用命令`uname -a`查看kernel版本号，克隆对应版本的kernel。下面是内核和工具链的仓库地址。如果GitHub太慢，可以选择国内镜像。

> 2019-06-20的内核版本是4.19.y

```shell
# github
git clone --depth=1 -b rpi-4.19.y https://github.com/raspberrypi/linux linux-rpi
git clone https://github.com/raspberrypi/tools tools-rpi

# 国内镜像
git clone --depth=1 -b rpi-4.19.y https://gitclone.com/github.com/raspberrypi/linux linux-rpi
git clone https://gitclone.com/github.com/raspberrypi/tools tools-rpi
```

编译内核之前，需要安装如下软件包。

```shell
apt-get install bison flex libncurses-dev libssl-dev
```

使用以下命令设置环境变量和编译内核。文件[defconfig](kernel/defconfig)是根据书上说明设置之后的config文件。


```shell
# 工具链的根目录，根据实际情况修改
TOOLDIR=${HOME}/tools-rpi

export PATH=${TOOLDIR}/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:$PATH
export TOOLCHAIN=${TOOLDIR}/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm

make mrproper
KERNEL=kernel7
make ARCH=arm bcm2709_defconfig
make ARCH=arm menuconfig  # 按照书上的说明设置
make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
```

#### 安装内核

使用下面的命令安装内核。如果后续更改了内核或设备树，更新对应的文件即可。

```shell
scp arch/arm/boot/zImage                  root@pi3b.inc:/boot/kernel-rpi-pk.img
scp arch/arm/boot/dts/bcm2710-rpi-3-b.dtb root@pi3b.inc:/boot/bcm2710-rpi-3-b-pk.dtb
scp arch/arm/boot/dts/overlays/*.dtb*     root@pi3b.inc:/boot/overlays/

rm -rf modules_install
mkdir modules_install
make ARCH=arm INSTALL_MOD_PATH=./modules_install modules_install
find ./modules_install -type l -exec rm -rf {} \;
scp -r ./modules_install/*                 root@pi3b.inc:/
```

编辑`/boot/config.txt`，加入如下内容，切换到新版本内核。系统启动成功之后，使用命令`uname -r`查看内核版本是否与内核源代码版本一致。进入内核源代码目录，使用命令`head -n 5 Makefile`可以查看内核源代码的版本。

```shell
dtparam=i2c_arm=on
dtparam=spi=on
dtoverlay=spi0=on

kernel=kernel-rpi-pk.img
device_tree=bcm2710-rpi-3-b-pk.dtb
```

#### 内核模块开发环境

内核模块的源文件不多，故在树莓派部署内核模块的开发环境。首先在PC机打包内核目录，并发送到树莓派。

```shell
tar -czvf linux-rpi-4.19.y.tgz linux-rpi-4.19.y
scp linux-rpi-4.19.y.tgz root@pi3b.inc:/root
```

登录树莓派，解压内核，并且创建软链接。

```shell
tar -xf linux-rpi-4.19.y.tgz
ln -s /root/linux-rpi-4.19.y /lib/modules/`uname -r`/build
ln -s /root/linux-rpi-4.19.y /lib/modules/`uname -r`/source
```

#### vscode remote

略。

