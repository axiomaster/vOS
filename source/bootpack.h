//
// Created by lism on 2016/10/9.
//
// asmhead.nas
struct BOOTINFO {
    char cyls, leds, vmode, reserve;
    short scrnx, scrny;
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
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

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
struct KEYBUF{
    unsigned char data[32];
    int next_r, next_w, len;
};
void init_pic(void);
void inthandler21(int *esp); //键盘 IRQ1  对应 0x21
void inthandler27(int *esp);
void inthandler2c(int *esp); //鼠标 IRQ12 对应 0x2c
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

