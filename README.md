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

## day9
内存检测

将一块内存读出，取异或，再写入，再读出，做异或。检测是否相同，判断是否有内存错误。

但2次异或操作，会被编译器优化掉，需要使用汇编直接实现。

qemu分配的内存为32MB。

## day12
真是艰难的半天啊！

在图层刷新的时候，将一个坐标由y写成了x，导致画面中一小块刷新一直不正确。但怎么查、怎么比对都找不到原因。

挨个测试文件正确性，结果一着急用notepad把头文件保存了一下，结果就一直报错，从第一个字符开始报错。

最后用二进制格式打开，果然，最前面混入了奇怪的字符，删掉之后，运行正常，继续查bug。

最终让我找到了，哈哈哈，感觉自己好屌，这感觉真爽呐~

可是实验室这会儿大家都在讨论学校论坛上今天发生的大事情——竟然有3个女生在找对象；张全安他老婆张雨绮结婚了。。。。。。

哎，就这样吧。

## day14
现在有GUI, 可以键盘输入，可以鼠标移动，简直太棒了。

马上就要进军多任务了，兴奋啊！

哇咔咔~