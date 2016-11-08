/*
 * (C) Copyright 2002 ELTEC Elektronik AG
 * Frank Gottschling <fgottschling@eltec.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* i8042.h - Intel 8042 keyboard driver header */
#include "ucos_ii.h"
#include "os_cpu.h"

#ifndef _I8042_H_
#define _I8042_H_

/* defines */

#define CFG_ISA_IO          0


/* AT keyboard */
/* 8042 ports */

#define I8042_DATA_REG      (CFG_ISA_IO + 0x0060)    /* keyboard i/o buffer */
                                                     /*Read : Read Output Buffer 
                                                     Write: Write Input Buffer(8042 Data&8048 Command) */
#define I8042_STATUS_REG    (CFG_ISA_IO + 0x0064)    /* keyboard status read */
#define I8042_COMMAND_REG   (CFG_ISA_IO + 0x0064)    /* keyboard ctrl write */
                                                     /*Read : Read Status Register                                                     Write: Write Input Buffer(8042 Command) */
                                                     
#define KB_BUF_SIZE         32
#define KB_DATA_PORT        I8042_DATA_REG

#define I8042_LED_CODE      0xED
#define I8042_ACK           0xFA

#define KBD_US              0        /* default US layout */
#define KBD_GER             1        /* german layout */

#define KBD_TIMEOUT         1000     /* 1 sec */
#define KBD_RESET_TRIES     3



#define MOUSE_BUF_SIZE      0x10



/* exports */

int i8042_kbd_init(void);
unsigned int i8042_getc(void);
void PC_Reboot(void);

typedef struct
{
    unsigned int keyState;
    unsigned int x_pos;
    unsigned int y_pos;
} MOUSE_ENVENT;



extern int mouse_x_pos;                   // 定义 鼠标的初始 x 坐标
extern int mouse_y_pos;                   // 定义 鼠标的初始 y 坐标


extern void MouseISR(void);
int i8042_mouse_init(void);
MOUSE_ENVENT i8042_get_mouse_envent(void);
#endif /* _I8042_H_ */



