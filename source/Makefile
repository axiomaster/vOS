TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/haribote/

MAKE     = $(TOOLPATH)make.exe -r
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# デフォルト動作

default :
	$(MAKE) vOS.img

# ファイル生成規則

vOS.img : vOS/ipl10.bin vOS/vOS.sys Makefile \
		a/a.hrb hello/hello4.hrb \
		winhelo/winhelo.hrb \
		star/stars.hrb \
		lines/lines.hrb walk/walk.hrb noodle/noodle.hrb \
		beep/beepdown.hrb color/color2.hrb sosu/sosu.hrb typeipl/typeipl.hrb
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:vOS/ipl10.bin len:512 from:0 to:0 \
		copy from:vOS/vOS.sys to:@: \
		copy from:vOS/ipl10.nas to:@: \
		copy from:make.bat to:@: \
		copy from:a/a.hrb to:@: \
		copy from:hello/hello4.hrb to:@: \
		copy from:winhelo/winhelo.hrb to:@: \
		copy from:star/stars.hrb to:@: \
		copy from:lines/lines.hrb to:@: \
		copy from:walk/walk.hrb to:@: \
		copy from:noodle/noodle.hrb to:@: \
		copy from:beep/beepdown.hrb to:@: \
		copy from:color/color2.hrb to:@: \
		copy from:sosu/sosu.hrb to:@: \
		copy from:typeipl/typeipl.hrb to:@: \
		imgout:vOS.img

# コマンド

run :
	$(MAKE) vOS.img
	$(COPY) vOS.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) vOS.img
	$(IMGTOL) w a: vOS.img

full :
	$(MAKE) -C vOS
	$(MAKE) -C apilib
	$(MAKE) -C a
	$(MAKE) -C hello4
	$(MAKE) -C winhelo
	$(MAKE) -C stars
	$(MAKE) -C lines
	$(MAKE) -C walk
	$(MAKE) -C noodle
	$(MAKE) -C beepdown
	$(MAKE) -C color2
	$(MAKE) vOS.img

run_full :
	$(MAKE) full
	$(COPY) vOS.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install_full :
	$(MAKE) full
	$(IMGTOL) w a: vOS.img

run_os :
	$(MAKE) -C vOS
	$(MAKE) run

clean :
# 何もしない

src_only :
	$(MAKE) clean
	-$(DEL) vOS.img

clean_full :
	$(MAKE) -C vOS		clean
	$(MAKE) -C apilib		clean
	$(MAKE) -C a			clean
	$(MAKE) -C hello4		clean
	$(MAKE) -C winhelo		clean
	$(MAKE) -C stars		clean
	$(MAKE) -C lines		clean
	$(MAKE) -C walk			clean
	$(MAKE) -C noodle		clean
	$(MAKE) -C beepdown		clean
	$(MAKE) -C color2		clean

src_only_full :
	$(MAKE) -C vOS		src_only
	$(MAKE) -C apilib		src_only
	$(MAKE) -C a			src_only
	$(MAKE) -C hello4		src_only
	$(MAKE) -C winhelo		src_only
	$(MAKE) -C stars		src_only
	$(MAKE) -C lines		src_only
	$(MAKE) -C walk			src_only
	$(MAKE) -C noodle		src_only
	$(MAKE) -C beepdown		src_only
	$(MAKE) -C color2		src_only
	-$(DEL) vOS.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) vOS.img
