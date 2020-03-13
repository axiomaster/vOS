default :
	make run

boot.bin : boot.asm Makefile
	nasm boot.asm -o boot.bin

a.img : boot.bin Makefile
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
	rm -rf bochsout.txt