    org 07c00h        ; 告诉编译器程序加载到7c00处
    jmp entry
    DB 0x90

    ; FAT12 磁盘头信息
    DB "VOS_BOOT"     ; OEM String, 必须 8 个字节
    DW 512            ; 每扇区字节数
    DB 1              ; 每簇多少扇区
    DW 1              ; Boot 记录占用多少扇区
    DB 2              ; 共有多少 FAT 表
    DW 224            ; 根目录文件数最大值
    DW 2880           ; 逻辑扇区总数
    DB 0xF0           ; 媒体描述符
    DW 9              ; 每FAT扇区数
    DW 18             ; 每磁道扇区数
    DW 2              ; 磁头数(面数)
    DD 0              ; 隐藏扇区数
    DD 0              ; wTotalSectorCount为0时这个值记录扇区数
    DB 0              ; 中断 13 的驱动器号
    DB 0              ; 未使用
    DB 29h            ; 扩展引导标记 (29h)
    DD 0              ; 卷序列号
    DB 'VOS_BOOT   '  ; 卷标, 必须 11 个字节
    DB 'FAT12   '     ; 文件系统类型, 必须 8个字节

    RESB 18

entry:
    mov ax, cs
    mov ds, ax
    mov es, ax
    call DispStr
    jmp fin

fin:
    HLT
    jmp fin           ; 无限循环

DispStr:
    mov ax, BootMessage
    mov bp, ax        ; ES:BP = 串地址
    mov cx, 16        ; CX = 串长度
    mov ax, 01301h    ; AH = 13,  AL = 01h
    mov bx, 000ch     ; 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)
    mov dl, 0
    int 10h           ; 10h 号中断
    ret
BootMessage:
    db "Hello, OS world!"

times 510-($-$$) db 0 ; 填充剩下的空间，使生成的二进制代码恰好为512字节
dw 0xaa55             ; 结束标志