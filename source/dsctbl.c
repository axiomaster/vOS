//
// Created by lism on 2016/10/9.
//
#include "bootpack.h"

void init_gdtidt(void) //gdt表 idt表
{
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000; //270000H - 27ffff
    struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800; //
    int i;

    /* GDTの初期化 */ //2^13 = 8192   8192*8 = 64k
    for (i = 0; i < 8192; i++) {
        set_segmdesc(gdt + i, 0, 0, 0); //每次8个字节递增
    }
    set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092); //段号1 -> 全部4GB
    set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a); //段号2 -> 512KB 对应bootpack.hrb
    load_gdtr(0xffff, 0x00270000); //gdtr寄存器：48位寄存器

    /* IDTの初期化 */
    for (i = 0; i < 256; i++) {
        set_gatedesc(idt + i, 0, 0, 0);
    }
    load_idtr(0x7ff, 0x0026f800);

    // IDT的设定
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32); //绑定中断处理函数
	set_gatedesc(idt + 0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);    

    return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
    if (limit > 0xfffff) {
        ar |= 0x8000; /* G_bit = 1 */ //limit单位 byte -> 4KB
        limit /= 0x1000;
    }
    sd->limit_low    = limit & 0xffff;
    sd->base_low     = base & 0xffff;
    sd->base_mid     = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high    = (base >> 24) & 0xff;
    return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
    gd->offset_low   = offset & 0xffff;
    gd->selector     = selector;
    gd->dw_count     = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high  = (offset >> 16) & 0xffff;
    return;
}