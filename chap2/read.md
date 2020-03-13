# 读取软盘数据

## 读取1个扇区

通过调用BIOS int 13中断来读取软盘数据。

```asm
    MOV AX, 0x0820
    MOV ES, AX
    MOV CH, 0         ; 柱面0
    MOV DH, 0         ; 磁头0
    MOV CL, 2         ; 扇区2

    MOV AH, 0x02      ; AH=0x02 : 读盘
    MOV AL, 1         ; 1个扇区
    MOV BX, 0
    MOV DL, 0x00      ; A驱动器
    INT 0x13          ; 调用BIOS int 13中断
    JC error
```

## 读取10个柱面

```c
for (int i=1;i<=10;i++) {
    for (int j=1;j<=2;j++) {
        for (int k=1;k<=18;k++) {
            read();
        }
    }
}
```
