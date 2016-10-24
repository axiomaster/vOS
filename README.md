# A simple OS
《30天自制操作系统》读书笔记

## day1-1
开发环境搭建, 使用windows下qemu虚拟器

## day1-2
汇编按字节写入启动磁盘映像文件
``` asm
DB ; data byte, 往文件写入一个字节
```
> 0x7c00 - 0x7dff ; 启动区内容的装载地址
> 
> INT 10h ; bios显示中断

## day1-3
使用汇编代码写入img
``` asm
DB ; 可以写入字符串
DW ; data word
DD ; data double-word
```

## day2
使用asm制作引导扇区。

使用Makefile合并脚本运行

## day4
使用C语言开发

``` c
int i;
char *p = 0xa0000;
for(i=0;i<0xffff;i++){
    p[i] = i&0x0f; // 此处 p[i] 与 *(p+i) 相同，c语言不检查数组越界
}
// p[i], *(p+i), *(i+p), i[p] 完全等效
```

## day5
字体文件：hankaku.txt -> hankaku.bin

makefont.exe: 将txt读入，写成bin文件 16*256=4096字节， 每个字符宽度为8，高度为16，占16byte

## day8
操作系统内容分布图
> 0x00000000 - 0x000fffff : 启动中多次使用，之后变空 （1MB）
>
> 0x00100000 - 0x00267fff : 用于保存软盘内容（1440KB）
> 
> 0x00268000 - 0x0026f7ff : 空 （30KB）
>
> 0x0026f800 - 0x0026ffff : IDT (2KB)
> 
> 0x00270000 - 0x0027ffff : GDT (64KB)
>
> 0x00280000 - 0x002fffff : bootpack.hrb (512KB)
>
> 0x00300000 - 0x003fffff : 栈及其他(1MB)
>
> 0x00400000 -            : 空