default :
	make run

boot.bin : boot.asm Makefile
	nasm boot.asm -o boot.bin

vos.sys : sys.asm Makefile
	nasm sys.asm -o vos.sys

a.img : boot.bin vos.sys Makefile
	cp ../../tools/a.img ./
	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc

init :
	cp ../../tools/bochsrc ./

run :
	make init
	make a.img
	bochs

clean :
	rm -rf bochsrc
	rm -rf *.img
	rm -rf *.bin
	rm -rf *.sys
	rm -rf bochsout.txt