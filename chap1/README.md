# 启动

## 编译

```bash
nasm boot.asm -o boot.bin
```

## 创建软盘

```bash
bximage
```

## 写入软盘

```bash
dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
```

## qemu启动

```bash
qemu -fda a.img
```
