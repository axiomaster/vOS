//
// Created by lism on 2016/10/24.
//
#include "bootpack.h"

struct FIFO32 *keyfifo;
int keydata0; //偏移量

void inthandler21(int *esp) //对应键盘中断
{
	int data;
	io_out8(PIC0_OCW2, 0x61); //PIC监视IRQ1中断
	data = io_in8(PORT_KEYDAT);
	fifo32_put(&keyfifo, data + keydata0);
	return;
}

#define PORT_KEYSTA             0x0064
#define KEYSTA_SEND_NOTREADY    0x02
#define KEYCMD_WRITE_MODE       0x60
#define KBC_MODE                0x47

void wait_KBC_sendready(void)
{
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
}

void init_keyboard(struct FIFO32 *fifo, int data0)
{
	keyfifo = fifo;
	keydata0 = data0;

	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
}

