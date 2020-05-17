<h1 id=file_bin>
    工具链及其选项说明
</h1>

介绍常用工具链及其选项。

<h2 id=toc>目录</h2>

1. [objdump](#objdump)
2. [objcopy](#objcopy)
3. [readelf](#readelf)
4. [nm](#nm)
5. [strip](#strip)

<h2 id=objdump>objdump</h2>

<h3 id=objdump-h>-h</h3>

选项全称：--section-headers 或 --headers

说明：显式目标文件各段的摘要信息。

<h3 id=objdump-x>-x</h3>

选项全称：--all-headers

说明：显式所有可用的标题信息，包括符号表和重定位入口。等同于 -a -f -h -p -r -t。

<h3 id=objdump-s>-s</h3>

选项全称：--full-contents

说明：显式所有段的内容，默认非空段都会输出。

输出内容格式：最左一列是偏移量，中间四列是十六进制内容，最右一列是ASCII形式。

<h3 id=objdump-d>-d</h3>

选项全称：--disassemble

说明：反汇编包含指令的段。

<h3 id=objdump-t>-t</h3>

选项全称：--syms

说明：打印符号表。

<h2 id=objcopy>objcopy</h2>

<h3 id=objcopy-I>-I</h3>

参数：-I bfdname

选项全称：--input-target=bfdnme

说明：指定输入文件的格式。

<h3 id=objcopy-O>-O</h3>

参数：-O bfdname

选项全称：--output-target=bfdname

说明：指定输出文件的格式。

<h3 id=objcopy-B>-B</h3>

参数：-B bfdarch

选项全称：--binary-architecture=bfdarch

说明：Useful when transforming a architecture-less input file into an object file.

可以通过目标文件中特定的符号访问数据：
* _binary_objfile_start
* _binary_objfile_end
* _binary_objfile_size
* objfile：文件名

<h2 id=readelf>readelf</h2>

<h3 id=readelf-S>-S</h3>

选项全称：--section-headers 或 -sections

说明：查看ELF文件的段表。

<h2 id=nm>nm</h2>

list symbols from object files

usage: nm [object file]

类似的功能：
1. [readelf -S](#readelf-S)
2. [objdump -t](objdump-t)

<h2 id=strip>strip</h2>

删除ELF文件中的符号。用法：
```shell
strip elf_file
```