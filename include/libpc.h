/*
    Copyright (C) 2009 Li Yuan (liyuan@ss.buaa.edu.cn)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _LIB_PC_H_	/* Avoid multiple inclusions */
#define	_LIB_PC_H_ 1

#include "stdint.h"
/* ------------------------------------------------------------ */
/* Constants for use with datatype BOOL (above).                */
/* ------------------------------------------------------------ */
#ifndef TRUE
#define	TRUE 1
#endif
#ifndef FALSE
#define	FALSE 0
#endif


#define PRIVATE static

/* ------------------------------------------------------------ */
/* 386 instructions needed when writing ISR's.	Note that IRET	*/
/* pops the pointer to the stack frame that was established by	*/
/* code that the compiler generates at every function entry.	*/
/* ------------------------------------------------------------ */
#define	PUSHCS __asm__ __volatile__ ("PUSHL %CS")
#define	PUSHF __asm__ __volatile__ ("PUSHFL")
#define	POPF __asm__ __volatile__ ("POPFL")
#define	STI __asm__ __volatile__ ("STI")
#define	CLI __asm__ __volatile__ ("CLI")
#define	PUSHA __asm__ __volatile__ ("PUSHAL")
#define	POPA __asm__ __volatile__ ("POPAL")
#define	ENTER __asm__ __volatile__ ("ENTER $0,$0")
#define	LEAVE __asm__ __volatile__ ("LEAVE")
#define	IRET __asm__ __volatile__ ("IRET")


/* ------------------------------------------------------------ */
/* Macros to extract the LSByte and MSByte of a WORD16 value	*/
/* ------------------------------------------------------------ */
#define	LSB(u)		((u) & 0xFF)
#define	MSB(u)		((u) >> 8)

/* ------------------------------------------------------------ */
/* Returns number of elements in an array. (Use in for loops.)	*/
/* ------------------------------------------------------------ */
#define	ENTRIES(a)	(sizeof(a)/sizeof(a[0]))

/* ------------------------------------------------------------ */
/* Define a NULL pointer.                                       */
/* ------------------------------------------------------------ */
#ifndef NULL
#define	NULL ((void *) 0)
#endif

/* ------------------------------------------------------------ */
/* Support for functions implemented in INIT-IDT.C              */
/* ------------------------------------------------------------ */
#define IRQ_TICK        0
#define IRQ_KYBD        1
#define IRQ_COM2_COM4   3
#define IRQ_COM1_COM3   4
#define IRQ_HARDDISK    5
#define IRQ_FLOPPY      6
#define IRQ_PAR_PORT    7
#define IRQ_RTC         8

#define IRQ_PS2_MOUSE   12
#define IRQ_HARD_DISK   14

#define IRQ3            3
#define IRQ4            4
#define IRQ5            5
#define IRQ6            6
#define IRQ7            7
#define IRQ9            9
#define IRQ10           10
#define IRQ11           11
#define IRQ12           12
#define IRQ14           14
#define IRQ15           15

typedef void (*ISR)(void);	/* Pointer to an ISR */
int PC_IRQ2INT(int irq) ;
ISR PC_GetISR(int int_numb) ;
void PC_SetISR(int int_numb, ISR isr) ;
void PC_EnableIRQ(unsigned char irq);


/* ------------------------------------------------------------ */
/* Support for functions implemented in INIT-8253.C             */
/* ------------------------------------------------------------ */
void PC_SetTick(uint16_t freq);

/* ------------------------------------------------------------ */
/* Support for functions implemented in SPEAKER.C               */
/* ------------------------------------------------------------ */
void PC_Sound(int hertz) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in IO.ASM                  */
/* ------------------------------------------------------------ */
void outportb(uint16_t port, uint8_t value) ;
uint8_t inportb(uint16_t port) ;


/*************************************************/
/****************** 时间处理 *********************/
/*************************************************/
struct ftime
{
unsigned ft_tsec:
    5;	  /* 0-29, double to get real seconds */
unsigned ft_min:
    6;	  /* 0-59 */
unsigned ft_hour:
    5;	  /* 0-23 */
unsigned ft_day:
    5;	  /* 1-31 */
unsigned ft_month:
    4;	/* 1-12 */
unsigned ft_year:
    7;	  /* since 1980 */
};

struct date
{
    short da_year;
    char  da_day;
    char  da_mon;
};

struct time
{
    unsigned char ti_min;
    unsigned char ti_hour;
    unsigned char ti_hund;
    unsigned char ti_sec;
};

/*
 		index   			Function
			00 			Current second for RTC
			01 			Alarm second
			02 			Current minute
			03 			Alarm minute
			04 			Current hour
			05 			Alarm hour
			06 			Current day of week（01＝Sunday）
			07 			Current date of month
			08 			Current month
			09 			Current year（final two digits，eg：93）
 */
enum RtcRegs
{
    rrSec = 0,
    rrAlmSec,
    rrMin,
    rrAlmMin,
    rrHour,
    rrAlmHour,
    rrWeekday,
    rrMDay,
    rrMonth,
    rrYear
};

unsigned char bcd2hex(unsigned char bcdval);
unsigned char PC_ReadRtcRegs(int index); /* 读RTC时钟寄存器驱动 */
void PC_GetDate( struct date *dp); /* 读日期 */
void PC_GetTime( struct time *tp); /* 读时间 */


static inline unsigned char inb (unsigned short int port)
{
    unsigned char _v;
__asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (port));
    return _v;
}

static inline unsigned char inb_p (unsigned short int port)
{
    unsigned char _v;

__asm__ __volatile__ ("inb %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
    return _v;
}

static inline unsigned short inw (unsigned short int port)
{
    unsigned short _v;
__asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd" (port));
    return _v;
}

static inline unsigned short int inw_p (unsigned short int port)
{
    unsigned short int _v;

__asm__ __volatile__ ("inw %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
    return _v;
}

static inline unsigned int inl (unsigned short int port)
{
    unsigned int _v;
__asm__ __volatile__ ("inl %w1,%0":"=a" (_v):"Nd" (port));
    return _v;
}

static inline unsigned int inl_p (unsigned short int port)
{
    unsigned int _v;
__asm__ __volatile__ ("inl %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
    return _v;
}

static inline void outb (unsigned char value, unsigned short int port)
{
__asm__ __volatile__ ("outb %b0,%w1": :"a" (value), "Nd" (port));
}


static inline void outb_p (unsigned char value, unsigned short int port)
{
__asm__ __volatile__ ("outb %b0,%w1\noutb %%al,$0x80": :"a" (value), "Nd" (port));
}

static inline void outw (unsigned short value, unsigned short int port)
{
__asm__ __volatile__ ("outw %w0,%w1": :"a" (value), "Nd" (port));
}

static inline void outw_p (unsigned short int value, unsigned short int port)
{
__asm__ __volatile__ ("outw %w0,%w1\noutb %%al,$0x80": :"a" (value),
                          "Nd" (port));
}
static inline void outl (unsigned int value, unsigned short int port)
{
__asm__ __volatile__ ("outl %0,%w1": :"a" (value), "Nd" (port));
}


static inline void outl_p (unsigned int value, unsigned short int port)
{
__asm__ __volatile__ ("outl %0,%w1\noutb %%al,$0x80": :"a" (value), "Nd" (port));
}
#endif /*_LIB_PC_H_*/


