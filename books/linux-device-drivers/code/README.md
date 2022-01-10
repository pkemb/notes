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