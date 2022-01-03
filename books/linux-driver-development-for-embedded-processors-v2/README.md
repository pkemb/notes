# 嵌入式Linux设备驱动程序开发指南

在树莓派3B平台，基于内核4.x.y版本，学习驱动开发。
* [豆瓣](https://book.douban.com/subject/35514232/)
* [github repo](https://github.com/ALIBERA/linux_book_2nd_edition)

## demo代码说明

学习过程中写的demo代码存放在[code](./code/)目录中。下面是关键要点的说明。

* [hello_world](code/hello_world/)
  * 模块的初始化与退出。
  * MODULE_AUTHOR()、MODULE_LICENSE()等宏的使用。
* [pkchr](code/pkchr/)
  * 字符设备
  * 设备号的申请与注册
  * 字符设备的初始化与注册
  * `file_operations`结构体常用函数及其语义
    * open()函数中`filp->private_data`成员的使用
  * 使用udev自动创建设备节点
* [miscchr](code/miscchr/)
  * 在主设备号10下快速创建字符设备
  * `struct miscdevice`结构体，name、minor、fops成员
  * misc_register()函数
* [platformchr](code/platformchr/)
  * 使用platform驱动创建字符设备
  * platform_driver_register()函数与`struct platform_driver`结构体
  * `struct of_device_id`结构体的`compatible`字段
  * 使用probe()和remove()函数代替init()和exit()函数
  * devm_kzalloc()，设备移除后自动free
  * platform_set_drvdata() / platform_get_drvdata()
* [led-gpiod](code/led-gpiod/)
  * 使用GPIOD控制pin脚的电平
  * DTS文件的编写，xxxx-gpios属性。
  * devm_gpiod_get() 函数各个参数的含义
  * of_property_read_string() 获取指定属性的值
  * gpiod_get_value() 获取电平
  * gpiod_set_value() 设置电平
* [led-pinctrl](code/led-pinctrl/)
  * 使用pinctrl控制GPIO的状态
  * DTS文件的编写，GPIO控制器的引脚状态节点，设备驱动结点的pinctrl-name和pinctrl-n节点
  * ioremap
  * iowrite / ioread

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


## 第02章 Linux设备与驱动模型

### 驱动模型

[第十四章 Linux设备模型](../linux-device-drivers-v3/README.md#第十四章-linux设备模型)

### 设备树

* [设备树标准](https://github.com/devicetree-org/devicetree-specification/releases)
* [kernel文档](https://www.kernel.org/doc/html/latest/devicetree/)

## 第03章 最简驱动程序

内核模块、模块参数、模块初始化与退出、Makefile等内容，略。

## 第04章 字符设备驱动

* cdev_init
  * struct cdev
  * file_operations结构体
    * kernel space和user space交换数据
    * struct inode：表示一个唯一的文件
    * struct file：表示一个打开的文件
    * 各个成员函数的语义
* cdev_add
  * 设备号，dev_t
    * 动态申请
    * 静态申请
* 模块与内核一起构建
  * Kconfig
  * Makefile
* 设备节点
  * mknod
  * udev
* misc字符设备
  * struct miscdevice
    * name：会出现在/proc/misc，并且会自动创建设备文件/dev/name
    * minor：指定的子设备号。如果设置为MISC_DYNAMIC_MINOR则表示动态申请
    * fops
  * misc_register()
  * misc_deregister()

## 第05章 平台设备驱动

平台设备用于枚举不可探测的设备。
* struct platform_driver
  * probe / remove
* struct of_device_id

### pinctrl子系统

**学习参考**

* https://www.kernel.org/doc/html/latest/driver-api/pin-control.html
* http://www.wowotech.net/gpio_subsystem/pin-control-subsystem.html

**注册引脚**

* pinctrl子系统核心需要SoC实现
  * pinctrl_desc
    * pinctrl_ops
    * pinmux_ops
    * pinconf_ops
  * gpio_chip
  * irq_chip
* dts提供引脚配置节点

设备驱动从pinctrl子系统核心请求引脚复用

**dts编写**

首先需要在GPIO控制器节点定义引脚状态。以树莓派3B为例。`brcm,pins`等属性是由`pinctrl-bcm2835.c`定义。

```dts
&gpio {
    led_pins: led_pins {
        brcm,pins = <27 22 26>;
        brcm,function = <BCM2835_FSEL_GPIO_OUT>;
        brcm,pull = <BCM2835_PUD_UP BCM2835_PUD_UP BCM2835_PUD_UP>;
    };
};
```

设备驱动节点引用GPIO控制器定义的引脚状态。标准属性`pinctrl-names`定义了引脚状态的名字，标准属性`pinctrl-n`引用引脚状态。

```dts
&soc {
    ledred {
        compatible = "pk,RGBleds";
        label = "ledred";
        pinctrl-names = "default";
        pinctrl-0 = <&led_pins>;
        pinctrl-1 = <&xxxxxxxx>
        pins = <27>;
    };
};
```

**设备驱动常用接口**

```c
// 获取 pinctrl
struct pinctrl *devm_pinctrl_get(struct device *dev);
// 查找状态
struct pinctrl_state *pinctrl_lookup_state(struct pinctrl *p, char *name);
// 切换到指定状态
int pinctrl_select_state(struct pinctrl *p, struct pinctrl_state *state);
```

### gpiod接口

**获取和释放gpio描述符**

```c
// 通过 IS_ERR() 检查返回值是否出错
struct gpio_desc *devm_gpiod_get(
    struct device *dev, const char *con_id, enum gpiod_flags flags);
struct gpio_desc *devm_gpiod_get_index(
    struct device *dev, const char *con_id, unsigned int idx, enum gpiod_flags flags);
void devm_gpiod_put(struct device *dev, struct gpio_desc *desc);
```

参数说明如下：
* dev：可以从probe()函数的参数获取device结构体
* con_id：设备树中GPIO映射定义的属性名。例如属性是`led-gpios=<&gpio 26 GPIO_ACTIVE_HIGH>;`，则`con_id="led"`
* idx：同一个属性可以包含多个GPIO映射定义，通过idx来区分
* flags：可以取如下值
  * GPIOD_ASIS或0：不初始化GPIO，后续需要使用API设置GPIO的方向。
  * GPIOD_IN：GPIO设置为输入
  * GPIOD_OUT_LOW：GPIO设置为输出，初值设置为0
  * GPIOD_OUT_HIGHT：GPIO设置为输入，初值设置为1

**GPIO方向**

```c
int gpiod_get_direction(const struct gpio_desc *desc);
int gpiod_direction_input(struct gpio_desc *desc);
int gpiod_direction_output(struct gpio_desc *desc, int value);
```

**设置GPIO的值**

```c
int gpiod_get_value(const struct gpio_desc *desc);
int gpiod_set_value(const struct gpio_desc *desc, int value);
```

gpiod使用逻辑值，`value=1`表示值有效，`value=0`表示值无效。下表列出了逻辑值和pin脚电平的关系。

| value | 电平有效属性 | pin脚电平 |
| - | - | - |
| 0 | 高电平有效 | 低 |
| 1 | 高电平有效 | 高 |
| 0 | 低电平有效 | 高 |
| 1 | 低电平有效 | 低 |

**gpio映射到中断**

`int gpiod_to_irq(const struct gpio_desc *desc)`获取GPIO对应的IRQ号，返回值可以传递到`request_irq()`或`free_irq()`。如果无法完成映射，则`gpiod_to_irq()`返回一个负的错误码。此函数不会阻塞。

**GPIO设备树**

示例如下。属性`led-gpios`定义了gpio映射，可以用逗号分隔映射多个gpio。对于每一个映射，`&gpio`表示gpio控制器，SoC厂家会在dts中给出定义。`27`表示gpio号，`GPIO_ACTIVE_HIGH`表示高电平有效。

```dts
ledred {
    compatible = "pk,RGBleds";
    label = "ledred";
    led-gpios = <&gpio 27 GPIO_ACTIVE_HIGH>;
};
```

### io端口

ARM处理器主存和IO设备使用相同的地址空间，这意味着可以使用常规指令访问IO设备。但是设备驱动无法直接访问物理地址，需要重新映射。

**io端口重映射**

下面是端口重映射和解除重映射的API。推荐使用`devm_`开头的API，因为设备模型会处理好资源的释放。

```c
void __iomem *ioremap(phys_addr_t offset, size_t size);
void iounmap(void *address);

void __iomem *devm_ioremap(struct device *dev, resource_size_t offset, resource_size_t size);
void devm_iounmap(struct device *dev, void __iomem *addr);
```

**读写io端口**

```c
u8 ioread8(const volatile void __iomem *addr);
u16 ioread16(const volatile void __iomem *addr);
u32 ioread32(const volatile void __iomem *addr);

void iowrite8(u8 value, volatile void __iomem *addr);
void iowrite16(u16 value, volatile void __iomem *addr);
void iowrite32(u32 value, volatile void __iomem *addr);
```
