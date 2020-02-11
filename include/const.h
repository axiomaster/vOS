#ifndef _CONST_H_
#define _CONST_H_

/* EXTERN is defined as extern except in global.c */
#define EXTERN extern
#define GDT_SIZE 128
#define IDT_SIZE 256

/* 权限 */
#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

// 中断控制器
#define INT_M_CTL 0x20
#define INT_M_CTLMASK 0x21
#define INT_S_CTL 0xA0
#define INT_S_CTLMASK 0xA1

#endif