[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_load_eflags
	EXTERN	_io_store_eflags
	EXTERN	_memtest_sub
	EXTERN	_load_cr0
	EXTERN	_store_cr0
[FILE "memory.c"]
[SECTION .text]
	GLOBAL	_memtest
_memtest:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	XOR	ESI,ESI
	CALL	_io_load_eflags
	MOV	EBX,EAX
	OR	EBX,262144
	PUSH	EBX
	CALL	_io_store_eflags
	POP	EAX
	TEST	EBX,262144
	JE	L2
	MOV	ESI,1
L2:
	AND	EBX,-262145
	PUSH	EBX
	CALL	_io_store_eflags
	POP	EAX
	MOV	EAX,ESI
	TEST	AL,AL
	JNE	L5
L3:
	PUSH	DWORD [12+EBP]
	PUSH	DWORD [8+EBP]
	CALL	_memtest_sub
	POP	EDX
	MOV	EBX,EAX
	POP	ECX
	MOV	EAX,ESI
	TEST	AL,AL
	JNE	L6
L4:
	LEA	ESP,DWORD [-8+EBP]
	MOV	EAX,EBX
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L6:
	CALL	_load_cr0
	AND	EAX,-6291457
	PUSH	EAX
	CALL	_store_cr0
	POP	EAX
	JMP	L4
L5:
	CALL	_load_cr0
	OR	EAX,6291456
	PUSH	EAX
	CALL	_store_cr0
	POP	EBX
	JMP	L3
	GLOBAL	_memman_init
_memman_init:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	MOV	DWORD [EAX],0
	MOV	DWORD [4+EAX],0
	MOV	DWORD [12+EAX],0
	MOV	DWORD [8+EAX],0
	POP	EBP
	RET
	GLOBAL	_memman_total
_memman_total:
	PUSH	EBP
	XOR	EAX,EAX
	MOV	EBP,ESP
	XOR	EDX,EDX
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	MOV	ECX,DWORD [EBX]
	CMP	EAX,ECX
	JAE	L15
L13:
	ADD	EAX,DWORD [20+EBX+EDX*8]
	INC	EDX
	CMP	EDX,ECX
	JB	L13
L15:
	POP	EBX
	POP	EBP
	RET
	GLOBAL	_memman_alloc
_memman_alloc:
	PUSH	EBP
	XOR	ECX,ECX
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	MOV	ESI,DWORD [12+EBP]
	MOV	EBX,DWORD [8+EBP]
	MOV	EAX,DWORD [EBX]
	CMP	ECX,EAX
	JAE	L30
L28:
	MOV	EDX,DWORD [20+EBX+ECX*8]
	CMP	EDX,ESI
	JAE	L32
	INC	ECX
	CMP	ECX,EAX
	JB	L28
L30:
	XOR	EAX,EAX
L16:
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L32:
	MOV	EDI,DWORD [16+EBX+ECX*8]
	LEA	EAX,DWORD [ESI+EDI*1]
	MOV	DWORD [16+EBX+ECX*8],EAX
	MOV	EAX,EDX
	SUB	EAX,ESI
	MOV	DWORD [20+EBX+ECX*8],EAX
	TEST	EAX,EAX
	JNE	L22
	MOV	EAX,DWORD [EBX]
	DEC	EAX
	MOV	DWORD [EBX],EAX
	CMP	ECX,EAX
	JAE	L22
	MOV	ESI,EAX
L27:
	MOV	EAX,DWORD [24+EBX+ECX*8]
	MOV	EDX,DWORD [28+EBX+ECX*8]
	MOV	DWORD [16+EBX+ECX*8],EAX
	MOV	DWORD [20+EBX+ECX*8],EDX
	INC	ECX
	CMP	ECX,ESI
	JB	L27
L22:
	MOV	EAX,EDI
	JMP	L16
	GLOBAL	_memman_free
_memman_free:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	MOV	ESI,DWORD [8+EBP]
	PUSH	EBX
	XOR	EBX,EBX
	MOV	EDI,DWORD [ESI]
	CMP	EBX,EDI
	JGE	L35
L39:
	MOV	EAX,DWORD [12+EBP]
	CMP	DWORD [16+ESI+EBX*8],EAX
	JA	L35
	INC	EBX
	CMP	EBX,EDI
	JL	L39
L35:
	TEST	EBX,EBX
	JLE	L40
	MOV	EDX,DWORD [12+ESI+EBX*8]
	MOV	EAX,DWORD [8+ESI+EBX*8]
	ADD	EAX,EDX
	CMP	EAX,DWORD [12+EBP]
	JE	L63
L40:
	CMP	EBX,EDI
	JGE	L49
	MOV	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [16+EBP]
	CMP	EAX,DWORD [16+ESI+EBX*8]
	JE	L64
L49:
	CMP	EDI,4095
	JG	L51
	MOV	ECX,EDI
	CMP	EDI,EBX
	JLE	L61
L56:
	MOV	EAX,DWORD [8+ESI+ECX*8]
	MOV	EDX,DWORD [12+ESI+ECX*8]
	MOV	DWORD [16+ESI+ECX*8],EAX
	MOV	DWORD [20+ESI+ECX*8],EDX
	DEC	ECX
	CMP	ECX,EBX
	JG	L56
L61:
	LEA	EAX,DWORD [1+EDI]
	MOV	DWORD [ESI],EAX
	CMP	DWORD [4+ESI],EAX
	JGE	L57
	MOV	DWORD [4+ESI],EAX
L57:
	MOV	EAX,DWORD [12+EBP]
	MOV	DWORD [16+ESI+EBX*8],EAX
	MOV	EAX,DWORD [16+EBP]
	MOV	DWORD [20+ESI+EBX*8],EAX
L62:
	XOR	EAX,EAX
L33:
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	RET
L51:
	MOV	EAX,DWORD [16+EBP]
	INC	DWORD [12+ESI]
	ADD	DWORD [8+ESI],EAX
	OR	EAX,-1
	JMP	L33
L64:
	MOV	EAX,DWORD [12+EBP]
	MOV	DWORD [16+ESI+EBX*8],EAX
	MOV	EAX,DWORD [16+EBP]
	ADD	DWORD [20+ESI+EBX*8],EAX
	JMP	L62
L63:
	ADD	EDX,DWORD [16+EBP]
	MOV	DWORD [12+ESI+EBX*8],EDX
	CMP	EBX,DWORD [ESI]
	JGE	L62
	MOV	EAX,DWORD [12+EBP]
	ADD	EAX,DWORD [16+EBP]
	CMP	EAX,DWORD [16+ESI+EBX*8]
	JE	L65
L43:
	MOV	ECX,DWORD [ESI]
	CMP	EBX,ECX
	JGE	L62
L48:
	MOV	EAX,DWORD [24+ESI+EBX*8]
	MOV	EDX,DWORD [28+ESI+EBX*8]
	MOV	DWORD [16+ESI+EBX*8],EAX
	MOV	DWORD [20+ESI+EBX*8],EDX
	INC	EBX
	CMP	EBX,ECX
	JL	L48
	JMP	L62
L65:
	ADD	EDX,DWORD [20+ESI+EBX*8]
	MOV	DWORD [12+ESI+EBX*8],EDX
	DEC	DWORD [ESI]
	JMP	L43
	GLOBAL	_memman_alloc_4k
_memman_alloc_4k:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [12+EBP]
	ADD	EAX,4095
	AND	EAX,-4096
	MOV	DWORD [12+EBP],EAX
	POP	EBP
	JMP	_memman_alloc
	GLOBAL	_memman_free_4k
_memman_free_4k:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [16+EBP]
	ADD	EAX,4095
	AND	EAX,-4096
	MOV	DWORD [16+EBP],EAX
	POP	EBP
	JMP	_memman_free
