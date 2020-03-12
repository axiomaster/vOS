    org 0xc400

    mov AL, 0x13 ; 全黑画面
    mov AH, 0x00
    int 0x10

fin:
    hlt
    jmp fin      ; 无限循环