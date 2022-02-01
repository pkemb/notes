
# 第03章 文件I/O

[函数表](func.md "本章函数表")

---

## 文件描述符

```c
内核通过文件描述符引用文件。
文件描述符是一个非负整数，介于 0 ~ OPEN_MAX-1 之间。

标准输入：0   STDIN_FILENO
标准输出：1   STDOUT_FILENO
标准错误：2   STDERR_FILENO
注：建议使用宏而不是常数。
```

## 函数open和openat

```c
int open(const char *path, int oflag, ... /* mode_t mode */);
int openat(int fd, const char *path, int oflag, ... /* mode_t mode */);

头文件：fcntl.h
功能：打开或创建一个文件
返回值：
    若成功，返回最小的，没有使用的文件描述符；
    若出错，返回-1。
形参说明：
    path：打开或创建文件的名字
    oflag：说明此函数的多个选项，
           用一个或多个常量进行位或运算构成，具体见下表。
    mode：最后一个参数，可选参数，仅创建文件时才使用，指定创建文件的权限。
    fd：把openat函数和open函数区分开，有三种可能：
        (1) path是绝对路径，openat 等同于 open。
        (2) path是相对路径，开始地址是 fd 指向的目录。
        (3) path是相对路径，fd 参数具有特殊值 AT_FDCWD，开始地址是 当前工作目录。
示例代码：
char path[] = "test_file.txt";
```

<table>
    <tr>
        <td>打开文件的用途</td>
        <td>O_RDONLY O_WRONLY O_RDWR O_EXEC O_SEARCH</td>
    </tr>
    <tr>
        <td>常用标志</td>
        <td>O_CREAT O_EXCL O_CLOEXEC O_DIRECTORY</td>
    </tr>
    <tr>
        <td>针对特殊文件</td>
        <td>O_NOFOLLOE O_NONBLOCK O_NOCTTY O_TTY_INIT</td>
    </tr>
    <tr>
        <td>文件偏移量</td>
        <td>O_APPEND O_TRUNC</td>
    </tr>
    <tr>
        <td>同步选项</td>
        <td>O_SYNC O_DSYNC O_RSYNC</td>
    </tr>
</table>

<table>
    <tr>
        <th>序号</th>
        <th>flag</th>
        <th>说明</th>
        <th>备注</th>
    </tr>
    <tr>
        <td>1</td>
        <td>O_RDONLY</td>
        <td>只读打开</td>
        <td rowspan="5">互斥，只能指定一个</td>
    </tr>
    <tr>
        <td>2</td>
        <td>O_WRONLY</td>
        <td>只写打开</td>
    </tr>
    <tr>
        <td>3</td>
        <td>O_RDWR</td>
        <td>读、写打开</td>
    </tr>
    <tr>
        <td>4</td>
        <td>O_EXEC</td>
        <td>只执行打开</td>
    </tr>
    <tr>
        <td>5</td>
        <td>O_SEARCH</td>
        <td>只搜索打开，应用于目录，支持的系统较少。</td>
    </tr>
    <tr>
        <td>6</td>
        <td>O_APPEND</td>
        <td>每次写时都追加到文件的尾端。</td>
        <td></td>
    </tr>
    <tr>
        <td>7</td>
        <td>O_CLOEXEC</td>
        <td>把FD_CLOEXEC常量设置为文件描述符标志。</td>
        <td>执行时关闭。</td>
    </tr>
    <tr>
        <td>8</td>
        <td>O_CREAT</td>
        <td>若此文件不存在则创建它。</td>
        <td>使用此选项时，需要指明 open/openat 函数的最后一个参数。</td>
    </tr>
    <tr>
        <td>9</td>
        <td>O_DIRECTORY</td>
        <td>如果path引用的不是目录，则出错。</td>
        <td></td>
    </tr>
    <tr>
        <td>10</td>
        <td>O_EXCL</td>
        <td>如果同时指定了O_CREAT，而文件存在，则出错。</td>
        <td>用此可以测试一个文件是否存在，如果不存在则创建。并且测试和创建是一个原子操作。</td>
    </tr>
    <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
    </tr>
</table>

<h2 id=ch_3.4>
    函数create
</h2>

```c
int creat(const char *path, mode_t mode)

头文件：fcntl.h
功能：创建一个新文件。
返回值：
    若成功，返回为只写打开的文件描述符。
    若出错，返回-1。
形参说明：
    path：创建文件的名字。
    mode：新文件的访问权限位。
示例代码：
    open函数可以替代creat函数。
    creat(path, mode); <==> open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);

    以读写方式创建一个文件：
    open(path, O_RDWR | O_CREAT | O_TRUNC, mode);
```

<h2 id=ch_3.5>
    函数close
</h2>

```c
int close(int fd);

头文件：unistd.h
功能：
    关闭一个打开的文件。

返回值：若成功，返回0；若失败，返回-1。
形参说明：
    fd：文件描述符。
注：
1、当一个进程终止时，将 自动 关闭它所有的打开文件。
   可以不显式调用此函数关闭。
2、关闭一个文件时，还将释放该进程加在该文件上的所有记录锁。
示例代码：
    close(STDOUT_FILENO);
```

<h2 id=ch_3.6>
    函数lseek
</h2>

```c
头文件：
功能：
返回值：
形参说明：
示例代码：
```

<h2 id=ch_3.7>
    函数read
</h2>

```c
头文件：
功能：
返回值：
形参说明：
示例代码：
```

<h2 id=ch_3.8>
    函数write
</h2>

```c
头文件：
功能：
返回值：
形参说明：
示例代码：
```

<h2 id=ch_3.9>
    I/O的效率
</h2>



<h2 id=ch_3.10>
    文件I/O的数据结构
</h2>

<h2 id=ch_3.11>
    原子操作
</h2>

<h2 id=ch_3.12>
    函数dup和dup2
</h2>

```c
头文件：
功能：
返回值：
形参说明：
示例代码：
```


---

[章节目录](../../README.md#title_ch03 "返回章节目录")
[函数表](func.md "本章函数表")
