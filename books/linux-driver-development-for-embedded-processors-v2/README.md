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

由于树莓派3B的系统资源太少，所以选择在PC机交叉编译内核。树莓派启动之后，使用命令`uname -a`查看kernel版本号，克隆对应版本的kernel。下面是内核和工具链的仓库地址。如果GitHub太慢，可以选择GitHub的国内镜像`github.com.cnpmjs.org`。

> 2019-06-20的内核版本是4.19.y

```shell
# github
git clone --depth=1 -b rpi-4.19.y https://github.com/raspberrypi/linux linux-rpi
git clone https://github.com/raspberrypi/tools tools-rpi

# 国内镜像
git clone --depth=1 -b rpi-4.19.y https://github.com.cnpmjs.org/raspberrypi/linux linux-rpi
git clone https://github.com.cnpmjs.org/raspberrypi/tools tools-rpi
```

编译内核之前，需要安装如下软件包。

```shell
apt-get install bison flex libncurses-dev libssl-dev bc
```

使用脚本[build.sh](kernel/build.sh)一键编译内核。执行脚本之前，需要检查`KERNELDIR`和`TOOLDIR`变量的设置是否正确，`TOOLDIR`定义在脚本[setenv.sh](kernel/setenv.sh)。文件[bcm2709_pk_defconfig](kernel/bcm2709_pk_defconfig)是根据书上说明设置之后的defconfig文件，脚本编译时会使用到此文件。

#### 安装内核

使用脚本[deploy.sh](kernel/deploy.sh)安装内核到树莓派。启动脚本之前，需要检查变量`HOST`和`KERNELDIR`的取值，推荐配置好SSH免密登录，不然SCP命令需要输入密码。脚本的使用方法如下：

```shell
Usage:  deploy.sh [kernel|dtb|modules|all] [help]

deploy.sh           deploy kernel
deploy.sh kernel    deploy kernel
deploy.sh dtb       deploy dtb
deploy.sh modules   deploy modules
deploy.sh all       deploy kernel & dtb & modules
deploy.sh help      print this message
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

下载[Eclipse IDE for C/C++ Developers](https://www.eclipse.org/downloads/packages/)，并按照书上的说明设置即可。

> Ubuntu16.04使用Eclipse最新版本会出错，提示GTK版本太低，使用下面的命令更新即可。
> ```shell
> sudo add-apt-repository ppa:gnome3-team/gnome3-staging
> sudo add-apt-repository ppa:gnome3-team/gnome3
> sudo apt update
> sudo apt dist-upgrade
> ```
> 如果apt太慢可以参考使用代理
> ```shell
> sudo apt-get -o Acquire::http::proxy="http://127.0.0.1:8000" upgrade
> ```

考虑到Eclipse太复杂了，所以不推荐使用。直接vscode remote + makefile即可。使用如下Makefile，可以实现`make i`安装，`make u`卸载。

```makefile
deploy:
	scp $(KO_NAME).ko $(USER)@$(HOST):/root

i: deploy
	@echo "install $(KO_NAME).ko"
	@ssh $(USER)@$(HOST) "echo 8 > /proc/sys/kernel/printk"
	@ssh $(USER)@$(HOST) "insmod /root/$(KO_NAME).ko"

u:
	@echo "uninstall $(KO_NAME).ko"
	@ssh $(USER)@$(HOST) "rmmod $(KO_NAME).ko"
```

#### vscode remote

略。

#### 树莓派3B系统设置

* apt切换到国内镜像：https://mirrors.tuna.tsinghua.edu.cn/help/raspbian/

