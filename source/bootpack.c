/* bootpack偺儊僀儞 */

#include "bootpack.h"
#include <stdio.h>

#define KEYCMD_LED		0xed

void keywin_off(struct SHEET *key_win);
void keywin_on(struct SHEET *key_win);
void close_constask(struct TASK *task);
void close_console(struct SHEET *sht);

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct SHTCTL *shtctl;
	char s[40];
	struct FIFO32 fifo, keycmd;
	int fifobuf[128], keycmd_buf[32], *cons_fifo[2];
	int mx, my, i, new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;
	//int cursor_x, cursor_c;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;

	unsigned char *buf_back, buf_mouse[256], *buf_cons[2]; //多个console
	struct SHEET *sht_back, *sht_mouse, *sht_win; // , *sht_cons[2];
	struct TASK *task_a, *task_cons[2], *task; //
	// struct TIMER *timer;
	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	int key_to = 0, key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	struct CONSOLE *cons;
	int j, x, y, mmx = -1, mmy = -1, mmx2 = 0; //
	struct SHEET *sht = 0, *key_win, *sht2; //

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC偺弶婜壔偑廔傢偭偨偺偱CPU偺妱傝崬傒嬛巭傪夝彍 */
	fifo32_init(&fifo, 128, fifobuf, 0);
	*((int *)0x0fec) = (int)&fifo;
	init_pit();
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8); /* PIT偲PIC1偲僉乕儃乕僪傪嫋壜(11111000) */
	io_out8(PIC1_IMR, 0xef); /* 儅僂僗傪嫋壜(11101111) */
	fifo32_init(&keycmd, 32, keycmd_buf, 0);

	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	*((int *)0x0fe4) = (int)shtctl;

	/* sht_back */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char *)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1); /* 摟柧怓側偟 */
	init_screen8(buf_back, binfo->scrnx, binfo->scrny);

	/* sht_cons */
	key_win = open_console(shtctl, memtotal);
	//sht_cons[0] = open_console(shtctl, memtotal);
	//sht_cons[1] = 0; //未打开状态

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2; /* 夋柺拞墰偵側傞傛偆偵嵗昗寁嶼 */
	my = (binfo->scrny - 28 - 16) / 2;

	sheet_slide(sht_back, 0, 0);
	sheet_slide(key_win, 300, 200); //console 1
//	sheet_slide(sht_cons[1], 8, 4);  //console 2
//	sheet_slide(sht_win, 64, 56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(key_win, 1);
	//	sheet_updown(sht_cons[1], 2);
	//	sheet_updown(sht_win, 3);
	sheet_updown(sht_mouse, 2);
	keywin_on(key_win);

	/* 嵟弶偵僉乕儃乕僪忬懺偲偺怘偄堘偄偑側偄傛偆偵丄愝掕偟偰偍偔偙偲偵偡傞 */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);

	for (;;) {
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* 僉乕儃乕僪僐儞僩儘乕儔偵憲傞僨乕僞偑偁傟偽丄憲傞 */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo32_status(&fifo) == 0) { //
			if (new_mx >= 0) {
				io_sti();
				sheet_slide(sht_mouse, new_mx, new_my);
				new_mx = -1;
			}
			else if (new_wx != 0x7fffffff) {
				io_sti();
				sheet_slide(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			}
			else {
				task_sleep(task_a);
				io_sti();
			}
		}
		else {
			i = fifo32_get(&fifo);
			io_sti();
			if (key_win != 0 && key_win->flags == 0) {    //输入窗口关闭
				if (shtctl->top == 1) {
					key_win = 0;
				}
				else {
					key_win = shtctl->sheets[shtctl->top - 1];
					keywin_on(key_win);
				}
			}
			if (256 <= i && i <= 511) { /* 僉乕儃乕僪僨乕僞 */
				if (i < 0x80 + 256) { /* 僉乕僐乕僪傪暥帤僐乕僪偵曄姺 */
					if (key_shift == 0) {
						s[0] = keytable0[i - 256];
					}
					else {
						s[0] = keytable1[i - 256];
					}
				}
				else {
					s[0] = 0;
				}
				if ('A' <= s[0] && s[0] <= 'Z') {	/* 擖椡暥帤偑傾儖僼傽儀僢僩 */
					if (((key_leds & 4) == 0 && key_shift == 0) ||
						((key_leds & 4) != 0 && key_shift != 0)) {
						s[0] += 0x20;	/* 戝暥帤傪彫暥帤偵曄姺 */
					}
				}
				if (s[0] != 0 && key_win != 0) {     //一般字符 退格 enter
					fifo32_put(&key_win->task->fifo, s[0] + 256);
				}
				if (i == 256 + 0x0f && key_win != 0) {	/* Tab */
					//cursor_c = keywin_off(key_win, sht_win, cursor_c, cursor_x);
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) {
						j = shtctl->top - 1;
					}
					key_win = shtctl->sheets[j];
					//cursor_c = keywin_on(key_win, sht_win, cursor_c);
					keywin_on(key_win);
				}
				if (i == 256 + 0x2a) {	/* 嵍僔僼僩 ON */
					key_shift |= 1;
				}
				if (i == 256 + 0x36) {	/* 塃僔僼僩 ON */
					key_shift |= 2;
				}
				if (i == 256 + 0xaa) {	/* 嵍僔僼僩 OFF */
					key_shift &= ~1;
				}
				if (i == 256 + 0xb6) {	/* 塃僔僼僩 OFF */
					key_shift &= ~2;
				}
				if (i == 256 + 0x3a) {	/* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) {	/* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) {	/* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x3b && key_shift != 0 && key_win != 0) {	// Shift+F1   && task_cons[0]->tss.ss0 != 0  
					//cons = (struct CONSOLE *) *((int *)0x0fec);
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						io_cli();	//强制结束时禁止任务切换
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int)asm_end_app;
						io_sti();
						task_run(task, -1, 0);
					}
				}
				if (i == 256 + 0x3c && key_shift != 0) {	/* Shift+F2 */
					//自动将焦点切入新打开的命令行窗口
					if (key_win != 0) {
						keywin_off(key_win);
					}
					keywin_off(key_win);
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 32, 4);
					sheet_updown(key_win, shtctl->top);
					keywin_on(key_win);
				}
				if (i == 256 + 0x57) { //F11
					sheet_updown(shtctl->sheets[1], shtctl->top - 1);
				}
				if (i == 256 + 0xfa) {	/* 僉乕儃乕僪偑僨乕僞傪柍帠偵庴偗庢偭偨 */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) {	/* 僉乕儃乕僪偑僨乕僞傪柍帠偵庴偗庢傟側偐偭偨 */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			}
			else if (512 <= i && i <= 767) {  //鼠标
				if (mouse_decode(&mdec, i - 512) != 0) {
					/* 儅僂僗僇乕僜儖偺堏摦 */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > binfo->scrnx - 1) {
						mx = binfo->scrnx - 1;
					}
					if (my > binfo->scrny - 1) {
						my = binfo->scrny - 1;
					}
					//sheet_slide(sht_mouse, mx, my);
					new_mx = mx;
					new_my = my;
					if ((mdec.btn & 0x01) != 0) { //移动中按下左键
						if (mmx < 0) {
							for (j = shtctl->top - 1; j > 0; j--) {
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x&&x < sht->bxsize && 0 <= y&&y < sht->bysize) {
									if (sht->buf[y*sht->bxsize + x] != sht->col_inv) {
										sheet_updown(sht, shtctl->top - 1);
										if (sht != key_win) {
											keywin_off(key_win);
											//cursor_c = keywin_off(key_win, sht_win, cursor_c, cursor_x);
											key_win = sht;
											//cursor_c = keywin_on(key_win, sht_win, cursor_c);
											keywin_on(key_win);
										}
										if (3 <= x&&x < sht->bxsize - 3 && 3 <= y&&y < 21) {
											mmx = mx;
											mmy = my;
											mmx2 = sht->vx0;
											new_wy = sht->vy0;
										}
										if (sht->bxsize - 21 <= x&&x < sht->bxsize - 5 && 5 <= y&&y < 19) { //点击位置 x
											if ((sht->flags & 0x10) != 0) {
												//cons = (struct CONSOLE *)*((int *)0x0fec);
												task = sht->task;
												cons_putstr0(task->cons, "\nBreak(mouse) :\n");
												io_cli();
												task->tss.eax = (int) &(task->tss.esp0);
												task->tss.eip = (int)asm_end_app;
												io_sti();
												task_run(task, -1, 0);
											}
											else {
												task = sht->task;
												sheet_updown(sht, -1);
												keywin_off(key_win);
												key_win = shtctl->sheets[shtctl->top - 1];
												keywin_on(key_win);
												io_cli();
												fifo32_put(&task->fifo, 4);
												io_sti();
											}
										}
										break;
									}
								}
							}
						}
						else {
							x = mx - mmx;
							y = my - mmy;
							new_wx = (mmx2 + x + 2)&~3;
							new_wy = new_wy + y;
							//sheet_slide(sht, (mmx2 + x + 2)&~3, sht->vy0 + y);
							//sheet_slide(sht, sht->vx0 + x, sht->vy0 + y);
							//mmx = mx;
							mmy = my;
						}
					}
					else {
						mmx = -1;
						if (new_wx != 0x7fffffff) {
							sheet_slide(sht, new_wx, new_wy);	//固定图层位置
							new_wx = 0x7fffffff;
						}
					}
				}
			}
			else if (768 <= i&&i <= 1023) {
				close_console(shtctl->sheets0 + (i - 768));
			}
			else if (1024 <= i&&i <= 2023) {
				close_constask(taskctl->tasks0 + (i - 1024));
			}
			else if (2024 <= i && i <= 2279) {	//只关闭命令行窗口
				sht2 = shtctl->sheets0 + (i - 2024);
				memman_free_4k(memman, (int)sht2->buf, 256 * 165);
				sheet_free(sht2);
			}
		}
	}
}

void keywin_off(struct SHEET *key_win)
{
	change_wtitle8(key_win, 0);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 3); /* console 窗口光标off */
	}
	return;
}

void keywin_on(struct SHEET *key_win)
{
	change_wtitle8(key_win, 1);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 2); //console 窗口光标on
	}
	return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	int *cons_fifo = (int *)memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int)&console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *)(task->tss.esp + 4)) = (int)sht;
	*((int *)(task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2); /* level=2, priority=2 */
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	unsigned char *buf = (unsigned char *)memman_alloc_4k(memman, 256 * 165);
	sheet_setbuf(sht, buf, 256, 165, -1); /* 透明色なし */
	make_window8(buf, 256, 165, "console", 0);
	make_textbox8(sht, 8, 28, 240, 128, COL8_000000);
	sht->task = open_constask(sht, memtotal);
	sht->flags |= 0x20;	/* カーソルあり */
	return sht;
}

void close_constask(struct TASK *task)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int)task->fifo.buf, 128 * 4);
	task->flags = 0;
	return;
}
void close_console(struct SHEET *sht)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int)sht->buf, 256 * 165);
	sheet_free(sht);
	close_constask(task);
	return;
}