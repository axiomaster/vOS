[INSTRSET "i486p"]
[BITS 32]
        MOV EAX, 1*8
        MOV DS, AX
        MOV BYTE [0x102600], 0
        RETF