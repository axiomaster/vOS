[FORMAT "WCOFF"]				; 生成对象文件的格式
[INSTRSET "i486p"]				; 486兼容指令集
[BITS 32]						; 32位机器语言
[FILE "a_nask.nas"]				; 源文件名

		GLOBAL	_api_putchar

[SECTION .text]

_api_putchar:	; void api_putchar(int c);
		MOV		EDX,1
		MOV		AL,[ESP+4]		; c
		INT		0x40
		RET
