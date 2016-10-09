## day1-1
开发环境搭建, 使用windows下qemu虚拟器

## day1-2
汇编按字节写入启动磁盘映像文件
```
DB ; data byte, 往文件写入一个字节
```
## day1-3
使用汇编代码写入img
```
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