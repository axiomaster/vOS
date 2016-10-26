// asmhead.nas
struct BOOTINFO {
    char cyls, leds, vmode, reserve;
    short scrnx, scrny; //分辨率
    char *vram;
};
#define ADR_BOOTINFO	0x00000ff0

// naskfunc.nas
void io_hlt(void);
void io_cli(void); //中断标记置为0
void io_sti(void); //中断标记置为1
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void); //读取eflags
void io_store_eflags(int eflags); //设置eflags
void load_gdtr(int limit, int addr); //为gdtr寄存器赋值
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
void asm_inthandler20(void); //计时器中断
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);

// fifo.c
struct FIFO8{
    unsigned char *buf;
    int p, q, size, free, flags;
};
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);

// graphic.c
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s); //字符串
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
                 int pysize, int px0, int py0, char *buf, int bxsize);
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

// dsctbl.c
struct SEGMENT_DESCRIPTOR {       //共8个字节
    short limit_low, base_low;    //2,2
    char base_mid, access_right;  //1,1
    char limit_high, base_high;   //1,1
};

struct GATE_DESCRIPTOR {          //8个字节
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092 //系统专用 可读写，不可执行
#define AR_CODE32_ER	0x409a //系统专用 可执行，可读，不可写
#define AR_INTGATE32	0x008e

// int.c
void init_pic(void);
void inthandler27(int *esp);
#define PIC0_ICW1		0x0020 //0x20 - 0x2f 对应 IRQ0 - IRQ15 外部中断
#define PIC0_OCW2		0x0020 //0x00 - 0x1f 对应CPU异常
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

//keyboard.c
void inthandler21(int *esp); //键盘 IRQ1  对应 0x21
void wait_KBC_sendready(void);
void init_keyboard(void);
extern struct FIFO8 keyfifo;
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD     0x0064

//mouse.c
struct MOUSE_DEC{
    unsigned char buf[3]; //鼠标中断产生3字节数据
    unsigned char phase;  //中断数据读取状态
    int x, y, btn;
};
void inthandler2c(int *esp); //鼠标 IRQ12 对应 0x2c
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
extern struct FIFO8 mousefifo;

//memory.c
#define MEMMAN_FREES        4096         //最多4096个区块，32KB
#define MEMMAN_ADDR         0x003c0000

struct FREEINFO
{
	unsigned int addr, size;
};

struct MEMMAN
{
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);

// sheet.c
#define MAX_SHEETS 256
struct SHEET {
	unsigned char *buf; //地址
	int bxsize, bysize, vx0, vy0, col_inv;
	int height;//层高
	int flags; //是否使用

	struct SHTCTL *ctl;
};

struct SHTCTL //管理图层
{
	unsigned char *vram;
	unsigned char *map; //虚拟图层
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
//设置缓冲区大小和透明色
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv); 
// 图层上下移动
void sheet_updown(struct SHEET *sht, int height);
// 刷新
//void sheet_refresh(struct SHTCTL *ctl);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
// 左右移动
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);



// timer.c
#define MAX_TIMER 500
struct TIMER
{
	unsigned int timeout, flags;
	struct FIFO8 *fifo;
	unsigned char data;
};
struct TIMERCTL
{
	unsigned int count;
	struct TIMER timer[MAX_TIMER];
};
extern struct TIMERCTL timerctl;
void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void inthandler20(int *esp);