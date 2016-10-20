; hello-os
; TAB=4
CYLS	EQU		10				;宏定义
		ORG		0x7c00			;指明程序装载地址

; 标准FAT12格式软盘专用代码
		JMP		entry
		DB		0x90
		DB		"HELLOIPL"		; 启动区名称，8字节
		DW		512				; 1个扇区大小
		DB		1				; 簇大小
		DW		1				; FAT起始位置
		DB		2				; FAT的个数
		DW		224				; 根目录大小
		DW		2880			; 磁盘大小
		DB		0xf0			; 磁盘种类
		DW		9				; FAT长度
		DW		18				; 1个磁道有9个扇区
		DW		2				; 磁头数
		DD		0				; 不使用分区
		DD		2880			; 重写一次磁盘大小
		DB		0,0,0x29		; 意义不明，固定代码
		DD		0xffffffff		; 卷标号码
		DB		"HELLO-OS   "	; 磁盘名称 11字节
		DB		"FAT12   "		; 磁盘格式名称 8字节
		RESB	18				; 空出18字节

; 程序本体
entry:							; 0x7c50
		MOV		AX,0
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; 加载程序
		MOV		AX, 0x0820
		MOV		ES, AX			; 段寄存器
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
readloop:
		MOV		SI,0			; 记录失败次数的寄存器
retry:
		MOV		AH,0x02			; 读盘
		MOV		AL,1			; 1个扇区
		MOV		BX,0			
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用磁盘BIOS
		JNC		next
		ADD		SI,1
		CMP		SI,5
		JAE		error			; SI>=5, 跳转到error
		MOV		AH,0x00
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 重置驱动器, 磁盘bios
		JMP		retry
next:
		MOV		AX,ES
		ADD		AX,0x0020		; ES增加512/16
		MOV		ES,AX
		ADD		CL,1
		CMP		CL,18			; CL小于18，一直读，读到第18个扇区
		JBE		readloop
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop

		MOV		[0x0ff0],CH
		JMP		0xc200			; 跳转到程序位置开始执行	磁盘4200H处，对应内存 0x8000+0x4200 = 0xc200处

error:
		MOV		SI,msg			;

; 信息显示部分

putloop:
		MOV		AL,[SI]			; [] 内存地址
		ADD		SI,1
		CMP		AL,0
		JE		fin				; 条件跳转指令
		MOV		AH,0x0e			; bios显示配置
		MOV		BX,15
		INT		0x10
		JMP		putloop

fin:
		HLT						; 让CPU停止，等待指令
		JMP		fin				; 无限循环			

msg:							; 0x7c74
		DB		0x0a, 0x0a		; 改行2个
		DB		"load, error"
		DB		0x0a			; 改行
		DB		0

		RESB	0x7dfe-$			; 直到0x001fe填0x00命令

		DB		0x55, 0xaa

; 启动区以外部分的输出

;		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
;		RESB	4600
;		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
;		RESB	1469432