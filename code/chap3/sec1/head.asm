
    ORG 0xc400

    ; 关闭中断
    MOV AL, 0xff
    OUT 0x21, AL
    NOP
    OUT 0xA1, AL
    CLI

    ; 设置A20 GATE
    CALL    waitkbdout
    MOV     AL, 0xD1
    OUT     0x64, AL
    CALL    waitkbdout
    MOV     AL, 0xdf
    OUT     0x60, AL
    CALL    waitkbdout

[INSTRSET "i486p"]
    LGDT    [GDTR0]
    MOV     EAX, CR0
    AND     EAX, 0x7fffffff
    OR      EAX, 0x00000001
    MOV     CR0, EAX
    JMP     pipelineflush
pipelineflush:
    MOV     AX, 1*8
    MOV     DS, AX
    MOV     ES, AX
    MOV     FS, AX
    MOV     GS, AX
    MOV     SS, AX

waitkbdout:
    IN  AL, 0x64
    AND AL, 0x20
    JNZ waitkbdout
    RET
