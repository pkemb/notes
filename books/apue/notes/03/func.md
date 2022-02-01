<h1 id=file_func>
    第03章函数表
</h1>

[章节目录](../../README.md "返回章节目录")
[笔记](notes.md "返回本章笔记")

---

<table>
    <tr>
        <td>基本操作</td>
        <td>open creat close write read</td>
    </tr>
    <tr>
        <td>原子读写</td>
        <td>pwrite pread</td>
    </tr>
    <tr>
        <td>描述符复制</td>
        <td>dup dup2</td>
    </tr>
    <tr>
        <td>缓冲区同步</td>
        <td>sync fsync fdatasync</td>
    </tr>
    <tr>
        <td>属性控制</td>
        <td>fcntl ioctl</td>
    </tr>
</table>

<table>
    <tr>
        <th>序号</th>
        <th>类别</th>
        <th>函数</th>
        <th>头文件</th>
    </tr>
    <tr>
        <td>1</td>
        <td rowspan="2">open函数</td>
        <td>int open(const char *path, int oflag, ... /* mode_t mode */);</td>
        <td>fcntl.h</td>
    </tr>
    <tr>
        <td>2</td>
        <td>int openat(int fd, const char *path, int oflag, ... /* mode_t mode */);</td>
        <td>fcntl.h</td>
    </tr>
</table>


<h2 id=func_open>int open(const char *path, int oflag, ... /* mode_t mode */);</h2>
<h2 id=func_open>int opent(int fd, const char *path, int oflag, ... /* mode_t mode */);</h2>

```

```

<a href="notes.md#ch_3.2">test</a>

---

[章节目录](../../README.md "返回章节目录")
[笔记](notes.md "返回本章笔记")
