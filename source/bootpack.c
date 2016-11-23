/* bootpack偺儊僀儞 */

#include "bootpack.h"
#include <stdio.h>

#define KEYCMD_LED		0xed

void keywin_off(struct SHEET *key_win);
void keywin_on(struct SHEET *key_win);

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct SHTCTL *shtctl;
	char s[40];
	struct FIFO32 fifo, keycmd;
	int fifobuf[128], keycmd_buf[32], *cons_fifo[2];
	int mx, my, i;
	//int cursor_x, cursor_c;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;

	unsigned char *buf_back, buf_mouse[256], *buf_cons[2]; //多个console
	struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_cons[2];
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
	struct SHEET *sht = 0, *key_win;

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC偺弶婜壔偑廔傢偭偨偺偱CPU偺妱傝崬傒嬛巭傪夝彍 */
	fifo32_init(&fifo, 128, fifobuf, 0);
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
	for (i = 0; i < 2; i++) {
		sht_cons[i] = sheet_alloc(shtctl);
		buf_cons[i] = (unsigned char *)memman_alloc_4k(memman, 256 * 165);
		sheet_setbuf(sht_cons[i], buf_cons[i], 256, 165, -1); //
		make_window8(buf_cons[i], 256, 165, "console", 0);
		make_textbox8(sht_cons[i], 8, 28, 240, 128, COL8_000000);
		task_cons[i] = task_alloc();
		task_cons[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
		task_cons[i]->tss.eip = (int)&console_task;
		task_cons[i]->tss.es = 1 * 8;
		task_cons[i]->tss.cs = 2 * 8;
		task_cons[i]->tss.ss = 1 * 8;
		task_cons[i]->tss.ds = 1 * 8;
		task_cons[i]->tss.fs = 1 * 8;
		task_cons[i]->tss.gs = 1 * 8;
		*((int *)(task_cons[i]->tss.esp + 4)) = (int)sht_cons[i];
		*((int *)(task_cons[i]->tss.esp + 8)) = memtotal;
		task_run(task_cons[i], 2, 2); /* level=2, priority=2 */
		sht_cons[i]->task = task_cons[i];
		sht_cons[i]->flags |= 0x20; //
		cons_fifo[i] = (int *)memman_alloc_4k(memman, 128 * 4);
		fifo32_init(&task_cons[i]->fifo, 128, cons_fifo[i], task_cons[i]);
	}


	/* sht_win */
	//sht_win = sheet_alloc(shtctl);
	//buf_win = (unsigned char *)memman_alloc_4k(memman, 160 * 52);
	//sheet_setbuf(sht_win, buf_win, 144, 52, -1); /* 摟柧怓側偟 */
	//make_window8(buf_win, 144, 52, "task_a", 1);
	//make_textbox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
	//cursor_x = 8;
	//cursor_c = COL8_FFFFFF;
	//timer = timer_alloc();
	//timer_init(timer, &fifo, 1);
	//timer_settime(timer, 50);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2; /* 夋柺拞墰偵側傞傛偆偵嵗昗寁嶼 */
	my = (binfo->scrny - 28 - 16) / 2;

	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_cons[0], 300, 200); //console 1
	sheet_slide(sht_cons[1], 8, 4);  //console 2
//	sheet_slide(sht_win, 64, 56);
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(sht_cons[0], 1);
	sheet_updown(sht_cons[1], 2);
//	sheet_updown(sht_win, 3);
	sheet_updown(sht_mouse, 3);
	key_win = sht_cons[0];
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
		if (fifo32_status(&fifo) == 0) {
			task_sleep(task_a);
			io_sti();
		}
		else {
			i = fifo32_get(&fifo);
			io_sti();
			if (key_win->flags == 0) {    //输入窗口关闭
				key_win = shtctl->sheets[shtctl->top - 1];
				//cursor_c = keywin_on(key_win, sht_win, cursor_c);
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
				if (s[0] != 0) {     //一般字符 退格 enter
					fifo32_put(&key_win->task->fifo, s[0] + 256);
					//if (key_win == sht_win) {	//发送至任务A
					//	if (cursor_x < 128) {
					//		/* 堦暥帤昞帵偟偰偐傜丄僇乕僜儖傪1偮恑傔傞 */
					//		s[1] = 0;
					//		putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
					//		cursor_x += 8;
					//	}
					//}
					//else {	/* 僐儞僜乕儖傊 */
					//	fifo32_put(&key_win->task->fifo, s[0] + 256);
					//}
				//}
				//if (i == 256 + 0x57 && i <= 511) { //F11
				//	sheet_updown(shtctl->sheets[1], shtctl->top - 1);
				//}
				//if (i == 256 + 0x0e) {	/* 僶僢僋僗儁乕僗 */
				//	if (key_win == sht_win) {	//发送至任务A
				//		if (cursor_x > 8) {
				//			/* 僇乕僜儖傪僗儁乕僗偱徚偟偰偐傜丄僇乕僜儖傪1偮栠偡 */
				//			putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
				//			cursor_x -= 8;
				//		}
				//	}
				//	else {	/* 僐儞僜乕儖傊 */
				//		fifo32_put(&key_win->task->fifo, 8 + 256);
				//	}
				//}
				//if (i == 256 + 0x1c) {	/* Enter */
				//	if (key_win != sht_win) {	//发送至任务A
				//		fifo32_put(&key_win->task->fifo, 10 + 256);
				//	}
				}
				if (i == 256 + 0x0f) {	/* Tab */
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
				if (i == 256 + 0x3b && key_shift != 0) {	// Shift+F1   && task_cons[0]->tss.ss0 != 0  
					//cons = (struct CONSOLE *) *((int *)0x0fec);
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						io_cli();	//强制结束时禁止任务切换
						task_cons[0]->tss.eax = (int) &(task_cons[0]->tss.esp0);
						task_cons[0]->tss.eip = (int)asm_end_app;
						io_sti();
					}
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
				/* 僇乕僜儖偺嵞昞帵 */
				//if (cursor_c >= 0) {
				//	boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				//}
				//sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
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
					sheet_slide(sht_mouse, mx, my);
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
										}
										if (sht->bxsize - 21 <= x&&x < sht->bxsize - 5 && 5 <= y&&y < 19) { //点击位置 x
											if ((sht->flags & 0x10) != 0) {
												//cons = (struct CONSOLE *)*((int *)0x0fec);
												task = sht->task;
												cons_putstr0(task->cons, "\nBreak(mouse) :\n");
												io_cli();
												task_cons[0]->tss.eax = (int) &(task->tss.esp0);
												task_cons[0]->tss.eip = (int)asm_end_app;
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
							sheet_slide(sht, (mmx2 + x + 2)&~3, sht->vy0 + y);
							//sheet_slide(sht, sht->vx0 + x, sht->vy0 + y);
							//mmx = mx;
							mmy = my;
						}
					}
					else {
						mmx = -1;
					}
				}
			}
			//else if (i <= 1) { /* 僇乕僜儖梡僞僀儅 */
			//	if (i != 0) {
			//		timer_init(timer, &fifo, 0); /* 師偼0傪 */
			//		if (cursor_c >= 0) {
			//			cursor_c = COL8_000000;
			//		}
			//	}
			//	else {
			//		timer_init(timer, &fifo, 1); /* 師偼1傪 */
			//		if (cursor_c >= 0) {
			//			cursor_c = COL8_FFFFFF;
			//		}
			//	}
			//	timer_settime(timer, 50);
			//	if (cursor_c >= 0) {
			//		boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
			//		sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			//	}
			//}
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

