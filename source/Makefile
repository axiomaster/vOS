OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj fifo.obj
TOOLPATH = ../z_tools/
INCPATH = ../z_tools/haribote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

default :
	$(MAKE) img

# 文件生成规则

ipl.bin : ipl.nas Makefile  # 依赖项
	$(NASK) ipl.nas ipl.bin ipl.lst

asmhead.bin : asmhead.nas Makefile # 依赖项
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin
hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)
#
bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

vOS.sys: asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb vOS.sys

vOS.img : ipl.bin vOS.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 \
		copy from:vOS.sys to:@: \
		imgout:vOS.img

# 一般规则
%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c
%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas
%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# 命令
img :
	$(MAKE) -r vOS.img

run :
	$(MAKE) img
	copy vOS.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu    

install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.img	      

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) *.obj
	-$(DEL) dsctbl.nas
	-$(DEL) graphic.nas
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) vOS.sys

src_only :
	$(MAKE) clean
	-$(DEL) vOS.img    