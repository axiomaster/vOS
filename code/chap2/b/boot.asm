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

read:                 ; 每次读取1个扇区
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
    JNC next
    ; 512字节 x 18个扇区 x 10个柱面 x 2个磁头 = 180KB
next:                 ; 每次向后移动1个扇区，直至18扇区
    MOV AX, ES        ; 每个扇区512字节，对应0x200，所以需要将内存地址向后移动
    ADD AX, 0x0020    ; 这里将ES向后移动0x200
    MOV ES, AX        ; ES寄存器无法直接add，所以通过AX寄存器进行操作
    ADD CL, 1         ; 扇区数+1
    CMP CL, 18        ; 扇区数是否小于18
    JBE read

    MOV CL, 1         ; 读满18扇区后，将扇区计数器置1
    ADD DH, 1         ; 磁头计数器+1
    CMP DH, 2         ; 第2个磁头结束
    JB read

    MOV DH, 0         ; 磁头置0
    ADD CH, 1         ; 柱面+1
    CMP CH, 10        ; 第10个柱面结束
    JB read

fin:
    hlt
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