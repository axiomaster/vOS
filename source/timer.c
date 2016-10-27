#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

#define TIMER_FLAGS_ALLOC	1
#define TIMER_FLAGS_USING	2

void init_pit(void)
{
	int i;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e); //0x2e9c:11932 -> 中断频率100Hz -> 10ms一次中断
	timerctl.count = 0;
	timerctl.next = 0xffffffff;
	timerctl.using = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0; //未使用
	}
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == 0) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0; //重新标记为未使用
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int e, i, j;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	for (i = 0; i < timerctl.using; i++) { //查找位置
		if (timerctl.timers[i]->timeout >= timer->timeout) {
			break;
		}
	}
	for (j = timerctl.using; j > i; j--) { //逐次后移
		timerctl.timers[j] = timerctl.timers[j - 1];
	}
	timerctl.using++;
	timerctl.timers[i] = timer;
	timerctl.next = timerctl.timers[0]->timeout;
	io_store_eflags(e);
	return;
}

void inthandler20(int *esp)
{
	int i, j;
	io_out8(PIC0_OCW2, 0x60); //IRQ0接收的信息通知给PIC
	timerctl.count++;
	if (timerctl.next > timerctl.count) {
		return;
	}
	for (i = 0; i < timerctl.using; i++) {
		if (timerctl.timers[i]->timeout > timerctl.count)
			break;
		timerctl.timers[i]->flags = TIMER_FLAGS_ALLOC; //已到期的定时器
		fifo32_put(timerctl.timers[i]->fifo, timerctl.timers[i]->data);
	}
	timerctl.using -= i;
	for (j = 0; j < timerctl.using; j++) { //将失效的定时器清出-> 后面的定时器往前挪
		timerctl.timers[j] = timerctl.timers[i + j];
	}
	if (timerctl.using > 0) {
		timerctl.next = timerctl.timers[0]->timeout;
	}
	else {
		timerctl.next = 0xffffffff;
	}
	return;
}